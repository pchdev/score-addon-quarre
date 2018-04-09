#include <quarre/mapping/quarre-mapping.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

using namespace score::addons;

quarre::Mapping::Mapping(
        const Id<quarre::Mapping> &id,
        QObject *parent) :

    IdentifiedObject    ( id, "quarrè-mapping", parent ),
    m_source            ( new QLineEdit ),
    m_destination       ( new QLineEdit )
{
    m_expression = "( function(v, dest) { dest[\"value\"] = v; } )";
}

const QString& quarre::Mapping::source()
{
    return m_source->text();
}

const QString& quarre::Mapping::destination()
{
    return m_destination->text();
}

const QString& quarre::Mapping::expression()
{
    return m_expression->toPlainText();
}

void quarre::Mapping::set_source(const QString &source)
{
    m_source->setText(source);
}

void quarre::Mapping::set_destination(const QString &destination)
{
    m_destination->setText(destination);
}

void quarre::Mapping::set_expression(const QString &expression)
{
    m_expression->setPlainText(expression);
}

template <> void DataStreamReader::read(
        const quarre::Mapping& e )
{
    m_stream << e.source();
    m_stream << e.destination();
    m_stream << e.expression();

    insertDelimiter();
}

template <> void DataStreamWriter::write(
        quarre::Mapping& e )
{
    QString src, dest, exp;
    m_stream >> src >> dest >> exp;

    e.set_source        ( src );
    e.set_destination   ( dest );
    e.set_expression    ( exp );

    checkDelimiter();
}

template <> void JSONObjectReader::read(
        const quarre::Mapping& e )
{
    obj [ "Source" ]            = e.source();
    obj [ "Destination"  ]      = e.destination();
    obj [ "Expression" ]        = e.expression();
}

template <> void JSONObjectWriter::write(
        quarre::Mapping& e )
{
    e.m_source->setText             ( obj [ "Source" ].toString() );
    e.m_destination->setText        ( obj [ "Destination" ].toString() );
    e.m_expression->setPlainText    ( obj [ "Expression" ].toString() );
}


