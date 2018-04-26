#include <stdlib.h>
#include "quarre-device.hpp"
#include <quarre/device/quarre-protocol-specific-settings.hpp>
#include <quarre/device/quarre-protocol-settings-widget.hpp>
#include <quarre/panel/quarre-panel-delegate.hpp>
#include <Device/Protocol/DeviceSettings.hpp>
#include <utility>
#include <Engine/OSSIA2score.hpp>
#include <Engine/score2OSSIA.hpp>

using namespace score::addons;
using namespace ossia::net;
using namespace ossia::oscquery;

// ---------------------------------------------------------------------------------------------------------
// USER_GESTURES
// ---------------------------------------------------------------------------------------------------------
static const std::vector<std::pair<std::string,std::vector<std::string>>> g_gestures =
{
    { "whip"        , {}},
    { "cover"       , {}},
    { "turnover"    , {}},
    { "pickup"      , {}},
    { "freefall"    , {}},
    { "shake",      { "left", "right", "up", "down" }},
    { "swipe",      { "left", "right", "up", "down"}},
    { "twist",      { "left", "right" }}
};

// ---------------------------------------------------------------------------------------------------------
// USER_SENSORS
// ---------------------------------------------------------------------------------------------------------
static const std::vector<std::pair<std::string,std::vector<pdata_t>>> g_sensors =
{
    { "accelerometers", {
          { "xyz", ossia::val_type::VEC3F },
          { "x", ossia::val_type::FLOAT },
          { "y", ossia::val_type::FLOAT },
          { "z", ossia::val_type::FLOAT }}},
    { "rotation", {
          { "xyz", ossia::val_type::VEC3F },
          { "x", ossia::val_type::FLOAT },
          { "y", ossia::val_type::FLOAT },
          { "z", ossia::val_type::FLOAT }}},
    { "proximity", {
          { "close", ossia::val_type::BOOL }}}
};

// ---------------------------------------------------------------------------------------------------------
// USER_GRAPHIC_CONTROLLERS
// ---------------------------------------------------------------------------------------------------------

static const std::vector<std::tuple<std::string,uint8_t,ossia::val_type>> g_controllers =
{
    { "pads", 16, ossia::val_type::BOOL },
    { "sliders", 4, ossia::val_type::FLOAT },
    { "xy", 1, ossia::val_type::VEC2F },
    { "strings", 1, ossia::val_type::LIST }
};

// ---------------------------------------------------------------------------------------------------------
// USER_TREE
// ---------------------------------------------------------------------------------------------------------
static const std::vector<pdata_t> g_user_tree =
{
    { "/address", ossia::val_type::STRING },

    { "/interactions/next/incoming", ossia::val_type::LIST },
    //! notifies user of an incoming interaction with following information:
    //! 0 [string] the title of the interaction
    //! 1 [string] the description of the interaction
    //! 2 [string] the interaction module to display on the remote
    //! 3 [ int ] the length of the interaction
    //! 4 [ int ] the time left before the beginning of the interaction

    { "/interactions/next/countdown", ossia::val_type::INT },
    { "/interactions/next/cancel", ossia::val_type::INT },
    //! notifies user that the next interaction is cancelled:
    //! 0 [ int ] the cancellation reason

    { "/interactions/next/begin", ossia::val_type::LIST },
    //! starts the incoming interaction, with following arguments:
    //! 0 [ string ] the title of the interaction
    //! 1 [ string ] the description of the interaction
    //! 2 [ string ] the interaction module to display on the remote
    //! 3 [ int ] the length of the interaction
    //!
    //! note that all of these arguments are optional,
    //! they will be used in case they're different from previous notification
    //! if user did not have notification of an incoming interaction
    //! the interaction will not begin

    { "/interactions/next/pause", ossia::val_type::IMPULSE },
    //! pauses the incoming interaction, countdown will stop

    { "/interactions/next/resume", ossia::val_type::IMPULSE },
    //! resumes the incoming interaction, countdown will start again

    { "/interactions/current/countdown", ossia::val_type::INT },

    { "/interactions/current/end", ossia::val_type::IMPULSE },
    //! the current interaction ends normally, going back to idle or incoming mode

    { "/interactions/current/stop", ossia::val_type::INT },
    //! the current interaction is forcefully stopped

    { "/interactions/current/pause", ossia::val_type::IMPULSE },
    //! pauses the current interaction

    { "/interactions/current/resume", ossia::val_type::IMPULSE },
    //! resumes the current interaction

    { "/interactions/current/force", ossia::val_type::LIST }
    //! starts the interaction, regardless of the current user status
    //! and whether a notification has been sent or not
    //! the arguments are the same as /interactions/next/begin
};

