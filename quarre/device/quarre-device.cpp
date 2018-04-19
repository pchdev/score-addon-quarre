#include <stdlib.h>
#include "quarre-device.hpp"
#include <quarre/device/quarre-protocol-specific-settings.hpp>
#include <quarre/device/quarre-protocol-settings-widget.hpp>
#include <Device/Protocol/DeviceSettings.hpp>
#include <utility>

using namespace score::addons;
using namespace ossia::net;
using namespace ossia::oscquery;

quarre::quarre_device* quarre::quarre_device::m_singleton;
#define QSD quarre::quarre_device::instance()->device()

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
static const std::vector<std::pair<std::string,std::vector<pdata_t>>> g_sensors =
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

quarre::user::input::input(std::string addr, generic_device& device) : m_addr ( addr )
{
    auto& n_avail    = ossia::net::create_node(device, addr + "/available");
    m_available      = parptr_t(n_avail.create_parameter(ossia::val_type::BOOL));

    auto& n_active   = ossia::net::create_node(device, addr + "/active");
    m_active         = parptr_t(n_active.create_parameter(ossia::val_type::BOOL));
}

void quarre::user::input::assign(const std::string &id,
        std::function<void(const ossia::value&)> function)
{
    for ( const auto& data : m_data )
    {
        auto addr = ossia::net::osc_parameter_string(*data);
        if ( addr.find(id) == std::string::npos )
            data->add_callback(function);
    }
}

void quarre::user::input::unassign(const std::string &id)
{
    for ( const auto& data : m_data )
    {
        auto addr = ossia::net::osc_parameter_string(*data);
        if ( addr.find(id) == std::string::npos )
            data->callbacks_clear();
    }
}

// ------------------------------------------------------------------------------
// USER_GESTURE
// ------------------------------------------------------------------------------

quarre::user::gesture::gesture(
        std::string addr, std::vector<std::string> subgestures,
        generic_device& device)
    : quarre::user::input ( addr, device )
{
    auto& n_trig    = ossia::net::create_node(device, m_addr + "/trigger");
    auto p_trig     = n_trig.create_parameter(ossia::val_type::IMPULSE);

    m_data.push_back(parptr_t (p_trig));

    for ( const auto& subgesture : subgestures )
    {
        auto& n_sub = ossia::net::create_node(device, m_addr + "/" + subgesture + "/trigger");
        auto p_sub = n_sub.create_parameter(ossia::val_type::IMPULSE);

        m_data.push_back(parptr_t(p_sub));
    }
}

// ------------------------------------------------------------------------------
// USER_SENSOR
// ------------------------------------------------------------------------------

quarre::user::sensor::sensor(std::string addr, std::vector<pdata_t> data, generic_device& device) :

    quarre::user::input ( addr, device)
{
    for ( const auto& d : data )
    {
        auto& ndata     = ossia::net::create_node(device, m_addr + "/data/" + d.first);
        auto pdata      = ndata.create_parameter(d.second);

        m_data.push_back(parptr_t(pdata));
    }
}

// ------------------------------------------------------------------------------
// USER
// ------------------------------------------------------------------------------

quarre::user::user(uint8_t id, generic_device& device) :
    m_id ( id ), m_status ( quarre::user::status::DISCONNECTED ),
    m_interaction_hdl(new interaction_hdl(*this))
{
    m_address = "/user/";
    m_address += std::to_string(m_id);

    auto gest_addr = m_address + "/gestures/";
    auto sens_addr = m_address + "/sensors/";

    // make user tree
    for ( const auto& parameter : g_user_tree )
    {
        auto& node = ossia::net::create_node(device, m_address + parameter.first);
        node.create_parameter(parameter.second);
    }

    for ( const auto& input : g_gestures )
    {
        auto gest = new quarre::user::gesture(gest_addr + input.first, input.second, device);
        m_inputs.push_back(gest);
    }

    for ( const auto& sensor : g_sensors )
    {
        auto sens = new quarre::user::sensor(sens_addr + sensor.first, sensor.second, device);
        m_inputs.push_back(sens);
    }
}

std::string quarre::user::input::address()
{
    return m_addr;
}

