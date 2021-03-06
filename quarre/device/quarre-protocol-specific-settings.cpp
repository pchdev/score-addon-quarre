#include "quarre-protocol-specific-settings.hpp"

#include <QJsonObject>
#include <QJsonValue>
#include <QString>

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

using namespace score::addons;

template <> void DataStreamReader::read(
        const quarre::SpecificSettings& n )
{
    m_stream << n.osc_port;
    m_stream << n.ws_port;
    m_stream << n.max_users;

    insertDelimiter();
}

template <> void DataStreamWriter::write(
         quarre::SpecificSettings& n )
{
    m_stream >> n.osc_port;
    m_stream >> n.ws_port;
    m_stream >> n.max_users;

    checkDelimiter();
}

template <> void JSONObjectReader::read(
        const quarre::SpecificSettings& n )
{
    obj [ "OSCPort" ]   = n.osc_port;
    obj [ "WSPort" ]    = n.ws_port;
    obj [ "MaxUsers" ]  = n.max_users;
}

template <> void JSONObjectWriter::write(
         quarre::SpecificSettings& n )
{
    n.osc_port  = obj  [ "OSCPort"  ].toInt();
    n.ws_port   = obj  [ "WSPort"   ].toInt();
    n.max_users = obj  [ "MaxUsers" ].toInt();
}
