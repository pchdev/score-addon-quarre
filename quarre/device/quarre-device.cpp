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

    { "/vote/choice", ossia::val_type::INT },
    { "/controllers/trajectories/trigger", ossia::val_type::BOOL },
    { "/controllers/birds/trigger", ossia::val_type::VEC3F },

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

    { "/common/num_users", ossia::val_type::INT },
    { "/common/sensors/rotation/offset", ossia::val_type::FLOAT },

    { "/common/interactions/next/incoming", ossia::val_type::LIST },
    { "/common/interactions/next/cancel", ossia::val_type::IMPULSE },
    { "/common/interactions/next/begin", ossia::val_type::LIST },
    { "/common/interactions/current/end", ossia::val_type::IMPULSE },
    { "/common/interactions/current/stop", ossia::val_type::INT },
    { "/common/interactions/current/pause", ossia::val_type::IMPULSE },
    { "/common/interactions/current/resume", ossia::val_type::IMPULSE },
    { "/common/interactions/current/force", ossia::val_type::LIST },
    //! when we want to send an interaction to ALL users

    { "/common/vote/result", ossia::val_type::INT },
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

    { "/common/transitions/name", ossia::val_type::STRING },

    { "/common/scenario/scene/name", ossia::val_type::STRING }
    //! displays the name of the current scene
};

//---------------------------------------------------------------------------------------------------------
// UTILITY
//---------------------------------------------------------------------------------------------------------

inline parameter_base& quarre::server::make_parameter ( std::string name, ossia::val_type type, bool critical )
{
    auto& node = ossia::net::create_node(get_device(), name);
    ossia::net::set_critical(node, critical);
    return *node.create_parameter(type);
}

inline parameter_base* quarre::server::get_parameter_from_string ( std::string& address )
{
    auto node   = ossia::net::find_node(get_device(), address);
    return      node->get_parameter();
}

inline parameter_base* quarre::server::get_parameter_from_string ( QString& address )
{
    auto node   = ossia::net::find_node(get_device(), address.toStdString());
    return      node->get_parameter();
}

inline parameter_base* quarre::server::get_user_parameter_from_string(
        const quarre::user &usr, std::string address )
{
    std::string res = usr.m_base_address + address;
    return server::get_parameter_from_string(res);
}

#define GET_USER_PARAMETER(p) \
    *m_server.get_user_parameter_from_string(*this, p)

//---------------------------------------------------------------------------------------------------------
// UTILITY_JS
//---------------------------------------------------------------------------------------------------------

#define JS_GET_VECF(t, n)                                   \
    auto arr = engine.newArray(n);                          \
    for ( int i = 0; i < n; ++i )                           \
        arr.setProperty(i, v.get<t>()[i]);                  \
    arguments << arr;

#define VECF_GET_JS(t)                                      \
    t vec;                                                  \
    auto array = result.toVariant().toJsonArray();          \
    for ( int i = 0; i < vec.size(); ++i )                  \
        vec[i] = array[i];                                  \


void quarre::js::append ( QJSValueList& arguments, const ossia::value& v, QJSEngine& engine )
{
    switch (v.getType())
    {
    case ossia::val_type::BOOL:     arguments << v.get<bool>(); break;
    case ossia::val_type::INT:      arguments << v.get<int>(); break;
    case ossia::val_type::FLOAT:    arguments << v.get<float>(); break;
    case ossia::val_type::CHAR:     arguments << v.get<char>(); break;
    case ossia::val_type::STRING:   arguments << QString::fromStdString(v.get<std::string>()); break;

    case ossia::val_type::VEC2F:    { JS_GET_VECF(ossia::vec2f, 2); break; }
    case ossia::val_type::VEC3F:    { JS_GET_VECF(ossia::vec3f, 3); break; }
    case ossia::val_type::VEC4F:    { JS_GET_VECF(ossia::vec4f, 4); break; }
    case ossia::val_type::LIST:     break;
    }
}

ossia::value quarre::js::parse_atom( const QJSValue &jsv )
{
    ossia::value res;

    if ( jsv.isBool() ) res = jsv.toBool();
    else if ( jsv.isNumber() ) res = (float) jsv.toNumber();
    else if ( jsv.isString() ) res = jsv.toString().toStdString();

    else if ( jsv.isArray() )
    {
        std::vector<ossia::value> vec;
        QJSValueIterator it ( jsv );

        while ( it.hasNext() )
            vec.push_back(quarre::js::parse_atom(it.value()));

        res = vec;
    }

    return res;
}

inline void quarre::js::parse_and_push ( const QJSValue& jsv, const Device::DeviceList& devlist ) // we assume value is object
{
    QString address_property    = "address";
    QString value_property      = "value";

    auto target_str = jsv.property(address_property).toString();
    if ( target_str == "undefined" ) return;

    auto state_addr     = State::Address::fromString(target_str).value_or(State::Address{});
    auto& output_p      = *Engine::score_to_ossia::address(state_addr, devlist);

   output_p.push_value  ( quarre::js::parse_atom(jsv.property(value_property)) );
}