// ---------------------------------------------------------------------------------------------------------
// COMMON_TREE
// ---------------------------------------------------------------------------------------------------------
static const std::vector<pdata_t> g_common_tree =
{
    { "/connections/ids", ossia::val_type::LIST },
    //! binding user id by ipv4 address

    { "/common/interactions/next/incoming", ossia::val_type::LIST },
    { "/common/interactions/next/cancel", ossia::val_type::IMPULSE },
    { "/common/interactions/next/begin", ossia::val_type::LIST },
    { "/common/interactions/current/end", ossia::val_type::IMPULSE },
    { "/common/interactions/current/stop", ossia::val_type::INT },
    { "/common/interactions/current/pause", ossia::val_type::IMPULSE },
    { "/common/interactions/current/resume", ossia::val_type::IMPULSE },
    { "/common/interactions/current/force", ossia::val_type::LIST },
    //! when we want to send an interaction to ALL users

    { "/common/scenario/start", ossia::val_type::IMPULSE },
    //! starts the scenario, activating global counter

    { "/common/scenario/end", ossia::val_type::IMPULSE },
    //! notifies the ending of the scenario, remote goes back into idle mode

    { "/common/scenario/pause", ossia::val_type::IMPULSE },
    //! notifies the pausing of the scenario, all interactions pause
    //! displaying the pause screen

    { "/common/scenario/resume", ossia::val_type::IMPULSE },
    //! notifies scenario resuming

    { "/common/scenario/stop", ossia::val_type::INT },
    //! notifies scenario stopping, because of an error

    { "/common/scenario/reset", ossia::val_type::IMPULSE },
    //! counter is reinitialized to zero

    { "/common/scenario/name", ossia::val_type::STRING },
    //! displays the name of the current scenario

    { "/common/scenario/scene/name", ossia::val_type::STRING }
    //! displays the name of the current scene
};

//---------------------------------------------------------------------------------------------------------
// UTILITY
//---------------------------------------------------------------------------------------------------------

inline parameter_base& quarre::server::get_parameter_from_string ( std::string& address )
{
    auto node   = ossia::net::find_node(server::get_device(), address);
    return      node->get_parameter();
}

inline parameter_base& quarre::server::get_parameter_from_string ( QString& address )
{
    return get_parameter_from_string(address.toStdString());
}

inline parameter_base& quarre::server::get_user_parameter_from_string
( const quarre::user::data& u, std::string& address )
{
    std::string res = u.base_address + address;
    return server::get_parameter_from_string(res);
}

//---------------------------------------------------------------------------------------------------------
// USER
//---------------------------------------------------------------------------------------------------------

quarre::user::data& quarre::user::instantiate_user(uint8_t index)
{
    auto _data = new quarre::user::data;

    _data->active_interaction       = 0;
    _data->incoming_interaction     = 0;
    _data->connected                = false;
    _data->index                    = index;
    _data->status                   = user::status_t::DISCONNECTED;
    _data->base_address             = "/user/";
    _data->base_address             += std::to_string(index);

    return *_data;
}

