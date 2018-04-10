#include "quarre-interaction.hpp"

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>
#include <QFormLayout>

using namespace score::addons;

quarre::Interaction::Interaction(
        const Id<quarre::Interaction> &id,
        QObject *parent) :

    IdentifiedObject    ( id, "quarrè-interaction", parent ),
    m_layout            ( new QVBoxLayout ),
    m_module            ( new QLineEdit ("Unknown")),
    m_title             ( new QLineEdit ("Untitled")),
    m_description       ( new QLineEdit ("No description")),
    m_length            ( new QSpinBox ),
    m_countdown         ( new QSpinBox )

{
    auto form = new QFormLayout;
    form->addRow ( tr ("Title"), m_title );
    form->addRow ( tr ("Description"), m_description );
    form->addRow ( tr ("Module"), m_module );
    form->addRow ( tr ("Length"), m_length );
    form->addRow ( tr ("Countdown"), m_countdown);

    auto plusb = new QPushButton("+");

    m_layout->addLayout(form);
    m_layout->addWidget(plusb);
}

QVBoxLayout* quarre::Interaction::layout()
{
    return m_layout;
}

QVector<quarre::Mapping> quarre::Interaction::mappings()
{
    return m_mappings;
}

const QString& quarre::Interaction::module()
{
    return m_module->text();
}

const QString& quarre::Interaction::title()
{
    return m_title->text();
}

const QString& quarre::Interaction::description()
{
    return m_description->text();
}

const int quarre::Interaction::length()
{
    return m_length->value();
}

const int quarre::Interaction::countdown()
{
    return m_countdown->value();
}

void quarre::Interaction::set_module(const QString &module)
{
    m_module->setText(module);
}

void quarre::Interaction::set_title(const QString &title)
{
    m_title->setText(title);
}

void quarre::Interaction::set_description(const QString &description)
{
    m_description->setText(description);
}

void quarre::Interaction::set_length(const int length)
{
    m_length->setValue(length);
}

void quarre::Interaction::set_countdown(const int countdown)
{
    m_countdown->setValue(countdown);
}

void quarre::Interaction::onPlusMappingButtonPressed()
{
    m_mappings.add(new quarre::Mapping);
}

void quarre::Interaction::set_host(std::shared_ptr<quarre::user> host)
{
    m_host = host;
}

std::shared_ptr<quarre::user> quarre::Interaction::host() const
{
    return m_host;
}

void quarre::Interaction::onMinusMappingButtonPressed()
{
    auto sender = qobject_cast<quarre::Mapping*>(QObject::sender());
    m_mappings.erase(sender);
}

template <> void DataStreamReader::read(
        const quarre::Interaction& e )
{
    m_stream << e.module();
    m_stream << e.title();
    m_stream << e.description();
    m_stream << e.length();
    m_stream << e.countdown();

    m_stream << e.mappings().size();
    for ( const auto& mapping : e.mappings() )
        readFrom( mapping );

    insertDelimiter();
}

template <> void DataStreamWriter::write(
        quarre::Interaction& e )
{
    QString module, title, description;
    int length, countdown, msz;

    m_stream >> module >> title >> description >> length >> countdown;
    m_stream >> msz;

    for (; msz-- >0;)
    {
        auto mp = new quarre::Mapping(*this, &e);
        e.m_mappings.add(mp);
        e.layout()->addLayout(mp->layout());
    }

    checkDelimiter();
}

template <> void JSONObjectReader::read(
        const quarre::Interaction& e )
{
    obj [ "Module" ]        = e.module();
    obj [ "Title"  ]        = e.title();
    obj [ "Description" ]   = e.description();
    obj [ "Length" ]        = e.length();
    obj [ "Countdown" ]     = e.countdown();
    obj [ "Mappings" ]      = toJsonArray(e.m_mappings);
}

template <> void JSONObjectWriter::write(
        quarre::Interaction& e )
{
    e.set_module        ( obj [ "Module" ].toString() );
    e.set_title         ( obj [ "Title" ].toString() );
    e.set_description   ( obj [ "Description"].tostring());
    e.set_length        ( obj [ "Length"].toInt());

    for ( const auto& json_vref : obj ["Mappings"].toArray())
    {
        JSONObject::Deserializer dsrz (json_vref.toObject);
        auto mp = new quarre::Mapping(dsrz, &e);

        e.m_mappings.add(mp);
        e.layout()->addLayout(mp->layout());

        connect(mp, SIGNAL(minusButtonPressed()), &e, SLOT(onMinusMappingButtonPressed(quarre::Mapping*)));
    }


}