//---------------------------------------------------------------------------------------------------------
// USER
//---------------------------------------------------------------------------------------------------------

quarre::user::user  ( uint8_t index, quarre::server& server) : m_server(server)
{
    m_active_interaction       = 0;
    m_incoming_interaction     = 0;
    m_connected                = false;
    m_index                    = index;
    m_status                   = user_status::DISCONNECTED;
    m_base_address             = "/user/";
    m_base_address             += std::to_string(index);

    make_user_parameter_tree();
}

void quarre::user::make_user_parameter_tree()
{
    for ( const auto& parameter : g_user_tree )
        m_server.make_parameter(m_base_address + parameter.first, parameter.second, true);

    for ( const auto& gesture : g_gestures )
    {        
        std::string gesture_addr = m_base_address + "/gestures/" + gesture.first;
        m_server.make_parameter ( gesture_addr + "/available", ossia::val_type::BOOL, true );
        m_server.make_parameter ( gesture_addr + "/poll", ossia::val_type::BOOL, true );
        m_server.make_parameter ( gesture_addr + "/trigger", ossia::val_type::BOOL, true );

        // create gesture's subnodes
        for ( const auto& subgesture : gesture.second )
            m_server.make_parameter(gesture_addr + "/" + subgesture, ossia::val_type::IMPULSE, true);
    }

    for ( const auto& sensor : g_sensors )
    {        
        std::string sensor_addr = m_base_address + "/sensors/" + sensor.first;
        m_server.make_parameter ( sensor_addr + "/available", ossia::val_type::BOOL, true );

        // create sensor data nodes
        for ( const auto& axis : sensor.second )
        {
            std::string subdata_addr = sensor_addr + "/" + axis.first;
            m_server.make_parameter ( subdata_addr + "/poll", ossia::val_type::BOOL, true );
            m_server.make_parameter ( subdata_addr + "/data", axis.second, false );
        }
    }

    for ( const auto& controller : g_controllers )
    {        
        std::string ctrl_addr = m_base_address + "/controllers/" + std::get<0>(controller);

        for ( int i = 0; i < std::get<1>(controller); ++i )
        {
            std::string data_addr = ctrl_addr + "/" + std::to_string(i) + "/data";
            m_server.make_parameter(data_addr, std::get<2>(controller), false);
        }
    }
}

inline void quarre::user::clear_input(QString input)
{
    if ( input == "" ) return;

    if ( !input.contains("controllers") )
        deactivate_input ( input );

    sanitize_input_name ( input );
    auto& p_expr_source = *m_server.get_parameter_from_string(input);
    p_expr_source.callbacks_clear();
}

inline void quarre::user::replace_user_wildcard(QString& target)
{
    target.replace("0", QString::number(m_index));
}

inline void quarre::user::get_input_base_address(QString& target)
{
    if ( target.contains("gestures"))
        target.remove("/trigger");

    else if ( target.contains("sensors"))
        target.remove("/data");

    else return;
}

inline void quarre::user::sanitize_input_name(QString& input_name)
{
    if ( input_name.startsWith("quarre-server:"))
        input_name.replace("quarre-server:/user/0", QString::fromStdString(m_base_address));

    else replace_user_wildcard(input_name);
}

inline parameter_base& quarre::user::get_input_parameter(QString input, QString replacement)
{
    sanitize_input_name         ( input );
    get_input_base_address      ( input );
    input.append                ( replacement );

    return *m_server.get_parameter_from_string(input);
}

parameter_base* quarre::user::get_and_activate_input_parameter ( QString input )
{
    if ( input == "" ) return 0;

    if ( !input.contains("controllers") )
        activate_input ( input );

    sanitize_input_name ( input );
    return m_server.get_parameter_from_string ( input );

}

inline bool quarre::user::supports_input(QString input)
{
    sanitize_input_name ( input );

    parameter_base* parameter = 0;

    if ( input.contains("sensors") )
    {
        QStringList split = input.split("sensors/");
        input = split[0] + "sensors/" + split[1].split("/")[0] + "/available";
        parameter = m_server.get_parameter_from_string(input);

    }

    else parameter = &get_input_parameter(input, "/available");
    return parameter->value().get<bool>();
}

inline void quarre::user::activate_input(QString input)
{
    auto& parameter = get_input_parameter(input, "/poll");
    parameter.push_value(true);
}

inline void quarre::user::deactivate_input(QString input)
{
    auto& parameter = get_input_parameter(input, "/poll");
    parameter.push_value(false);
}