void quarre::user::initialize_user_node_tree(const quarre::user::data& u)
{
    for ( const auto& parameter : g_user_tree )
    {
        auto node = ossia::net::create_node(server::get_device(), u.base_address + parameter.first);
        node.create_parameter(parameter.second);
    }

    for ( const auto& gesture : g_gestures )
    {
        auto node = ossia::net::create_node(
                    server::get_device(),
                    u.base_address + "/gestures/" + gesture.first);

        auto available_node = node.create_child("available");
        available_node->create_parameter(ossia::val_type::BOOL);

        auto poll_node = node.create_child("poll");
        poll_node->create_parameter(ossia::val_type::BOOL);

        auto trigger_node = node.create_child("trigger");
        trigger_node->create_parameter(ossia::val_type::IMPULSE);

        // create gesture's subnodes

        for ( const auto& subgesture : gesture.second )
        {
            auto subnode = node.create_child(subgesture);
            auto subtrigger = subnode.create_child("trigger");
            subtrigger->create_parameter(ossia::val_type::IMPULSE);
        }
    }

    for ( const auto& sensor : g_sensors )
    {
        auto sensor_node = ossia::net::create_node(
                    server::get_device(),
                    u.base_address + "/sensors/" + sensor.first );

        auto available_node = sensor_node.create_child("available");
        available_node->create_parameter(ossia::val_type::BOOL);

        // create sensor data nodes
        for ( const auto& axis : sensor.second )
        {
            auto subnode    = sensor_node.create_child(axis.first);
            auto poll_node  = subnode.create_child("poll");
            poll_node       ->create_parameter(ossia::val_type::BOOL);

            auto data_node  = subnode.create_child("data");
            data_node       ->create_parameter(axis.second);
        }
    }

    for ( const auto& controller : g_controllers )
    {
        auto controller_node = ossia::net::create_node(
                    server::get_device(),
                    u.base_address + "/controllers/" + std::get<0>(controller) );

        for ( int i = 0; i < std::get<1>(controller); ++i )
        {
            auto subnode        = controller_node.create_child(std::to_string(i));
            auto subnode_data   = subnode->create_child("data");
            subnode_data        ->create_parameter(std::get<2>(controller));
        }
    }
}

inline void replace_user_wildcard(const quarre::user::data& u, QString& target)
{
    target.replace("0", QString::number(u.index));
}

inline void get_input_base_address(QString& target)
{
    if ( target.contains("gestures"))
        target.remove("/trigger");

    else if ( target.contains("sensors"))
        target.remove("/data");

    else return;
}

inline void sanitize_input_name(const quarre::user::data& u, QString& input_name)
{
    if ( input_name.startsWith("quarre-server:"))
        input_name.replace("quarre-server:/user/0", u.base_address);

    else replace_user_wildcard(u, input_name);
}

inline parameter_base& get_input_parameter(const quarre::user::data& u, QString input, QString replacement)
{
    sanitize_input_name         ( u, input );
    get_input_base_address      ( input );
    input.append                ( replacement );

    return quarre::server::get_parameter_from_string(input);
}

inline bool quarre::user::supports_input(const quarre::user::data& u, QString input)
{
    auto parameter = get_input_parameter(u, input, "/available");
    return parameter.value().get<bool>();
}

inline void quarre::user::activate_input(const quarre::user::data &u, QString input)
{
    auto parameter = server::get_input_parameter(u, input, "/poll");
    parameter.push_value(true);
}

inline void quarre::user::deactivate_input(const quarre::user::data &u, QString input)
{
    auto parameter = server::get_input_parameter(u, input, "/poll");
    parameter.push_value(false);
}

inline void quarre::user::update_net_address(quarre::user::data &u, const std::string &net_address)
{
    u.net_address   = net_address;
    auto& parameter = server::get_user_parameter_from_string(u, "/address");

    parameter.push_value(net_address);
}

//---------------------------------------------------------------------------------------------------------
// INTERACTIONS
//---------------------------------------------------------------------------------------------------------

void quarre::user::set_incoming_interaction(quarre::user::data& u, const quarre::interaction& i)
{
    u.incoming_interaction = &i;

    if ( u.status == user::status_t::ACTIVE )
         u.status = user::status_t::INCOMING_ACTIVE;
    else u.status = user::status_t::INCOMING;

    auto& parameter = server::get_user_parameter_from_string(u, "/interactions/next/incoming");
    parameter.push_value(i.to_list());
}

