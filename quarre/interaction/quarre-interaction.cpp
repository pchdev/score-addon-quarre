#include "quarre-interaction.hpp"

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

using namespace score::addons;

quarre::Interaction::Interaction(
        const Id<quarre::Interaction> &id,
        QObject *parent) :

    IdentifiedObject    ( id, "Interaction", parent ),
    module              ( "None" ),
    title               ( "Untitled" ),
    description         ( "No description" ),
    length              ( 0 ),
    countdown           ( 0 )
{
}

quarre::Interaction::~Interaction() {}

template <> void DataStreamReader::read(
        const quarre::Interaction& e )
{
    m_stream << e.module;
    m_stream << e.title;
    m_stream << e.description;
    m_stream << e.length;
    m_stream << e.countdown;

    m_stream << e.sensors.size();
    for ( const auto& sensor : e.sensors )
        m_stream << sensor;

    m_stream << e.gestures.size();
    for ( const auto& gesture: e.gestures )
        m_stream << gesture;

    insertDelimiter();
}

template <> void DataStreamWriter::write(
        quarre::Interaction& e )
{
    uint8_t sensors_count, gestures_count;

    m_stream >> e.module;
    m_stream >> e.title;
    m_stream >> e.description;
    m_stream >> e.length;
    m_stream >> e.countdown;

    m_stream >> sensors_count;

    for (; sensors_count-- > 0; )
        m_stream >> e.sensors;

    m_stream >> gestures_count;

    for (; gestures_count-- > 0; )
        m_stream >> e.gestures;

    checkDelimiter();
}


template <> void JSONObjectReader::read(
        const quarre::Interaction& e )
{
    obj [ "Module" ]        = e.module;
    obj [ "Title"  ]        = e.title;
    obj [ "Description" ]   = e.description;
    obj [ "Length" ]        = e.length;
    obj [ "Countdown" ]     = e.countdown;
    obj [ "Sensors" ]       = e.sensors;
    obj [ "Gestures" ]      = e.gestures;

}

template <> void JSONObjectWriter::write(
        quarre::Interaction& e )
{
    e.module        = obj [ "Module" ].toString();
    e.title         = obj [ "Title" ].toString();
    e.description   = obj [ "Description" ].toString();
    e.length        = obj [ "Length" ].toInt();
    e.countdown     = obj [ "Countdown" ].toInt();
    e.sensors       = obj [ "Sensors" ].toArray();
    e.gestures      = obj [ "Gestures" ].toArray();
}

