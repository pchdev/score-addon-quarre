#include <stdlib.h>
#include "quarre-device.hpp"
#include <quarre/device/quarre-protocol-specific-settings.hpp>
#include <quarre/device/quarre-protocol-settings-widget.hpp>
#include <quarre/panel/quarre-panel-delegate.hpp>
#include <Device/Protocol/DeviceSettings.hpp>
#include <utility>
#include <Engine/OSSIA2score.hpp>
#include <Engine/score2OSSIA.hpp>
#include <QJSValueIterator>

using namespace score::addons;
using namespace ossia::net;
using namespace ossia::oscquery;

quarre::server* quarre::server::m_singleton = 0;

static const std::vector<pdata_t> g_common_tree =
{
    { "/connections/ids", ossia::val_type::LIST, true },
    //! binding user id by ipv4 address

    { "/common/num_users", ossia::val_type::INT, true },
    { "/common/modules/strings/new", ossia::val_type::INT, true },
    { "/common/modules/strings/trigger", ossia::val_type::IMPULSE, true },
    { "/common/modules/strings/new2", ossia::val_type::INT, true },
    { "/common/modules/strings/trigger2", ossia::val_type::IMPULSE, true },

    { "/common/interactions/next/incoming", ossia::val_type::LIST, true },
    { "/common/interactions/next/cancel", ossia::val_type::IMPULSE, true },
    { "/common/interactions/next/begin", ossia::val_type::LIST, true },
    { "/common/interactions/current/end", ossia::val_type::IMPULSE, true },
    { "/common/interactions/current/stop", ossia::val_type::INT, true },
    { "/common/interactions/current/pause", ossia::val_type::IMPULSE, true },
    { "/common/interactions/current/resume", ossia::val_type::IMPULSE, true },
    { "/common/interactions/current/force", ossia::val_type::LIST, true},
    //! when we want to send an interaction to ALL users

    { "/common/vote/result", ossia::val_type::INT, true },
    { "/common/scenario/start", ossia::val_type::IMPULSE, true },
    //! starts the scenario, activating global counter

    { "/common/scenario/end", ossia::val_type::IMPULSE, true },
    //! notifies the ending of the scenario, remote goes back into idle mode

    { "/common/scenario/pause", ossia::val_type::IMPULSE, true },
    //! notifies the pausing of the scenario, all interactions pause
    //! displaying the pause screen

    { "/common/scenario/resume", ossia::val_type::IMPULSE, true },
    //! notifies scenario resuming

    { "/common/scenario/stop", ossia::val_type::INT, true },
    //! notifies scenario stopping, because of an error

    { "/common/scenario/reset", ossia::val_type::IMPULSE, true },
    //! counter is reinitialized to zero

    { "/common/scenario/name", ossia::val_type::STRING, true },
    //! displays the name of the current scenario

    { "/common/transitions/name", ossia::val_type::STRING, true },

    { "/common/scenario/scene/name", ossia::val_type::STRING, true }
    //! displays the name of the current scene
};

parameter_base& quarre::server::make_parameter ( std::string name, ossia::val_type type, bool critical )
{
    auto& node = ossia::net::create_node(get_device(), name);
    ossia::net::set_critical(node, critical);
    return *node.create_parameter(type);
}

parameter_base& quarre::server::make_user_parameter(uint8_t index, std::string addr, ossia::val_type type, bool critical)
{
    std::string target = "/user/";

    if ( index == 0 ) target += "wildcard";
    else target += std::to_string(index);
    target += addr;

    return make_parameter(target, type, critical);
}

parameter_base* quarre::server::get_common_parameter ( std::string address )
{
    auto node   = ossia::net::find_node(get_device(), address);
    return      node->get_parameter();
}

parameter_base* quarre::server::get_common_parameter ( QString& address )
{
    auto node   = ossia::net::find_node(get_device(), address.toStdString());
    return      node->get_parameter();
}

parameter_base* quarre::server::get_user_parameter(
        const quarre::user &usr, std::string address )
{
    std::string res = usr.m_base_address + address;
    return server::get_common_parameter(res);
}

quarre::server& quarre::server::instance()
{
    return *m_singleton;
}

inline generic_device& quarre::server::get_device()
{
    return *dynamic_cast<generic_device*>(m_dev.get());
}

quarre::server::server(const Device::DeviceSettings &settings) :
    OwningOSSIADevice ( settings )
{
    m_capas.canRefreshTree      = true;
    m_capas.canRenameNode       = true;
    m_capas.canSetProperties    = true;
    m_capas.canAddNode          = true;
    m_capas.canRemoveNode       = true;
    m_singleton                 = this;

    reconnect();
}

