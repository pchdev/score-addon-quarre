#include <stdlib.h>
#include "quarre-device.hpp"
#include <quarre/device/quarre-protocol-specific-settings.hpp>
#include <quarre/device/quarre-protocol-settings-widget.hpp>
#include <Device/Protocol/DeviceSettings.hpp>

using namespace score::addons;
using namespace ossia::net;
using namespace ossia::oscquery;

#define QSD *quarre::Device::device()

// --------------------------------------------------------------------------------------------
// USER_GESTURES
// --------------------------------------------------------------------------------------------
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

// --------------------------------------------------------------------------------------------
// USER_SENSORS
// --------------------------------------------------------------------------------------------
static const std::vector<std::pair<pdata_t>> g_sensors =
{
    { "accelerometers", {
          { "x", ossia::val_type::FLOAT },
          { "y", ossia::val_type::FLOAT },
          { "z", ossia::val_type::FLOAT }}},
    { "rotation", {
          { "x", ossia::val_type::FLOAT },
          { "y", ossia::val_type::FLOAT },
          { "z", ossia::val_type::FLOAT }}},
    { "proximity", {
          { "close", ossia::val_type::BOOL }}}
};

// --------------------------------------------------------------------------------------------
// USER_TREE
// --------------------------------------------------------------------------------------------
static const std::vector<std::pair<std::string,ossia::val_type>> g_user_tree =
{
    { "/address", ossia::val_type::STRING },

    { "/interactions/next/incoming", ossia::val_type::LIST },
    //! notifies user of an incoming interaction with following information:
    //! 0 [string] the title of the interaction
    //! 1 [string] the description of the interaction
    //! 2 [string] the interaction module to display on the remote
    //! 3 [ int ] the length of the interaction
    //! 4 [ int ] the time left before the beginning of the interaction

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

// --------------------------------------------------------------------------------------------
// COMMON_TREE
// --------------------------------------------------------------------------------------------
static const std::vector<std::pair<std::string,ossia::val_type>> g_common_tree =
{
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

// ------------------------------------------------------------------------------
// USER_INPUT
// ------------------------------------------------------------------------------

quarre::user::input::input(std::string id) : m_id ( id )
{
    auto dev  = m_device;

    m_addr = "/user/";
    m_addr += m_id + "/" + id;

    auto n_avail    = ossia::net::create_node(QSD, m_addr + "/available");
    m_available     = parptr_t(n_avail.create_parameter(ossia::val_type::BOOL));

    auto n_active   = ossia::net::create_node(QSD, m_addr + "/active");
    m_active        = parptr_t(n_active.create_parameter(ossia::val_type::BOOL));
}

void quarre::user::input::assign(const std::string &id,
        std::function<(ossia::value&)> function)
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

quarre::user::gesture::gesture(std::string id, std::vector<std::string> subgestures)
    : quarre::input ( id )
{
    auto n_trig = ossia::net::create_node(QSD, m_addr + id + "/trigger");
    auto p_trig = n_trig.create_parameter(ossia::val_type::IMPULSE);

    m_data.push_back(parptr_t (p_trig));

    for ( const auto& subgesture : subgestures )
    {
        auto n_sub = ossia::net::create_node(QSD, m_addr + subgesture + "/trigger");
        auto p_sub = n_sub.create_parameter(ossia::val_type::IMPULSE);

        m_data.push_back(parptr_t(p_sub));
    }
}

// ------------------------------------------------------------------------------
// USER_SENSOR
// ------------------------------------------------------------------------------

quarre::user::sensor::sensor(
        std::string id,
        std::vector<pdata_t> data ) :

    quarre::user::input ( id )
{
    for ( const auto& d : data )
    {
        auto ndata = ossia::net::create_node(QSD, m_addr + "/data/" + d.first());
        auto pdata = ndata.create_parameter(d.second());

        m_data.push_back(pdata_t(pdata));
    }
}

// ------------------------------------------------------------------------------
// USER
// ------------------------------------------------------------------------------

quarre::user::user(uint8_t id, device_base *device) :
    m_id(id), m_status(quarre::user::status::DISCONNECTED)
{
    const std::string base_addr = "/user/";
    base_addr += std::to_string(m_id);

    // make user tree
    for ( const auto& parameter : g_user_tree )
    {
        auto node = ossia::net::create_node(QSD, base_addr + parameter.first());
        node.create_parameter(parameter.second());
    }

    for ( const auto& input : g_gestures )
    {
        auto gest = new quarre::user::gesture(input.first(), input.second());
        m_inputs.push_back(gest);
    }

    for ( const auto& sensor : g_sensors )
    {
        auto sens = new quarre::user::sensor(input.first(), input.second());
        m_inputs.push_back(sens);
    }
}

bool quarre::user::supports_input(const std::string& input) const
{
    for ( const auto& input : m_inputs )
    {
        if ( input->m_id == input )
            return input->m_available;
    }
}

bool quarre::user::connected()
{
    return m_connected;
}

void quarre::user::set_connected(bool connected)
{
    m_connected = connected;
}

void quarre::user::set_address(const std::string &address)
{
    m_address = address;
}

std::string const& quarre::user::address()
{
    return m_address;
}

quarre::user::status quarre::user::status() const
{
    return m_status;
}

void quarre::user::set_status(const status &st)
{
    m_status = st;
}

bool quarre::user::interaction_count()
{
    return m_interaction_hdl.m_interaction_count;
}

void quarre::user::activate_input(const std::string& input)
{
    for ( const auto& input : m_inputs )
    {
        if ( input->m_id == input )
             input->m_active->set_value(true);
    }
}

void quarre::user::deactivate_input(const std::string& input)
{
    for ( const auto& input : m_inputs )
    {
        if ( input->m_id == input )
             input->m_active->set_value(false);
    }
}

int quarre::user::active_countdown() const
{
    return m_interaction_hdl.m_active_countdown->value();
}

quarre::intact_t quarre::user::incoming_interaction() const
{
    return m_interaction_hdl.m_incoming_interaction;
}

quarre::intact_t quarre::user::active_interaction() const
{
    return m_interaction_hdl.m_active_interaction;
}

void quarre::user::interaction_hdlr::set_active_interaction(intact_t interaction)
{
    if ( interaction == m_incoming_interaction )
        m_incoming_interaction.reset();

    m_active_interaction = interaction;
}

void quarre::user::interaction_hdlr::set_incoming_interaction(intact_t interaction)
{
    m_incoming_interaction = interaction;
}

void quarre::user::interaction_hdlr::cancel_next_interaction(intact_t interaction)
{
    m_incoming_interaction.reset();
}

void quarre::user::interaction_hdlr::stop_current_interaction(intact_t interaction)
{
    m_active_interaction.reset();
}

void quarre::user::interaction_hdlr::end_current_interaction(intact_t interaction)
{
    m_active_interaction.reset();
}

void quarre::user::interaction_hdlr::pause_current_interaction(intact_t interaction)
{

}

void quarre::user::interaction_hdlr::resume_current_interaction(intact_t interaction)
{

}

// ------------------------------------------------------------------------------
// USER_DISPATCHER
// ------------------------------------------------------------------------------

using namespace quarre::Device;

void quarre::Device::dispatcher::dispatch_incoming_interaction(intact_t interaction)
{
    // candidate algorithm
    // eliminate non-connected clients
    // eliminate clients that cannot support the requested inputs
    // eliminate clients that already have an interaction going on
    // eliminate clients that already have an incoming interaction

    std::vector<dispatcher::candidate> candidates;

    for ( const auto& user : m_users )
    {
        dispatcher::candidate candidate;

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
                 candidate.user->interaction_hdlr().active_countdown() + 5) goto next;

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

    dispatcher::candidate* winner;

    for ( const auto& candidate : candidates )
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

    if ( winner->user )
        winner->user->set_active_interaction(interaction);
}

void quarre::Device::dispatcher::dispatch_active_interaction(intact_t interaction)
{
    for ( const auto& user : m_users )
    {
        if ( user.incoming_interaction() == interaction )
            user.set_active_interaction ( interaction );
    }
}

void quarre::Device::dispatcher::dispatch_ending_interaction(intact_t interaction)
{
    interaction->host()->end_current_interaction(this);
}

void quarre::Device::dispatcher::dispatch_paused_interaction(intact_t interaction)
{
    interaction->host()->pause_current_interaction(this);
}

void quarre::Device::dispatcher::dispatch_resumed_interaction(intact_t interaction)
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

quarre::Device::dispatcher const& quarre::Device::dispatcher()
{
    return m_dispatcher;
}

const std::unique_ptr<device_base>& quarre::Device::device()
{
    return m_dev;
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

    // build users
    for ( int i = 0; i < m_n_max_users; ++i )
        m_dispatcher.m_users.push_back(new quarre::user(i, m_dev.get()));

    // make common tree
    make_tree();
}

quarre::Device::~Device() {}

bool quarre::Device::reconnect()
{

}

void quarre::Device::recreate(const Device::Node &n)
{

}

void quarre::Device::on_client_connected(const std::string &ip)
{
    for ( const auto& user : m_dispatcher.m_users )
    {
        if ( !user.connected() )
        {
            user.set_connected  ( true );
            user.set_address    ( ip );
            user.set_status     ( quarre::user::status::IDLE );
            return;
        }
    }
}

void quarre::Device::on_client_disconnected(const std::string &ip)
{
    for ( const auto& user : m_dispatcher.m_users )
        if ( user.address() == ip )
        {
            user.set_connected  ( false );
            user.set_address    ( "" );
            user.set_status     ( quarre::user::status::DISCONNECTED );
        }
}

uint8_t quarre::Device::max_users()
{
    return m_n_max_users;
}

void quarre::Device::make_common_tree()
{        
    for ( const auto& parameter : g_common_tree )
    {
        auto node = ossia::net::create_node(*m_dev, parameter.first());
        node.create_parameter(parameter.second());
    }
}