void quarre::user::set_active_interaction(quarre::user::data &u, const quarre::interaction& i)
{
    u.incoming_interaction  = 0;
    u.active_interaction    = &i;
    u.status                = user::status_t::ACTIVE;

    // push interaction to remote
    auto p_active = server::get_user_parameter_from_string(u, "/interactions/next/begin");
    p_active->push_value(i.to_list());

    // set end expression
    auto expr_source    = i.end_expression_source();
    auto& expr          = i.end_expression_js();
    auto& tsync         = i.get_ossia_tsync();

    if ( expr_source != "" )
    {
        activate_input(u, expr_source);
        auto p_expr_source = server::get_user_parameter_from_string(u, expr_source);

        p_expr_source->add_callback([&](const ossia::value&v) {

            QJSValueList arguments;
            QJSValue fun = u.js_engine.evaluate(expr);

            switch (v.getType())
            {
            case ossia::val_type::BOOL: arguments << v.get<bool>(); break;
            case ossia::val_type::INT: arguments << v.get<int>(); break;
            case ossia::val_type::FLOAT: arguments << v.get<float>(); break;
            case ossia::val_type::STRING: arguments << QString::fromStdString(v.get<std::string>()); break;
            case ossia::val_type::LIST: /*arguments << v.get<std::vector<ossia::value>>();*/ break;
            case ossia::val_type::VEC2F: /*arguments << v.get<ossia::vec2f>();*/break;
            case ossia::val_type::VEC3F: /*arguments << v.get<ossia::vec3f>();*/break;
            case ossia::val_type::VEC4F: /*arguments << v.get<ossia::vec4f>();*/ break;
            case ossia::val_type::CHAR: arguments << v.get<char>(); break;
            }

            QJSValue result = fun.call(arguments);

            if ( result.toBool() )
                tsync.trigger_request = true;
        });

    }

    for ( const auto& mapping : interaction->mappings())
    {
        auto map_dest       = mapping->destination();
        auto& map_expr      = mapping->expression_js();
        auto& p_input       = server::get_user_parameter_from_string(u, mapping->source());

        auto state_addr     = State::Address::fromString(map_dest).value_or(State::Address{});
        auto& dlist         = i.get_device_list();
        auto& p_output      = *Engine::score_to_ossia::address(state_addr, dlist);

        p_input->add_callback([&](const ossia::value& v) {

            QJSValueList arguments;
            QJSValue fun = u.js_engine.evaluate(map_expr);

            switch (v.getType())
            {
            case ossia::val_type::BOOL: arguments << v.get<bool>(); break;
            case ossia::val_type::INT: arguments << v.get<int>(); break;
            case ossia::val_type::FLOAT: arguments << v.get<float>(); break;
            case ossia::val_type::STRING: arguments << QString::fromStdString(v.get<std::string>()); break;
            case ossia::val_type::LIST: /*arguments << v.get<std::vector<ossia::value>>();*/ break;
            case ossia::val_type::VEC2F: /*arguments << v.get<ossia::vec2f>();*/break;
            case ossia::val_type::VEC3F: /*arguments << v.get<ossia::vec3f>();*/break;
            case ossia::val_type::VEC4F: /*arguments << v.get<ossia::vec4f>();*/ break;
            case ossia::val_type::CHAR: arguments << v.get<char>(); break;
            }

            QJSValue result = fun.call(arguments);

            switch ( p_output.get_value_type())
            {
            case ossia::val_type::BOOL: p_output.push_value(result.toBool()); break;
            case ossia::val_type::INT: p_output.push_value(result.toInt()); break;
            case ossia::val_type::FLOAT: p_output.push_value(result.toNumber()); break;
            case ossia::val_type::STRING: p_output.push_value(result.toString().toStdString()); break;
            case ossia::val_type::LIST: break; // non-priority for Reaper
            case ossia::val_type::VEC2F: break;
            case ossia::val_type::VEC3F: break;
            case ossia::val_type::VEC4F: break;
            case ossia::val_type::IMPULSE: p_output.push_value(ossia::impulse{}); break;
            case ossia::val_type::CHAR: p_output.push_value(result.toString().toStdString().c_str());
            }
        });
    }
}

void quarre::user::end_interaction(quarre::user::data &u, const quarre::interaction &i)
{
    u.active_interaction = 0;

    if ( u.status == user::status_t::INCOMING_ACTIVE )
         u.status = user::status_t::INCOMING;
    else u.status = user::status_t::IDLE;

    p_end = server::get_user_parameter_from_string(u, "/interactions/current/end");
    p_end->push_value ( ossia::impulse{} );

    auto expr_source = i.end_expression_source();

    if ( expr_source != "")
        quarre::user::deactivate_input(u, expr_source);

    auto p_expr_source = server::get_user_parameter_from_string(u, expr_source);
    p_expr_source.callbacks_clear();

    for ( const auto& mapping : interaction->mappings())
    {
        user::deactivate_input(u, mapping->source());
        auto p_input = server::get_user_parameter_from_string(u, mapping->source());
        p_input->callbacks_clear();
    }
}

