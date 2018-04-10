#include <stdlib.h>
#include "quarre-device.hpp"
#include <quarre/device/quarre-protocol-specific-settings.hpp>
#include <quarre/device/quarre-protocol-settings-widget.hpp>
#include <Device/Protocol/DeviceSettings.hpp>

using namespace score::addons;
using namespace ossia::net;
using namespace ossia::oscquery;

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

    for (int i = 0; i < m_n_max_users; ++i)
        m_users.push_back(new User(i));

    make_tree();
}

quarre::Device::~Device() {}

bool quarre::Device::recreate(const Device::Node& n)
{

}

void quarre::Device::recreate(const Device::Node &n)
{

}

void quarre::Device::dispatch_incoming_interaction(quarre::Interaction* interaction)
{
    // candidate algorithm
    // eliminate non-connected clients
    // eliminate clients that cannot support the requested inputs
    // eliminate clients that already have an interaction going on
    // eliminate clients that already have an incoming interaction

    std::vector<quarre::Candidate> candidates;

    for ( const quarre::User* user : m_users )
    {
        quarre::Candidate candidate;

        candidate.user      = user;
        candidate.priority  = 0;

        switch ( user->status() )
        {
        case User::Status::DISCONNECTED:        goto next;
        case User::Status::INCOMING:            goto next;
        case User::Status::INCOMING_ACTIVE:     goto next;

        case User::Status::IDLE:
            goto check_inputs; // priority stays at 0

        case User::Status::ACTIVE:
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

    quarre::Candidate* winner;

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

void quarre::Device::dispatch_active_interaction(quarre::Interaction *interaction)
{
    for ( const auto& user : m_users )
    {
        if ( user.incoming_interaction() == interaction )
            user.set_active_interaction(interaction);
    }
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

