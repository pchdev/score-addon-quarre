#include <quarre/mapping/quarre-mapping.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>
#include <QFormLayout>
#include <quarre/process/inspector/quarre-process-inspector.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>

using namespace score::addons;

quarre::mapping::mapping(
        const Id<quarre::mapping> &id,
        QObject *parent) :

    IdentifiedObject    ( id, "quarrè-mapping", parent )
{
    m_expression = "return v;";
}

const QString quarre::mapping::source() const
{
    return m_source;
}

const QString quarre::mapping::destination() const
{
    return m_destination;
}

const QString quarre::mapping::expression() const
{
    return m_expression;
}

const QString& quarre::mapping::expression_js() const
{
    return m_expression_js;
}

void quarre::mapping::on_source_changed(const QString& source)
{
    m_source = source;
}

void quarre::mapping::on_destination_changed(const QString& destination)
{
    m_destination = destination;
}

void quarre::mapping::on_expression_changed(const QString& expression)
{
    m_expression = expression;
    QString expr_js = m_expression;
    expr_js.prepend("(function(v){").append("})");
    m_expression_js = expr_js;
}

template <> void DataStreamReader::read(
        const quarre::mapping& e )
{
    m_stream << e.source();
    m_stream << e.destination();
    m_stream << e.expression();

    insertDelimiter();
}

template <> void DataStreamWriter::write(
        quarre::mapping& e )
{
    QString src, dest, exp;
    m_stream >> src >> dest >> exp;

    e.on_source_changed        ( src );
    e.on_destination_changed   ( dest );
    e.on_expression_changed    ( exp );

    checkDelimiter();
}

template <> void JSONObjectReader::read(
        const quarre::mapping& e )
{
    obj [ "Source" ]            = e.source();
    obj [ "Destination"  ]      = e.destination();
    obj [ "Expression" ]        = e.expression();
}

template <> void JSONObjectWriter::write(
        quarre::mapping& e )
{
    e.on_source_changed         ( obj [ "Source" ].toString() );
    e.on_destination_changed    ( obj [ "Destination" ].toString() );
    e.on_expression_changed     ( obj [ "Expression" ].toString() );
}


