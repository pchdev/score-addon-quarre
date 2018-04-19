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

    IdentifiedObject    ( id, "quarrè-mapping", parent ),
    m_minus_button      ( new QPushButton("-")),
    m_layout            ( new QVBoxLayout ),
    m_expression        ( new QTextEdit )
{

    auto inspector      = dynamic_cast<quarre::interaction*>(parent)->m_inspector;
    const auto& doc     = inspector->m_dctx;
    auto& explorer      = doc.plugin<Explorer::DeviceDocumentPlugin>().explorer();
    m_source            = new Explorer::AddressAccessorEditWidget(explorer, inspector);
    m_destination       = new Explorer::AddressAccessorEditWidget(explorer, inspector);

    m_expression->setPlainText("( function(v, dest) { dest[\"value\"] = v; } )");

    auto form = new QFormLayout;
    form->addRow ( tr ( "delete" ), m_minus_button );
    form->addRow ( tr ( "source" ), m_source );
    form->addRow ( tr ( "destination" ), m_destination );

    m_layout->addLayout(form);
    m_layout->addWidget(m_expression);
}

QVBoxLayout* quarre::mapping::layout() const
{
    return m_layout;
}

const QString quarre::mapping::source() const
{
    return m_source->addressString();
}

const QString quarre::mapping::destination() const
{
    return m_destination->addressString();
}

const QString quarre::mapping::expression() const
{
    return m_expression->toPlainText();
}

void quarre::mapping::set_source(const QString &source)
{
    State::AddressAccessor lame;
    m_source->setAddress(State::AddressAccessor::fromString(source).value_or(lame));
}

void quarre::mapping::set_destination(const QString &destination)
{
    State::AddressAccessor lame;
    m_destination->setAddress(State::AddressAccessor::fromString(destination).value_or(lame));
}

void quarre::mapping::set_expression(const QString &expression)
{
    m_expression->setPlainText(expression);
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

    e.set_source        ( src );
    e.set_destination   ( dest );
    e.set_expression    ( exp );

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
    e.set_source        ( obj [ "Source" ].toString() );
    e.set_destination   ( obj [ "Destination" ].toString() );
    e.set_expression    ( obj [ "Expression" ].toString() );
}


