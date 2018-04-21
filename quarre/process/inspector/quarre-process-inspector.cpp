#include "quarre-process-inspector.hpp"
#include <score/document/DocumentContext.hpp>
#include <ossia/detail/logger.hpp>
#include <QFormLayout>
#include <QVBoxLayout>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>

using namespace score::addons;

quarre::mapping_view::mapping_view(const score::DocumentContext &dctx, mapping &map, QWidget* parent) :

    m_source            ( 0 ),
    m_destination       ( 0 ),
    m_expression        ( new QTextEdit ),
    m_minus_button      ( new QPushButton ("-") ),
    m_layout            ( new QVBoxLayout ),
    m_mapmodel          ( &map )
{
    auto& explorer      = dctx.plugin<Explorer::DeviceDocumentPlugin>().explorer();
    m_source            = new Explorer::AddressAccessorEditWidget(explorer, parent);
    m_destination       = new Explorer::AddressAccessorEditWidget(explorer, parent);

    set_source          ( map.source() );
    set_destination     ( map.destination() );
    set_expression      ( map.expression() );

    m_expression        ->setPlainText("( function(v, dest) { dest[\"value\"] = v; } )");

    m_form    = new QFormLayout;
    m_form->addRow ( tr ( "delete" ), m_minus_button );
    m_form->addRow ( tr ( "source" ), m_source );
    m_form->addRow ( tr ( "destination" ), m_destination );

    m_layout->addLayout ( m_form );
    m_layout->addWidget ( m_expression );

    QObject::connect(m_source, SIGNAL(addressChanged(Device::FullAddressAccessorSettings)), this, SLOT(on_source_address_changed(Device::FullAddressAccessorSettings)));
    QObject::connect(m_destination, SIGNAL(addressChanged(Device::FullAddressAccessorSettings)), this, SLOT(on_destination_address_changed(Device::FullAddressAccessorSettings)));
    QObject::connect(m_expression, SIGNAL(textChanged()), this, SLOT(on_expression_changed()));
    QObject::connect(m_minus_button, SIGNAL(released()), this, SLOT(on_minus_button_pressed()));

    QObject::connect(this, SIGNAL(sourceChanged(QString)), &map, SLOT(on_source_changed(QString)));
    QObject::connect(this, SIGNAL(destinationChanged(QString)), &map, SLOT(on_destination_changed(QString)));
    QObject::connect(this, SIGNAL(expressionChanged(QString)), &map, SLOT(on_expression_changed(QString)));

}

quarre::mapping_view::~mapping_view()
{
    delete m_source;
    delete m_destination;
    delete m_expression;
    delete m_minus_button;
    delete m_form;
    delete m_layout;
}

void quarre::mapping_view::on_minus_button_pressed()
{
    emit mappingDeleteRequest(m_mapmodel);
}

void quarre::mapping_view::on_source_address_changed(const Device::FullAddressAccessorSettings)
{
    emit sourceChanged(m_source->addressString());
}

void quarre::mapping_view::on_destination_address_changed(const Device::FullAddressAccessorSettings)
{
    emit destinationChanged(m_destination->addressString());
}

void quarre::mapping_view::on_expression_changed()
{
    emit expressionChanged(m_expression->toPlainText());
}

QVBoxLayout* quarre::mapping_view::layout() const
{
    return m_layout;
}

const QString quarre::mapping_view::source() const
{
    return m_source->addressString();
}

const QString quarre::mapping_view::destination() const
{
    return m_destination->addressString();
}

const QString quarre::mapping_view::expression() const
{
    return m_expression->toPlainText();
}

void quarre::mapping_view::set_source(const QString &source)
{
    State::AddressAccessor lame;
    m_source->setAddress(State::AddressAccessor::fromString(source).value_or(lame));
}

void quarre::mapping_view::set_destination(const QString &destination)
{
    State::AddressAccessor lame;
    m_destination->setAddress(State::AddressAccessor::fromString(destination).value_or(lame));
}