inline void quarre::user::pause_current_interaction(quarre::user::data &u, const quarre::interaction &i)
{
    auto p_pause = server::get_user_parameter_from_string(u, "/interactions/current/pause");
    p_pause.push_value ( ossia::impulse{} );
}

inline void quarre::user::resume_current_interaction(quarre::user::data &u, const quarre::interaction &i)
{
    auto p_resume = server::get_user_parameter_from_string(u, "/interactions/current/resume");
    p_resume.push_value ( ossia::impulse{} );
}

//---------------------------------------------------------------------------------------------------------
// DISPATCH INTERACTION
//---------------------------------------------------------------------------------------------------------

void quarre::dispatching::dispatch_incoming_interaction(const quarre::server::data& server, const quarre::interaction &i)
{
    // candidate algorithm
    // eliminate non-connected clients
    // eliminate clients that cannot support the requested inputs
    // eliminate clients that already have an incoming interaction

    std::vector<quarre::dispatching::candidate> candidates;

    for ( const auto& user : server.users )
    {
        quarre::dispatching::candidate candidate;

        candidate.target    = user;
        candidate.priority  = 0;

        switch ( user->status )
        {
        case user::status_t::DISCONNECTED:        goto next;
        case user::status_t::INCOMING:            goto next;
        case user::status_t::INCOMING_ACTIVE:     goto next;

        case user::status_t::IDLE:
            goto check_inputs; // priority stays at 0

        case user::status_t::ACTIVE:
        {
            if ( i.countdown() <
                 candidate.user->interactions()->active_countdown() + 5) goto next;

            candidate.priority = 1;
            goto check_inputs;
        }
        }

        check_inputs:
        for ( const auto& input : i.inputs() )
        {
            bool support = user->supports_input(input);
            if ( !support )
                goto next;
        }

        select:
        candidate.priority += user->interaction_count;
        candidates.push_back(candidate);

        next:
        continue;
    }

    // the candidate with the lowest priority will be selected
    // if there is no candidate, interaction will not be dispatched
    if ( candidates.size() == 0 ) return;

    quarre::dispatching::candidate* winner = 0;

    for ( auto& candidate : candidates )
    {
        if ( ! winner )
        {
            winner = &candidate;
            continue;
        }
        // if its a draw between two or more candidates,
        // select randomly between them
        if ( candidate.priority == winner->priority )
        {
            std::srand ( std::time(nullptr) );
            int r = std::rand();
            r %= 2;

            if ( r ) winner = &candidate;
        }

        else if ( candidate.priority < winner->priority )
            winner = &candidate;
    }

    if ( winner->target )
        user::set_incoming_interaction(*winner->target, i);
}

void quarre::dispatching::dispatch_active_interaction(const quarre::server::data &server, const quarre::interaction &i)
{
    for ( const auto& user : server.users )
    {
        if ( user->incoming_interaction == &i )
            user::set_active_interaction(*user, i);
    }
}

void quarre::dispatching::dispatch_ending_interaction(const quarre::server::data &server, const quarre::interaction &i)
{
    for ( const auto& user : server.users )
    {
        if ( user->active_interaction == &i )
            user::end_interaction(*user, i);
    }
}

void quarre::dispatching::dispatch_paused_interaction(const quarre::server::data &server, const quarre::interaction &i)
{
    for ( const auto& user : server.users )
    {
        if ( user->active_interaction == &i )
            user::pause_interaction(*user, i);
    }
}

void quarre::dispatching::dispatch_resumed_interaction(const quarre::server::data &server, const quarre::interaction &i)
{
    for ( const auto& user : server.users )
    {
        if ( user->active_interaction == &i )
            user::resume_interaction(*user, i);
    }
}

// ------------------------------------------------------------------------------
// DEVICE
// ------------------------------------------------------------------------------

quarre::quarre_device *quarre::quarre_device::instance(const Device::DeviceSettings& settings )
{
    if ( !m_singleton ) m_singleton = new quarre_device(settings);
    return m_singleton;
}

quarre::quarre_device *quarre::quarre_device::instance()
{
    return m_singleton;
}

generic_device& quarre::quarre_device::device()
{
    return *dynamic_cast<generic_device*>(m_dev.get());
}

