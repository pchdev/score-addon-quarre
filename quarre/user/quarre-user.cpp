#include "quarre-user.hpp"
#include <quarre/js/quarre-js.hpp>
#include <QJSValueIterator>

using namespace score::addons::quarre;

static const std::vector<pdata_t> g_user_tree =
{
    // NETWORK ---------------------------------------------------------------------

    { "/address", ossia::val_type::STRING, true },

    // INTERACTIONS ----------------------------------------------------------------

    { "/interactions/next/incoming", ossia::val_type::LIST, true },
    { "/interactions/next/begin", ossia::val_type::LIST, true },
    { "/interactions/next/cancel", ossia::val_type::IMPULSE, true },
    { "/interactions/next/countdown", ossia::val_type::INT, true },
    { "/interactions/current/countdown", ossia::val_type::INT, true },
    { "/interactions/current/end", ossia::val_type::IMPULSE, true },
    { "/interactions/current/force", ossia::val_type::STRING, true },

    // GESTURES --------------------------------------------------------------------

    { "/gestures/whip/available", ossia::val_type::BOOL, true },
    { "/gestures/cover/available", ossia::val_type::BOOL, true },
    { "/gestures/shake/available", ossia::val_type::BOOL, true },
    { "/gestures/turnover/available", ossia::val_type::BOOL, true },

    // SENSORS ---------------------------------------------------------------------

    { "/sensors/rotation/available", ossia::val_type::BOOL, true },
    { "/sensors/accelerometers/available", ossia::val_type::BOOL, true },
    { "/sensors/proximity/available", ossia::val_type::BOOL, true },

    // OTHERS ---------------------------------------------------------------------
    { "/modules/vote/choice", ossia::val_type::INT, true },
    { "/modules/xytouch/trigger", ossia::val_type::VEC2F, true },
    { "/modules/birds/trigger", ossia::val_type::VEC3F, true },
    { "/modules/trajectories/trigger", ossia::val_type::BOOL, true },
    { "/modules/trajectories/position", ossia::val_type::VEC2F, false},
    { "/modules/gestures/hammer/trigger", ossia::val_type::BOOL, true },
    { "/modules/gestures/palm/trigger", ossia::val_type::BOOL, true },
    { "/modules/gestures/shake/trigger", ossia::val_type::BOOL, true },
    { "/modules/gestures/blow/trigger", ossia::val_type::BOOL, true },
    { "/modules/sensors/rotation/x/angle", ossia::val_type::FLOAT, false },
    { "/modules/sensors/rotation/y/angle", ossia::val_type::FLOAT, false },
    { "/modules/sensors/rotation/z/angle", ossia::val_type::FLOAT, false },
    { "/modules/sensors/rotation/xyz/data", ossia::val_type::VEC3F, false },
    { "/modules/sensors/accelerometers/x/data", ossia::val_type::FLOAT, false },
    { "/modules/sensors/accelerometers/y/data", ossia::val_type::FLOAT, false },
    { "/modules/sensors/accelerometers/z/data", ossia::val_type::FLOAT, false},
    { "/modules/sensors/accelerometers/xyz/data", ossia::val_type::VEC3F, false },
    { "/modules/sensors/proximity/close", ossia::val_type::BOOL, true },

    { "/modules/vare/resonator/brightness", ossia::val_type::FLOAT, false },
    { "/modules/vare/resonator/inpos", ossia::val_type::FLOAT, false },
    { "/modules/vare/resonator/pitch", ossia::val_type::FLOAT, false },
    { "/modules/vare/body/tone", ossia::val_type::FLOAT, false },
    { "/modules/vare/body/pitch", ossia::val_type::FLOAT, false },
    { "/modules/vare/body/xy", ossia::val_type::VEC2F, false },
    { "/modules/vare/granular/pitch_env", ossia::val_type::FLOAT, false },
    { "/modules/vare/granular/overlap", ossia::val_type::FLOAT, false },
    { "/modules/vare/granular/pitch", ossia::val_type::FLOAT, false },
    { "/modules/vare/granular/sample", ossia::val_type::INT, true },
    { "/modules/vare/percussions/handdrum", ossia::val_type::BOOL, true },
    { "/modules/vare/percussions/shake", ossia::val_type::BOOL, true },
    { "/modules/markhor/resonator/brightness", ossia::val_type::FLOAT, false },
    { "/modules/markhor/resonator/inpos", ossia::val_type::FLOAT, false },
    { "/modules/markhor/resonator/pitch_p", ossia::val_type::FLOAT, false },
    { "/modules/markhor/resonator/sustain", ossia::val_type::FLOAT, false },
    { "/modules/markhor/body/tone", ossia::val_type::FLOAT, false },
    { "/modules/markhor/body/pitch", ossia::val_type::FLOAT, false },
    { "/modules/markhor/body/xy", ossia::val_type::VEC2F, false },
    { "/modules/markhor/granular/pitch_env", ossia::val_type::FLOAT, false },
    { "/modules/markhor/granular/pitch", ossia::val_type::FLOAT, false },
    { "/modules/markhor/granular/overlap", ossia::val_type::FLOAT, false },
    { "/modules/markhor/granular/sample", ossia::val_type::FLOAT, false },
    { "/modules/markhor/pads/index", ossia::val_type::INT, true },
    { "/modules/ammon/reverb/model", ossia::val_type::INT, true },
    { "/modules/ammon/reverb/level", ossia::val_type::FLOAT, false }
};

