#include <stdlib.h>
#include "quarre-device.hpp"
#include <quarre/device/quarre-protocol-specific-settings.hpp>
#include <quarre/device/quarre-protocol-settings-widget.hpp>
#include <Device/Protocol/DeviceSettings.hpp>

using namespace score::addons;
using namespace ossia::net;
using namespace ossia::oscquery;

// ------------------------------------------------------------------------------
// USER_INPUT
// ------------------------------------------------------------------------------

quarre::user::input::input(std::string id, std::shared_ptr<user> parent) :

    m_id        ( id ),
    m_parent    ( parent )
{
    auto dev            = parent->m_device;

    m_addr = "/user/";
    m_addr += parent->m_id + "/" + id;

    auto n_avail    = ossia::net::create_node(*dev, m_addr + "/available");
    m_available     = std::make_shared<parameter_base>(
                        n_avail.create_parameter(ossia::val_type::BOOL));

    auto n_active   = ossia::net::create_node(*dev, m_addr + "/active");
    m_active        = std::make_shared<parameter_base>(
                        n_active.create_parameter(ossia::val_type::BOOL));
}

void quarre::user::input::assign(
        const std::string &id,
        std::function<void (ossia::value &)> function)
{
    for ( const auto& data : m_data )
    {
        auto addr = ossia::net::osc_parameter_string(*data);
        if ( addr.find(id) == std::string.npos )
            data->add_callback(function);
    }
}

void quarre::user::input::clear_assignment(const std::string &id)
{
    for ( const auto& data : m_data )
    {
        auto addr = ossia::net::osc_parameter_string(*data);
        if ( addr.find(id) == std::string.npos )
            data->callbacks_clear();
    }
}

// ------------------------------------------------------------------------------
// USER_GESTURE
// ------------------------------------------------------------------------------

quarre::user::gesture::gesture(
        std::string id, std::shared_ptr<device_base> parent,
        std::vector<std::string> subgestures )

    : quarre::input ( id, parent )
{

    auto n_trig = ossia::net::create_node(*m_device, m_addr + id + "/trigger");
    auto p_trig = n_trig.create_parameter(ossia::val_type::IMPULSE);

    m_data.push_back(parptr_t (p_trig));

    for ( const auto& subgesture : subgestures )
    {
        auto n_sub = ossia::net::create_node(*m_device, m_addr + subgesture + "/trigger");
        auto p_sub = n_sub.create_parameter(ossia::val_type::IMPULSE);

        m_data.push_back(parptr_t(p_sub));
    }
}

// ------------------------------------------------------------------------------
// USER_SENSOR
// ------------------------------------------------------------------------------

quarre::user::sensor::sensor(
        std::string id, std::shared_ptr<device_base> parent,
        std::vector<pdata_t> data ) :

    quarre::user::input ( id, parent )
{
    for ( const auto& d : data )
    {
        auto ndata = ossia::net::create_node(*m_device, m_addr + "/data/" + d.first());
        auto pdata = ndata.create_parameter(d.second());

        m_data.push_back(pdata_t(pdata));
    }
}

// ------------------------------------------------------------------------------
// USER
// ------------------------------------------------------------------------------

quarre::user::user(uint8_t id, device_base *device) :
    m_id(id), m_device(device)
{

}

bool quarre::user::supports_input(const std::string& input) const
{

}

bool quarre::user::connected()
{
    return m_connected;
}

void quarre::user::set_connected(bool connected)
{
    m_connected = connected;
}

bool quarre::user::interaction_count()
{
    return m_interaction_count;
}

void quarre::user::activate_input(const std::string& input)
{

}

void quarre::user::deactivate_input(const std::string& input)
{

}

int quarre::user::active_countdown() const
{
    return m_active_countdown->value();
}

quarre::intact_t quarre::user::incoming_interaction() const
{
    return m_incoming_interaction;
}

quarre::intact_t quarre::user::active_interaction() const
{
    return m_active_interaction;
}

void quarre::user::set_active_interaction(intact_t interaction)
{
    if ( interaction == m_incoming_interaction )
        m_incoming_interaction.reset();

    m_active_interaction = interaction;
}

void quarre::user::set_incoming_interaction(intact_t interaction)
{
    m_incoming_interaction = interaction;
}

void quarre::user::cancel_next_interaction(intact_t interaction)
{
    m_incoming_interaction.reset();
}

void quarre::user::stop_current_interaction(intact_t interaction)
{
    m_active_interaction.reset();
}

void quarre::user::end_current_interaction(intact_t interaction)
{
    m_active_interaction.reset();
}

void quarre::user::pause_current_interaction(intact_t interaction)
{

}

void quarre::user::resume_current_interaction(intact_t interaction)
{

}

quarre::user::status quarre::user::status() const
{
    return m_status;
}

// ------------------------------------------------------------------------------
// USER_DISPATCHER
// ------------------------------------------------------------------------------