void quarre::user::update_net_address(const std::string &net_address)
{
    m_net_address   = net_address;
    auto& parameter = GET_USER_PARAMETER("/address");

    parameter.push_value(net_address);
}

uint8_t quarre::user::get_active_countdown() const
{
    auto& parameter = GET_USER_PARAMETER("/interactions/current/countdown");
    return parameter.value().get<int>();
}

//---------------------------------------------------------------------------------------------------------
// INTERACTIONS
//---------------------------------------------------------------------------------------------------------

void quarre::user::set_incoming_interaction(quarre::interaction& i)
{
    m_incoming_interaction = &i;

    if ( m_status == user_status::ACTIVE )
         m_status = user_status::INCOMING_ACTIVE;
    else m_status = user_status::INCOMING;

    auto& parameter = GET_USER_PARAMETER("/interactions/next/incoming");
    parameter.push_value(i.to_list());
}

#define GET_JS_RESULT_FROM_EXPRESSION(e)            \
    QJSValueList arguments;                         \
    QJSValue fun = m_js_engine.evaluate(e);         \
    quarre::js::append(arguments, v, m_js_engine);  \
    QJSValue result = fun.call(arguments);


void quarre::user::set_active_interaction(
        quarre::interaction& i, const Device::DeviceList &devlist)
{
    m_status                = user_status::ACTIVE;
    m_incoming_interaction  = 0;
    m_active_interaction    = &i;

    // push interaction to remote
    auto& p_active =  GET_USER_PARAMETER ( "/interactions/next/begin" );
    p_active.push_value(i.to_list());

    // set end expression to trigger timesync

    auto end_expression_source    = i.end_expression_source();
    if ( !end_expression_source.isEmpty() )
    {
        auto& end_expression_js  = i.end_expression_js();
        auto& tsync = i.get_ossia_tsync();

        auto& end_expression_p = *get_and_activate_input_parameter(end_expression_source);

        end_expression_p.add_callback([&](const ossia::value& v)
        {
            GET_JS_RESULT_FROM_EXPRESSION   ( end_expression_source );
            if ( result.toBool() ) tsync.trigger_request = true;
        });
    }

    // MAPPINGS

    for ( const auto& mapping : i.mappings())
    {
        auto& mapping_expression    = mapping->expression_js();
        auto& mapping_input_p       = *get_and_activate_input_parameter(mapping->source());
        auto mapping_destination    = mapping->destination();

        if ( mapping_destination.isEmpty() )
        {
            mapping_input_p.add_callback([&](const ossia::value& v)
            {
                GET_JS_RESULT_FROM_EXPRESSION ( mapping_expression );
                QJSValueIterator it ( result );

                while ( it.hasNext() )
                {
                    it.next();
                    quarre::js::parse_and_push(it.value(), devlist);
                }
            });
        }

        else
            // an unique parameter destination has been selected in the form
            // this will optimize evaluation, as we don't have to parse destination
        {
            auto state_addr = State::Address::fromString(mapping_destination)
                    .value_or(State::Address{});

            auto& output_p = *Engine::score_to_ossia::address(state_addr, devlist);

            mapping_input_p.add_callback([&](const ossia::value& v)
            {
                GET_JS_RESULT_FROM_EXPRESSION ( mapping_expression );
                auto ov = quarre::js::parse_atom ( result );
                output_p.push_value(ov);
            });
        }
    }
}

void quarre::user::end_interaction(quarre::interaction &i)
{
    m_active_interaction = 0;

    if ( m_status == user_status::INCOMING_ACTIVE )
         m_status = user_status::INCOMING;
    else m_status = user_status::IDLE;

    // send interaction end + reset begin & incoming for qml
    auto& p_end     = GET_USER_PARAMETER    ( "/interactions/current/end" );
    auto& p_inc     = GET_USER_PARAMETER    ( "/interactions/next/incoming" );
    auto& p_begin   = GET_USER_PARAMETER    ( "/interactions/next/begin" );

    p_end.push_value    ( ossia::impulse{} );
    p_inc.push_value    ( std::vector<ossia::value> { ossia::value {} } );
    p_begin.push_value  ( std::vector<ossia::value> { ossia::value {} } );

    // special ending for vote
    if ( i.module() == "Vote" )
    {
        m_server.parse_vote_result();
        return;
    }

    // clear mapping & end-expression callbacks
    clear_input(i.end_expression_source());

    for ( const auto& mapping : i.mappings())
        clear_input(mapping->source());
}

void quarre::user::pause_current_interaction(quarre::interaction &i)
{
    auto& p_pause = GET_USER_PARAMETER("/interactions/current/pause");
    p_pause.push_value ( ossia::impulse{} );
}

void quarre::user::resume_current_interaction(quarre::interaction &i)
{
    auto& p_resume = GET_USER_PARAMETER("/interactions/current/resume");
    p_resume.push_value ( ossia::impulse{} );
}