quarre::server::data::data(const Device::DeviceSettings &settings) :
    OwningOSSIADevice ( settings )
{
    m_capas.canRefreshTree      = true;
    m_capas.canRenameNode       = false;
    m_capas.canSetProperties    = false;
    m_capas.canRemoveNode       = false;

    reconnect();
}

quarre::server::data::~data() {}

bool quarre::server::data::reconnect()
{
    disconnect();
    quarre::SpecificSettings qsettings;

    if ( m_settings.deviceSpecificSettings.canConvert<quarre::SpecificSettings>() )
        qsettings = m_settings.deviceSpecificSettings.value<quarre::SpecificSettings>();

    m_n_max_users   = qsettings.max_users;
    auto server     = std::make_unique<oscquery_server_protocol>(
                      qsettings.osc_port, qsettings.ws_port );

    server->onClientConnected.connect<
            quarre::quarre_device,
            &quarre::quarre_device::on_client_connected>(this);

    server->onClientDisconnected.connect<
            quarre::quarre_device,
            &quarre::quarre_device::on_client_disconnected>(this);

    m_dev = std::make_unique<generic_device>( std::move(server), m_settings.name.toStdString());

    setLogging_impl ( isLogging() );
    enableCallbacks ( );

    // build users
    // note that user 0 is a wildcard:
    // it will select the best candidate to receive the interaction
    auto& gendev = *dynamic_cast<generic_device*>(m_dev.get());

    user_zero = new quarre::user(0, gendev);

    for ( int i = 1; i <= n_max_users; ++i )
        users.push_back(new quarre::user(i, gendev));

    server::make_server_common_tree(*this);

    auto panel = quarre::PanelDelegate::instance();
    panel->on_server_instantiated(*this);

    return connected();
}

void quarre::server::data::recreate(const Device::Node &n)
{
    for ( auto& child : n )
        addNode(child);
}

void quarre::server::on_client_connected(const std::string &ip)
{
    for ( const auto& user : g_server->users )
    {
        if ( !user->connected )
        {
            user->connected = true;
            user::update_net_address ( *user, ip );
            user->status = quarre::user::status_t::IDLE;

            // update panel
            if ( auto panel = quarre::PanelDelegate::instance() )
                quarre::PanelDelegate::instance()->on_user_changed(*user);

            auto qstring_ip     = QString::fromStdString(ip);
            auto splitted_ip    = qstring_ip.split(':');
            auto ip_bis         = splitted_ip[0].toStdString();

            // update id bindings
            auto ids_p = server::get_parameter_from_string("/connections/ids");
            auto ids_v = ids_p->value().get<std::vector<ossia::value>>();

            // in case address already exists ( disconnection failed )
            if ( auto it = std::find(ids_v.begin(), ids_v.end(), ip_bis) != ids_v.end())
                on_client_disconnected ( ip );

            ids_v.push_back(ip_bis);
            ids_v.push_back(user->index());

            ids_p->set_value(ids_v);

            return;
        }
    }
}

void quarre::server::on_client_disconnected(const std::string &ip)
{
    for ( const auto& user : g_server->users )

        if ( user->net_address == ip )
        {
            user->connected     = false;
            user->net_address   = "";
            user->status        = quarre::user::status_t::DISCONNECTED;

            if ( auto panel = quarre::PanelDelegate::instance() )
                quarre::PanelDelegate::instance()->on_user_changed(*user);

            auto qstring_ip = QString::fromStdString(ip);
            auto splitted_ip = qstring_ip.split(':');
            auto ip_bis = splitted_ip[0].toStdString();

            // update id bindings
            auto ids_p = get_parameter_from_string("/connections/ids");
            auto ids_v = ids_p->value().get<std::vector<ossia::value>>();

            ids_v.erase(std::remove(ids_v.begin(), ids_v.end(), ip_bis), ids_v.end());
            ids_v.erase(std::remove(ids_v.begin(), ids_v.end(), user->index()), ids_v.end());

            ids_p->set_value(ids_v);

            return;
        }
}

void quarre::server::make_server_common_tree(quarre::server::data& server)
{        
    for ( const auto& parameter : g_common_tree )
    {
        auto& dev = server::get_device(server);
        auto& node = ossia::net::create_node(dev, parameter.first);
        node.create_parameter(parameter.second);
    }
}

