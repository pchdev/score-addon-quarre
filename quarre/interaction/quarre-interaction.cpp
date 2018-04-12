#include "quarre-interaction.hpp"

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>
#include <QFormLayout>

using namespace score::addons;

quarre::interaction::interaction(
        const Id<quarre::interaction> &id,
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

QStringList quarre::interaction::inputs() const
{
    QStringList res;

    for ( const quarre::mapping& mapping : m_mappings )
    {
        QString source = mapping.source();
        QString res_src = source.remove("/user/0");
        res << res_src;
    }

    return res;
}

QVBoxLayout* quarre::interaction::layout() const
{
    return m_layout;
}

std::shared_ptr<score::EntityMap<quarre::mapping> >quarre::interaction::mappings() const
{
    std::shared_ptr<score::EntityMap<quarre::mapping> ptr(&m_mappings);
    return ptr;
}

const QString quarre::interaction::module() const
{
    return m_module->text();
}

const QString quarre::interaction::title() const
{
    return m_title->text();
}

const QString quarre::interaction::description() const
{
    return m_description->text();
}

int quarre::interaction::length() const
{
    return m_length->value();
}

int quarre::interaction::countdown() const
{
    return m_countdown->value();
}

void quarre::interaction::set_module(const QString &module)
{
    m_module->setText(module);
}

void quarre::interaction::set_title(const QString &title)
{
    m_title->setText(title);
}

void quarre::interaction::set_description(const QString &description)
{
    m_description->setText(description);
}

void quarre::interaction::set_length(const int length)
{
    m_length->setValue(length);
}

void quarre::interaction::set_countdown(const int countdown)
{
    m_countdown->setValue(countdown);
}

void quarre::interaction::onPlusMappingButtonPressed()
{
    m_mappings.add(new quarre::mapping);
}

void quarre::interaction::set_current_host(std::shared_ptr<quarre::user> host)
{
    m_host = host;
}

std::shared_ptr<quarre::user> quarre::interaction::current_host() const
{
    return m_host;
}

void quarre::interaction::onMinusMappingButtonPressed()
{
    auto sender = qobject_cast<quarre::mapping*>(QObject::sender());
    m_mappings.erase(sender);
}

template <> void DataStreamReader::read(
        const quarre::interaction& e )
{
    m_stream << e.module();
    m_stream << e.title();
    m_stream << e.description();
    m_stream << e.length();
    m_stream << e.countdown();

    m_stream << e.mappings()->size();
    for ( const auto& mapping : e.mappings() )
        readFrom( mapping );

    insertDelimiter();
}

template <> void DataStreamWriter::write(
        quarre::interaction& e )
{
    QString module, title, description;
    int length, countdown, msz;

    m_stream >> module >> title >> description >> length >> countdown;
    m_stream >> msz;

    for (; msz-- >0;)
    {
        auto mp = new quarre::mapping(*this, &e);
        e.m_mappings.add(mp);
        e.layout()->addLayout(mp->layout());
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
    obj [ "mappings" ]      = toJsonArray(e.m_mappings);
}

template <> void JSONObjectWriter::write(
        quarre::interaction& e )
{
    e.set_module        ( obj [ "Module" ].toString() );
    e.set_title         ( obj [ "Title" ].toString() );
    e.set_description   ( obj [ "Description"].toString());
    e.set_length        ( obj [ "Length"].toInt());

    for ( const auto& json_vref : obj ["mappings"].toArray())
    {
        JSONObject::Deserializer dsrz (json_vref.toObject());
        auto mp = new quarre::mapping(dsrz, &e);

        e.m_mappings.add(mp);
        e.layout()->addLayout(mp->layout());

        QObject::connect(mp, SIGNAL(minusButtonPressed()), &e, SLOT(onMinusmappingButtonPressed(quarre::mapping*)));
    }


}