void quarre::mapping_view::set_expression(const QString &expression)
{
    m_expression->setPlainText(expression);
}

quarre::InspectorWidget::InspectorWidget(const ProcessModel &object,
        const score::DocumentContext &ctx, QWidget *parent) :

    InspectorWidgetDelegate_T   ( object, parent ),
    m_dispatcher                ( ctx.commandStack ),
    m_layout            ( new QVBoxLayout(this) ),
    m_module            ( new QLineEdit ( object.interaction()->module())),
    m_title             ( new QLineEdit ( object.interaction()->title())),
    m_description       ( new QLineEdit ( object.interaction()->description())),
    m_length            ( new QSpinBox ),
    m_countdown         ( new QSpinBox ),
    m_dctx              ( ctx )
{

    auto& explorer      = ctx.plugin<Explorer::DeviceDocumentPlugin>().explorer();
    m_end_expression    = new Explorer::AddressAccessorEditWidget(explorer, this);

    m_interaction = object.interaction();

    m_length->setValue      ( m_interaction->length() );
    m_length->setRange      ( 0, 999 );

    m_countdown->setValue   ( m_interaction->countdown() );
    m_countdown->setRange   ( 0, 999 );

    setObjectName           ( "quarrè-process-inspector" );
    setParent               ( parent );

    auto form = new QFormLayout;
    form->addRow ( tr ("Title"), m_title );
    form->addRow ( tr ("Description"), m_description );
    form->addRow ( tr ("Module"), m_module );
    form->addRow ( tr ("Length"), m_length );
    form->addRow ( tr ("Countdown"), m_countdown);
    form->addRow ( tr ("Ending"), m_end_expression );

    auto plusb = new QPushButton("+");

    m_layout->addLayout( form );
    m_layout->addWidget( plusb );

    for ( quarre::mapping* mapping : m_interaction->mappings() )
        on_mapping_added(*mapping);

    connect(m_title, SIGNAL(textChanged(QString)), m_interaction, SLOT(onTitleChanged(QString)));
    connect(m_description, SIGNAL(textChanged(QString)), m_interaction, SLOT(onDescriptionChanged(QString)));
    connect(m_module, SIGNAL(textChanged(QString)), m_interaction, SLOT(onModuleChanged(QString)));
    connect(m_length, SIGNAL(valueChanged(int)), m_interaction, SLOT(onLengthChanged(int)));
    connect(m_countdown, SIGNAL(valueChanged(int)), m_interaction, SLOT(onCountdownChanged(int)));
    connect(plusb, SIGNAL(released()), m_interaction, SLOT(on_mapping_added()));

    connect(m_length, SIGNAL(valueChanged(int)), &object, SLOT(on_interaction_length_changed(int)));
    connect(m_countdown, SIGNAL(valueChanged(int)), &object, SLOT(on_interaction_countdown_changed(int)));

    connect( m_interaction, SIGNAL(mapping_added(quarre::mapping&)), this, SLOT(on_mapping_added(quarre::mapping&)));
    connect( this, SIGNAL(mappingDeleteRequest(quarre::mapping*)), m_interaction, SLOT(on_mapping_removed(quarre::mapping*)));

}

void quarre::InspectorWidget::on_mapping_added(quarre::mapping &mapping)
{
    auto mapview = new quarre::mapping_view ( m_dctx, mapping, this );
    QObject::connect(mapview, SIGNAL(mappingDeleteRequest(quarre::mapping*)), this, SLOT(on_mapping_removed(quarre::mapping*)));

    m_layout->addLayout(mapview->layout());
    m_mapping_views.push_back( mapview );
}

void quarre::InspectorWidget::on_mapping_removed(quarre::mapping* mapping )
{
    // delete view first, chain on signal to model
    auto sender = qobject_cast<quarre::mapping_view*>(QObject::sender());
    m_layout->removeItem(sender->layout());
    m_mapping_views.removeAll(sender);
    delete sender;

    emit mappingDeleteRequest(mapping);
}