//---------------------------------------------------------------------------------------------------------
// INTERACTION_DISPATCH
//---------------------------------------------------------------------------------------------------------

bool quarre::dispatcher::dispatch_incoming_interaction(quarre::interaction &i)
{
    // candidate algorithm
    // eliminate non-connected clients
    // eliminate clients that cannot support the requested inputs
    // eliminate clients that already have an incoming interaction
    auto& srv = quarre::server::instance();

    if ( i.dispatch_all() )
    {
        for ( const auto& user : srv.m_users)
            if ( user->m_connected )
                user->set_incoming_interaction(i);

        return true;
    }

    std::vector<dispatcher::candidate> candidates;

    for ( const auto& user : srv.m_users )
    {
        dispatcher::candidate candidate;

        candidate.target    = user;
        candidate.priority  = 0;

        switch ( user->m_status )
        {
        case user_status::DISCONNECTED:        goto next;
        case user_status::INCOMING:            goto next;
        case user_status::INCOMING_ACTIVE:     goto next;

        case user_status::IDLE:
            goto check_inputs; // priority stays at 0

        case user_status::ACTIVE:
        {
            auto acd = user->get_active_countdown();
            if ( i.countdown() < acd+5) goto next;

            candidate.priority = 1;
            goto check_inputs;
        }
        }

        check_inputs:
        for ( const auto& input : i.inputs() )
        {
            if ( input == "" || input.contains("controllers") ) continue;
            bool support = user->supports_input(input);
            if ( !support )
                goto next;
        }

        select:
        candidate.priority += user->m_interaction_count;
        candidates.push_back(candidate);

        next:
        continue;
    }

    // the candidate with the lowest priority will be selected
    // if there is no candidate, interaction will not be dispatched
    if ( candidates.size() == 0 ) return false;

    dispatcher::candidate* winner = 0;

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
    {
        winner->target->set_incoming_interaction(i);
        return true;
    }

    else return false;
}

void quarre::dispatcher::dispatch_active_interaction(
        quarre::interaction& i, const Device::DeviceList& devlist )
{
    for ( const auto& user : quarre::server::instance().m_users )
    {
        if ( user->m_incoming_interaction == &i )
        {
            user->set_active_interaction(i, devlist);
            if ( !i.dispatch_all() ) return;
        }
    }
}

void quarre::dispatcher::dispatch_ending_interaction(quarre::interaction &i)
{
    auto& srv = quarre::server::instance();

    for ( const auto& user : srv.m_users )
    {
        if ( user->m_active_interaction == &i )
        {
            user->end_interaction(i);
            if ( !i.dispatch_all() ) return;
        }
    }
}

void quarre::dispatcher::dispatch_paused_interaction(quarre::interaction &i)
{
    for ( const auto& user : quarre::server::instance().m_users )
    {
        if ( user->m_active_interaction == &i )
        {
             user->pause_current_interaction(i);
             if ( !i.dispatch_all() ) return;
        }
    }
}

void quarre::dispatcher::dispatch_resumed_interaction(interaction &i)
{
    for ( const auto& user : quarre::server::instance().m_users )
    {
        if ( user->m_active_interaction == &i )
        {
            user->resume_current_interaction(i);
            if ( !i.dispatch_all() ) return;
        }
    }
}

//---------------------------------------------------------------------------------------------------------
// DEVICE
//---------------------------------------------------------------------------------------------------------

quarre::server& quarre::server::instance()
{
    return *m_singleton;
}

generic_device& quarre::server::get_device()
{
    return *dynamic_cast<generic_device*>(m_dev.get());
}

quarre::server::server(const Device::DeviceSettings &settings) :
    OwningOSSIADevice ( settings )
{
    m_capas.canRefreshTree      = true;
    m_capas.canRenameNode       = false;
    m_capas.canSetProperties    = false;
    m_capas.canRemoveNode       = false;

    m_singleton = this;
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

    // build users
    // note that user 0 is a wildcard:
    // it will select the best candidate to receive the interaction
    auto& gendev = *dynamic_cast<generic_device*>(m_dev.get());

    m_user_zero = new quarre::user(0, *this);

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
            std::string cids = "/connections/ids";
            auto& ids_p = *get_parameter_from_string(cids);
            auto ids_v = ids_p.value().get<std::vector<ossia::value>>();

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
            auto& ids_p = *get_parameter_from_string(cids);
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
        auto& node = ossia::net::create_node(get_device(), parameter.first);
        node.create_parameter(parameter.second);
        ossia::net::set_critical(node, true);
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
            auto& choice = *get_user_parameter_from_string(*user, "/vote/choice");
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
    auto& res_p = *get_parameter_from_string(res_str);
    res_p.push_value(total);
}

