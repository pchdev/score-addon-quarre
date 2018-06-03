#include "quarre-js.hpp"
#include <QJSValueIterator>

using namespace score::addons;

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

    case ossia::val_type::LIST:
    {
        auto list = v.get<std::vector<ossia::value>>();
        QJSValueList jsvlist;
        int i = 0;

        for ( const auto& value : list )
            quarre::js::append(jsvlist, value, engine);

        arguments << jsvlist;
        break;
    }
    }
}

ossia::value quarre::js::parse_atom( const QJSValue &jsv, ossia::val_type vt )
{
    ossia::value res;

    if      ( jsv.isBool() ) res = jsv.toBool();
    else if ( jsv.isNumber() )
    {
        if       ( vt == ossia::val_type::INT )     res = (int) jsv.toNumber();
        else if  ( vt == ossia::val_type::FLOAT )   res = (float) jsv.toNumber();
    }

    else if ( jsv.isString() ) res = jsv.toString().toStdString();
    else if ( jsv.isArray() )
    {
        std::vector<ossia::value> vec;
        QJSValueIterator it ( jsv );

        while ( it.hasNext() )
            vec.push_back ( quarre::js::parse_atom(it.value(), vt) );

        res = vec;
    }

    return res;
}

void quarre::js::parse_and_push ( const QJSValue& jsv, const Device::DeviceList& devlist ) // we assume value is object
{
    QString address_property    = "address";
    QString value_property      = "value";

    auto target_str = jsv.property(address_property).toString();
    if ( target_str == "undefined" ) return;

    auto state_addr   = State::Address::fromString(target_str).value_or(State::Address{});
    auto output_p     = Engine::score_to_ossia::address(state_addr, devlist);

    if ( output_p )  output_p->push_value(quarre::js::parse_atom ( jsv.property(value_property),
                                          output_p->get_value_type()) );
}
