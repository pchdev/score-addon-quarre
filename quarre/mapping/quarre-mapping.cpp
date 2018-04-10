#include <quarre/mapping/quarre-mapping.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>
#include <QFormLayout>

using namespace score::addons;

static const QStringList sensors_list =
{
    "Accelerometer_x", "Accelerometer_y", "Accelerometer_z",
    "Rotation_x", "Rotation_y", "Rotation_z",
    "Proximiy"
};

static const QStringList gestures_list =
{
    "Whip", "Cover", "Turnover", "Freefall", "Pickup",
    "Shake", "Shake/Left", "Shake/Right", "Shake/Up", "Shake/Down",
    "Twist", "Twist/Left", "Twist/Right"
};

quarre::Mapping::Mapping(
        const Id<quarre::Mapping> &id,
        QObject *parent) :

    IdentifiedObject    ( id, "quarrè-mapping", parent ),
    m_source            ( new QLineEdit ),
    m_destination       ( new QLineEdit ),
    m_minus_button      ( new QPushButton("-")),
    m_layout            ( new QVBoxLayout ),
    m_expression        ( new QTextEdit )
{
    m_expression->setPlainText("( function(v, dest) { dest[\"value\"] = v; } )");

    auto form = new QFormLayout;
    form->addRow ( tr ( "delete" ), m_minus_button );
    form->addRow ( tr ( "source" ), m_source );
    form->addRow ( tr ( "destination" ), m_destination );

    m_layout->addLayout(form);
    m_layout->addWidget(m_expression);
}

QLayout* quarre::Mapping::layout() const
{
    return m_layout;
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