void quarre::Device::user_dispatcher::dispatch_incoming_interaction(intact_t interaction)
{
    // candidate algorithm
    // eliminate non-connected clients
    // eliminate clients that cannot support the requested inputs
    // eliminate clients that already have an interaction going on
    // eliminate clients that already have an incoming interaction

    std::vector<quarre::candidate> candidates;

    for ( const auto& user : m_users )
    {
        quarre::candidate candidate;

        candidate.user      = user;
        candidate.priority  = 0;

        switch ( user->status() )
        {
        case user::status::DISCONNECTED:        goto next;
        case user::status::INCOMING:            goto next;
        case user::status::INCOMING_ACTIVE:     goto next;

        case user::status::IDLE:
            goto check_inputs; // priority stays at 0

        case user::status::ACTIVE:
        {
            if ( interaction->countdown() <
                 candidate.user->active_countdown() + 5) goto next;

            candidate.priority = 1;
            goto check_inputs;
        }
        }

        check_inputs:
        for ( const auto& input : requested_inputs )
            if ( !user->supports_input(input) ) goto ctn;

        select:
        candidate.priority += user->interaction_count();
        candidates.push_back(candidate);

        next:
        continue;
    }

    // the candidate with the lowest priority will be selected
    // if there is no candidate, interaction will not be dispatched
    if ( candidates.size() == 0 ) return;

    quarre::candidate* winner;

    for ( const auto& candidate : candidates )
    {
        if ( ! winner )
        {
            winner = &candidate;
            return;
        }

        // if its a draw between two or more candidates, select randomly between them
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

    if ( winner->user ) winner->user->set_active_interaction(interaction);
}

void quarre::Device::user_dispatcher::dispatch_active_interaction(intact_t interaction)
{
    for ( const auto& user : m_users )
    {
        if ( user.incoming_interaction() == interaction )
            user.set_active_interaction ( interaction );
    }
}

void quarre::Device::user_dispatcher::dispatch_ending_interaction(intact_t interaction)
{
    interaction->host()->end_current_interaction(this);
}

void quarre::Device::user_dispatcher::dispatch_paused_interaction(intact_t interaction)
{
    interaction->host()->pause_current_interaction(this);
}

void quarre::Device::user_dispatcher::dispatch_resumed_interaction(intact_t interaction)
{
    interaction->host()->resume_current_interaction(this);
}

// ------------------------------------------------------------------------------
// DEVICE
// ------------------------------------------------------------------------------

quarre::Device* quarre::Device::instance(const Device::DeviceSettings &settings)
{
    if ( m_singleton ) delete m_singleton;
    m_singleton = new quarre::Device(settings);

    return m_singleton;
}

quarre::Device* quarre::Device::instance()
{
    return m_singleton;
}

quarre::Device::Device(const Device::DeviceSettings &settings) :
    Device::DeviceInterface ( settings )
{
    quarre::SpecificSettings qsettings;

    if ( m_settings.deviceSpecificSettings.canConvert<quarre::SpecificSettings>() )
        qsettings = m_settings.deviceSpecificSettings;

    m_n_max_users = qsettings.max_users;

    auto mpx = std::make_unique<multiplex_protocol>( );
    m_dev = std::make_unique<device_base>( std::move(mpx), m_settings.name );

    auto server     = std::make_unique<oscquery_server_protocol>(
                      qsettings.osc_port, qsettings.ws_port );

    mpx->expose_to  ( std::move(server) );

    make_tree();
}

quarre::Device::~Device() {}

bool quarre::Device::recreate(const Device::Node& n)
{

}

void quarre::Device::recreate(const Device::Node &n)
{

}

void quarre::Device::make_tree()
{        
    // COMMON --------------------------------------------------------------------------------

    make_common_parameter   ( root, "/common/scenario/start",  ossia::val_type::IMPULSE );
    make_common_parameter   ( root, "/common/scenario/end",    ossia::val_type::IMPULSE );
    make_common_parameter   ( root, "/common/scenario/pause",  ossia::val_type::IMPULSE );
    make_common_parameter   ( root, "/common/scenario/stop",   ossia::val_type::IMPULSE );
    make_common_parameter   ( root, "/common/scenario/reset",  ossia::val_type::IMPULSE );

    make_common_parameter   ( root, "/common/scenario/name",        ossia::val_type::STRING );
    make_common_parameter   ( root, "/common/scenario/scene/name",  ossia::val_type::STRING );

    // ID --------------------------------------------------------------------------------

    make_user_parameter     ( root, "/connected", ossia::val_type::BOOL );
    make_user_parameter     ( root, "/address", ossia::val_type::STRING );

    // INTERACTIONS ----------------------------------------------------------------------

    make_user_parameter     ( root, "/interactions/next/incoming", ossia::val_type::LIST );
    make_user_parameter     ( root, "/interactions/next/begin", ossia::val_type::LIST );
    make_user_parameter     ( root, "/interactions/next/cancel", ossia::val_type::INT );
    make_user_parameter     ( root, "/interactions/current/end", ossia::val_type::INT );
    make_user_parameter     ( root, "/interactions/current/force", ossia::val_type::LIST );
    make_user_parameter     ( root, "/interactions/reset", ossia::val_type::IMPULSE );

    // SENSORS --------------------------------------------------------------------------

    make_user_parameter     ( root, "/sensors/{accelerometers,rotation}/data/{x,y,z}", ossia::val_type::FLOAT );
    make_user_parameter     ( root, "/sensors/{accelerometers,rotation,proximity}/{available,active}", ossia::val_type::BOOL );
    make_user_parameter     ( root, "/sensors/proximity/data/close", ossia::val_type::BOOL );

    // GESTURES --------------------------------------------------------------------------

    make_user_parameter     ( root, "/gestures/{whip,cover,turnover,freefall,twist,twist/left,twist/right,shake,shake/left,shake/right,shake/up,shake/down,pickup,swipe}/{active,available}", ossia::val_type::BOOL);
    make_user_parameter     ( root, "/gestures/{whip,cover,turnover,freefall,twist,twist/left,twist/right,shake,shake/left,shake/right,shake/up,shake/down,pickup,swipe}/trigger", ossia::val_type::IMPULSE );


}