bool quarre::user::supports_input(const std::string& target) const
{
    for ( auto& input : m_inputs )
    {
        std::string stripped_input = input->address();
        stripped_input.erase(0, 7);
        qDebug() << stripped_input;

        if ( stripped_input == target )
            return input->m_available->value().get<bool>();
    }

    return false;
}

bool quarre::user::connected() const
{
    return m_connected;
}

void quarre::user::set_connected(bool connected)
{
    m_connected = connected;
}

inline parameter_base* get_parameter_from_string ( std::string address )
{
    auto& dev   = quarre::quarre_device::instance()->device();
    auto node   = ossia::net::find_node(dev, address);
    return node->get_parameter();
}

void quarre::user::set_address(const std::string &address)
{
    auto param = get_parameter_from_string(m_address+"/address");
    auto& addr = param->set_value(address);
}

std::string const& quarre::user::address() const
{
    auto param = get_parameter_from_string(m_address+"/address");
    auto& addr = param->value().get<std::string>();

    return addr;
}

enum quarre::user::status quarre::user::status() const
{
    return m_status;
}

void quarre::user::set_status(const enum status &st)
{
    m_status = st;
}

uint8_t quarre::user::interaction_hdl::interaction_count() const
{
    return m_interaction_count;
}

void quarre::user::activate_input(const std::string& target)
{
    for ( const auto& input : m_inputs )
    {
        if ( input->m_id == target )
             input->m_active->set_value(true);
    }
}

void quarre::user::deactivate_input(const std::string& target)
{
    for ( const auto& input : m_inputs )
    {
        if ( input->m_id == target )
             input->m_active->set_value(false);
    }
}


quarre::user::interaction_hdl::interaction_hdl(const quarre::user &parent)
    : m_user(parent)
{

}

quarre::user::interaction_hdl& quarre::user::interactions()
{
    return *m_interaction_hdl;
}

int quarre::user::interaction_hdl::active_countdown() const
{
    return m_active_countdown->value().get<int>();
}

quarre::interaction* quarre::user::interaction_hdl::incoming_interaction() const
{
    return m_incoming_interaction;
}

quarre::interaction* quarre::user::interaction_hdl::active_interaction() const
{
    return m_active_interaction;
}

void quarre::user::interaction_hdl::set_active_interaction(quarre::interaction* interaction)
{
    if ( interaction == m_incoming_interaction )
        m_incoming_interaction = 0;

    m_active_interaction = interaction;

    auto p_act = get_parameter_from_string(m_user.m_address+"/interactions/next/next/begin");
    p_act->set_value(interaction->to_list());
}


void quarre::user::interaction_hdl::set_incoming_interaction(quarre::interaction* interaction)
{
    qDebug() << m_user.m_address;

    m_incoming_interaction = interaction;    
    auto p_inc = get_parameter_from_string(m_user.m_address+"/interactions/next/incoming");

    p_inc->set_value(interaction->to_list());
}

void quarre::user::interaction_hdl::cancel_next_interaction(quarre::interaction* interaction)
{
    m_incoming_interaction = 0;
}

void quarre::user::interaction_hdl::stop_current_interaction(quarre::interaction* interaction)
{
    m_active_interaction = 0;
}

void quarre::user::interaction_hdl::end_current_interaction(quarre::interaction* interaction)
{
    m_active_interaction = 0;

    auto p_end = get_parameter_from_string(m_user.m_address+"/interactions/current/end");
    p_end->set_value ( ossia::impulse{} );
}

void quarre::user::interaction_hdl::pause_current_interaction(quarre::interaction* interaction)
{

}

void quarre::user::interaction_hdl::resume_current_interaction(quarre::interaction* interaction)
{

}

// ------------------------------------------------------------------------------
// USER_DISPATCHER
// ------------------------------------------------------------------------------