user::user(uint8_t index, quarre::server& server) : m_server(server), m_index(index)
{
    m_incoming_interaction  = 0;
    m_active_interaction    = 0;

    m_status    = user_status::DISCONNECTED;
    m_connected = false;

    m_base_address = "/user/";
    if ( index == 0 ) m_base_address += "wildcard";
    else m_base_address += std::to_string(index);

    for ( const auto& parameter : g_user_tree )
        server.make_user_parameter(index, parameter.address, parameter.type, parameter.critical);
}

void user::sanitize_input_name(QString &input_name)
{
    input_name.replace ( "wildcard", QString::number(m_index));
    input_name.replace ( "quarre-server:", "" );
}

inline parameter_base& user::get_input_parameter(QString path)
{
    sanitize_input_name( path );
    return *m_server.get_common_parameter(path);
}

inline parameter_base& user::get_parameter(std::string path)
{
    return *m_server.get_user_parameter(*this, path);
}

inline void user::clear_input(QString path)
{
    if ( path == "" ) return;
    get_input_parameter(path).callbacks_clear();
}

bool user::supports_interaction(quarre::interaction& i)
{
    for ( const auto& module : g_modules )
        if ( i.module() == module.name )
        {
            if ( module.address == "" ) return true;
            else return get_parameter(module.address + "/available").value().get<bool>();
        }

    return false;
}

void user::set_incoming_interaction(quarre::interaction& i)
{
    m_incoming_interaction = &i;

    if ( m_status == user_status::ACTIVE )
         m_status = user_status::INCOMING_ACTIVE;
    else m_status = user_status::INCOMING;

    auto& parameter = get_parameter("/interactions/next/incoming");
    parameter.push_value(i.to_list());
}

#define GET_JS_RESULT_FROM_EXPRESSION(e)            \
    QJSValueList arguments;                         \
    QJSValue fun = m_js_engine.evaluate(e);         \
    quarre::js::append(arguments, v, m_js_engine);  \
    QJSValue result = fun.call(arguments);

void user::set_active_interaction(
        quarre::interaction& i, const Device::DeviceList &devlist)
{
    m_status                = user_status::ACTIVE;
    m_incoming_interaction  = 0;
    m_active_interaction    = &i;

    // push interaction to remote
    auto& p_active =  get_parameter("/interactions/next/begin" );
    p_active.push_value(i.to_list());

    // set end expression to trigger timesync

    auto end_expression_source = i.end_expression_source();
    if ( !end_expression_source.isEmpty() )
    {
        auto& end_expression_js  = i.end_expression_js();
        auto& tsync = i.get_ossia_tsync();
        auto& end_expression_p = get_input_parameter(end_expression_source);

        end_expression_p.add_callback([&](const ossia::value& v)
        {
            GET_JS_RESULT_FROM_EXPRESSION   ( end_expression_js );
            if ( result.toBool() ) tsync.trigger_request = true;
        });
    }

    // MAPPINGS

    for ( const auto& mapping : i.mappings())
    {
        auto& mapping_expression    = mapping->expression_js();
        auto& mapping_input_p       = get_input_parameter(mapping->source());
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
                auto ov = quarre::js::parse_atom ( result, output_p.get_value_type() );
                output_p.push_value(ov);
            });
        }
    }
}

void user::end_interaction(quarre::interaction &i)
{
    m_active_interaction = 0;   

    if ( m_status == user_status::INCOMING_ACTIVE )
         m_status = user_status::INCOMING;
    else m_status = user_status::IDLE;

    // send interaction end + reset begin & incoming for qml
    auto& p_end     = get_parameter("/interactions/current/end");
    auto& p_inc     = get_parameter("/interactions/next/incoming");
    auto& p_begin   = get_parameter("/interactions/next/begin");

    p_end.push_value    ( ossia::impulse{} );
    p_inc.push_value    ( std::vector<ossia::value> { ossia::value {""} } );
    p_begin.push_value  ( std::vector<ossia::value> { ossia::value {""} } );

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

void user::pause_current_interaction(quarre::interaction& i)
{
    auto& p_pause = get_parameter("/interactions/current/pause");
    p_pause.push_value ( ossia::impulse{} );
}

void user::resume_current_interaction(quarre::interaction& i)
{
    auto& p_resume = get_parameter("/interactions/current/resume");
    p_resume.push_value ( ossia::impulse{} );
}

void user::cancel_incoming_interaction(quarre::interaction& i)
{
    m_incoming_interaction = 0;
    if ( m_status  == user_status::INCOMING ) m_status = user_status::IDLE;
    else if ( m_status == user_status::INCOMING_ACTIVE ) m_status = user_status::ACTIVE;

    auto& p_cancel = get_parameter("/interactions/next/cancel");
    p_cancel.push_value(ossia::impulse{});
}

void user::update_net_address(const std::string &net_address)
{
    m_net_address = net_address;
    get_parameter("/address").push_value(net_address);
}

uint8_t user::get_active_countdown()
{
    return get_parameter("/interactions/current/countdown").value().get<int>();
}


