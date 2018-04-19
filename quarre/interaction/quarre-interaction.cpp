#include "quarre-interaction.hpp"

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>
#include <QFormLayout>
#include <score/tools/IdentifierGeneration.hpp>


using namespace score::addons;

quarre::interaction::interaction(
        const Id<quarre::interaction> &id,
        QObject *parent) :

    IdentifiedObject    ( id, "quarrè-interaction", parent ),
    m_module            ( "No selected module"),
    m_title             ( "Untitled"),
    m_description       ( "No description"),
    m_length            ( 0 ),
    m_countdown         ( 0 )

{

}

void quarre::interaction::on_mapping_added()
{
    auto mp = new quarre::mapping(getStrongId(m_mappings), this);
    m_mappings.push_back(mp);
    emit mapping_added(*mp);
}

void quarre::interaction::on_mapping_removed(quarre::mapping *target)
{
    m_mappings.erase (std::remove(m_mappings.begin(), m_mappings.end(), target), m_mappings.end());
    delete target;
    qDebug() << m_mappings.size();
}

const std::vector<quarre::mapping*>& quarre::interaction::mappings()
{
    return m_mappings;
}

QStringList quarre::interaction::inputs() const
{
    QStringList res;

    for ( const auto& mapping : m_mappings )
    {
        QString source = mapping->source();
        QString res_src = source.remove("/user/0");
        res << res_src;
    }

    return res;
}

void quarre::interaction::onModuleChanged(QString module)
{
    m_module = module;
}

void quarre::interaction::onTitleChanged(QString title)
{
    m_title = title;
}

void quarre::interaction::onDescriptionChanged(QString description)
{
    m_description = description;
}

void quarre::interaction::onLengthChanged(int length)
{
    m_length = length;
}

void quarre::interaction::onCountdownChanged(int countdown)
{
    m_countdown = countdown;
}

const QString quarre::interaction::module() const
{
    return m_module;
}

const QString quarre::interaction::title() const
{
    return m_title;
}

const QString quarre::interaction::description() const
{
    return m_description;
}

int quarre::interaction::length() const
{
    return m_length;
}

int quarre::interaction::countdown() const
{
    return m_countdown;
}

template <> void DataStreamReader::read(
        const quarre::interaction& e )
{
    m_stream << e.module();
    m_stream << e.title();
    m_stream << e.description();
    m_stream << e.length();
    m_stream << e.countdown();

    m_stream << (qint64) e.m_mappings.size();
    for ( const auto& mapping : e.m_mappings )
        readFrom( *mapping );

    insertDelimiter();
}

template <> void DataStreamWriter::write(
        quarre::interaction& e )
{
    int msz;

    m_stream >> e.m_module >> e.m_title >> e.m_description >> e.m_length >> e.m_countdown;
    m_stream >> msz;

    for (; msz-- >0;)
    {
        auto mp = new quarre::mapping(*this, &e);
        writeTo(*mp);
        e.m_mappings.push_back(mp);
    }

    checkDelimiter();
}

template <> void JSONObjectReader::read(
        const quarre::interaction& e )
{
    obj [ "Module" ]        = e.module();
    obj [ "Title"  ]        = e.title();
    obj [ "Description" ]   = e.description();
    obj [ "Length" ]        = e.length();
    obj [ "Countdown" ]     = e.countdown();
    obj [ "Mappings" ]      = toJsonArray(e.m_mappings);
}

template <> void JSONObjectWriter::write(
        quarre::interaction& e )
{
    e.m_module =         obj [ "Module" ].toString();
    e.m_title =          obj [ "Title" ].toString();
    e.m_description =    obj [ "Description"].toString();
    e.m_length =         obj [ "Length"].toInt();
    e.m_countdown =      obj [ "Countdown" ].toInt();

    for ( const auto& json_vref : obj ["Mappings"].toArray())
    {
        JSONObject::Deserializer dsrz (json_vref.toObject());
        auto mp = new quarre::mapping(dsrz, &e);

        e.m_mappings.push_back(mp);
    }
}