void quarre::quarre_device::dispatch_incoming_interaction(quarre::interaction* interaction)
{
    // candidate algorithm
    // eliminate non-connected clients
    // eliminate clients that cannot support the requested inputs
    // eliminate clients that already have an incoming interaction

    std::vector<quarre_device::candidate> candidates;

    for ( const auto& user : m_users )
    {
        quarre_device::candidate candidate;

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
                 candidate.user->interactions().active_countdown() + 5) goto next;

            candidate.priority = 1;
            goto check_inputs;
        }
        }

        check_inputs:
        for ( const auto& input : interaction->inputs() )
            if ( !user->supports_input(input.toStdString()) ) goto next;

        select:
        candidate.priority += user->interactions().interaction_count();
        candidates.push_back(candidate);

        next:
        continue;
    }

    // the candidate with the lowest priority will be selected
    // if there is no candidate, interaction will not be dispatched
    if ( candidates.size() == 0 ) return;

    quarre_device::candidate* winner;

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

    if ( winner->user )
        winner->user->interactions().set_incoming_interaction(interaction);
}

void quarre::quarre_device::dispatch_active_interaction(quarre::interaction* interaction)
{
    for ( const auto& user : m_users )
    {
        if ( user->interactions().incoming_interaction() == interaction )
            user->interactions().set_active_interaction ( interaction );
    }
}

void quarre::quarre_device::dispatch_ending_interaction(quarre::interaction* interaction)
{
    for ( const auto& user : m_users )
    {
        if ( user->interactions().active_interaction() == interaction )
            user->interactions().end_current_interaction ( interaction );
    }
}

void quarre::quarre_device::dispatch_paused_interaction(quarre::interaction* interaction)
{
    for ( const auto& user : m_users )
    {
        if ( user->interactions().active_interaction() == interaction )
            user->interactions().pause_current_interaction( interaction );
    }
}

void quarre::quarre_device::dispatch_resumed_interaction(quarre::interaction* interaction)
{
    for ( const auto& user : m_users )
    {
        if ( user->interactions().active_interaction() == interaction )
            user->interactions().resume_current_interaction( interaction );
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

quarre::quarre_device::quarre_device(const Device::DeviceSettings &settings) :
    OwningOSSIADevice ( settings )
{
    m_capas.canRefreshTree      = true;
    m_capas.canRenameNode       = false;
    m_capas.canSetProperties    = false;
    m_capas.canRemoveNode       = false;

    connect(this, &quarre_device::sig_command,
            this, &quarre_device::slot_command, Qt::QueuedConnection);

    reconnect();
}

quarre::quarre_device::~quarre_device() {}

bool quarre::quarre_device::reconnect()
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

    setLogging_impl(isLogging());
    enableCallbacks();

    // build users
    // note that user 0 is a wildcard:
    // it will select the best candidate to receive the interaction
    auto& gendev = *dynamic_cast<generic_device*>(m_dev.get());

    m_user_zero = new quarre::user(0, gendev);

    for ( int i = 1; i <= m_n_max_users; ++i )
        m_users.push_back(new quarre::user(i, gendev));

    // make common tree
    make_common_tree();
    return connected();
}

void quarre::quarre_device::recreate(const Device::Node &n)
{
    for ( auto& child : n )
        addNode(child);
}

void quarre::quarre_device::on_client_connected(const std::string &ip)
{
    qDebug() << ip;
    for ( const auto& user : m_users )
    {
        if ( !user->connected() )
        {
            user->set_connected  ( true );
            user->set_address    ( ip );
            user->set_status     ( quarre::user::status::IDLE );
            return;
        }
    }
}

void quarre::quarre_device::on_client_disconnected(const std::string &ip)
{
    qDebug() << ip;
    for ( const auto& user : m_users )
        if ( user->address() == ip )
        {
            user->set_connected  ( false );
            user->set_address    ( "" );
            user->set_status     ( quarre::user::status::DISCONNECTED );
        }
}

uint8_t quarre::quarre_device::max_users() const
{
    return m_n_max_users;
}

void quarre::quarre_device::make_common_tree()
{        
    for ( const auto& parameter : g_common_tree )
    {
        auto& gendev = *dynamic_cast<generic_device*>(m_dev.get());
        auto& node = ossia::net::create_node(gendev, parameter.first);
        node.create_parameter(parameter.second);
    }
}

void quarre::quarre_device::slot_command()
{

}