bool quarre::server::reconnect()
{
    disconnect();
    quarre::SpecificSettings qsettings;

    if ( m_settings.deviceSpecificSettings.canConvert<quarre::SpecificSettings>() )
        qsettings = m_settings.deviceSpecificSettings.value<quarre::SpecificSettings>();

    m_n_max_users   = qsettings.max_users;
    auto server     = std::make_unique<oscquery_server_protocol>(
                      qsettings.osc_port, qsettings.ws_port );

    server->onClientConnected.connect<
            quarre::server,
            &quarre::server::on_client_connected>(this);

    server->onClientDisconnected.connect<
            quarre::server,
            &quarre::server::on_client_disconnected>(this);

    m_dev = std::make_unique<generic_device>( std::move(server), m_settings.name.toStdString());

    setLogging_impl ( isLogging() );
    enableCallbacks ( );

    m_user_zero = new quarre::user ( 0, *this );

    for ( int i = 1; i <= m_n_max_users; ++i )
        m_users.push_back(new quarre::user(i, *this));

    make_common_tree();

    auto panel = quarre::PanelDelegate::instance();
    panel->on_server_instantiated(*this);

    return connected();
}

void quarre::server::recreate(const Device::Node &n)
{
    for ( auto& child : n )
        addNode(child);
}

void quarre::server::on_client_connected(const std::string &ip)
{
    for ( const auto& user : m_users )
    {
        if ( !user->m_connected )
        {
            user->m_connected = true;
            user->update_net_address ( ip );
            user->m_status = quarre::user_status::IDLE;

            // update panel
            if ( auto panel = quarre::PanelDelegate::instance() )
                quarre::PanelDelegate::instance()->on_user_changed(*user);

            auto qstring_ip     = QString::fromStdString(ip);
            auto splitted_ip    = qstring_ip.split(':');
            auto ip_bis         = splitted_ip[0].toStdString();

            // update id bindings

            std::string cids    = "/connections/ids";
            auto& ids_p         = *get_common_parameter(cids);
            auto ids_v          = ids_p.value().get<std::vector<ossia::value>>();

            // in case address already exists ( disconnection failed )
            if ( auto it = std::find(ids_v.begin(), ids_v.end(), ip_bis) != ids_v.end())
                on_client_disconnected ( ip );

            ids_v.push_back(ip_bis);
            ids_v.push_back(user->m_index);

            ids_p.set_value(ids_v);

            return;
        }
    }
}

void quarre::server::on_client_disconnected(const std::string &ip)
{
    for ( const auto& user : m_users )

        if ( user->m_net_address == ip )
        {
            user->m_connected     = false;
            user->m_net_address   = "";
            user->m_status        = quarre::user_status::DISCONNECTED;

            if ( auto panel = quarre::PanelDelegate::instance() )
                quarre::PanelDelegate::instance()->on_user_changed(*user);

            auto qstring_ip = QString::fromStdString(ip);
            auto splitted_ip = qstring_ip.split(':');
            auto ip_bis = splitted_ip[0].toStdString();

            // update id bindings
            std::string cids = "/connections/ids";
            auto& ids_p = *get_common_parameter(cids);
            auto ids_v = ids_p.value().get<std::vector<ossia::value>>();

            ids_v.erase(std::remove(ids_v.begin(), ids_v.end(), ip_bis), ids_v.end());
            ids_v.erase(std::remove(ids_v.begin(), ids_v.end(), user->m_index), ids_v.end());

            ids_p.set_value(ids_v);

            return;
        }
}

void quarre::server::make_common_tree()
{        
    for ( const auto& parameter : g_common_tree )
    {
        auto& node = ossia::net::create_node(get_device(), parameter.address);
        node.create_parameter(parameter.type);
        ossia::net::set_critical(node, parameter.critical);
    }
}

void quarre::server::parse_vote_result()
{
    uint8_t res_zero    = 0;
    uint8_t res_one     = 0;
    uint8_t res_two     = 0;
    uint8_t total       = 0;

    for ( const auto& user : m_users )
    {
        if ( user->m_status != user_status::DISCONNECTED )
        {
            auto& choice = *get_user_parameter(*user, "/vote/choice");
            if ( choice.value().get<int>() == 0 ) res_zero++;
            else if ( choice.value().get<int>() == 1 ) res_one++;
            else if ( choice.value().get<int>() == 2 ) res_two++;
        }
    }

    if ( res_zero > res_one && res_zero > res_two )
    {
        std::srand ( std::time(nullptr) );
        total = std::rand();
        total %= 2;
    }

    else if ( res_one > res_zero && res_one > res_two )
        total = 0;

    else if ( res_two > res_zero && res_two > res_one )
        total = 1;

    std::string res_str = "/common/vote/result";
    auto& res_p = *get_common_parameter(res_str);
    res_p.push_value(total);
}

