#include "quarre-process-inspector.hpp"
#include <score/document/DocumentContext.hpp>
#include <ossia/detail/logger.hpp>
#include <QFormLayout>
#include <QVBoxLayout>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Scenario/Commands/Interval/SetMaxDuration.hpp>
#include <Scenario/Commands/Interval/SetMinDuration.hpp>
#include <Scenario/Commands/Scenario/Displacement/MoveEvent.hpp>
#include <Scenario/Commands/Scenario/Displacement/MoveEventMeta.hpp>
#include <Scenario/Document/BaseScenario/BaseScenario.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/TimeSync/TimeSyncModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>

#include <score/tools/Todo.hpp>

using namespace score::addons;

quarre::mapping_view::mapping_view(const score::DocumentContext &dctx, mapping &map, QWidget* parent) :

    m_source            ( 0 ),
    m_destination       ( 0 ),
    m_expression        ( new QTextEdit ),
    m_minus_button      ( new QPushButton ("remove mapping") ),
    m_layout            ( new QVBoxLayout ),
    m_mapmodel          ( &map )
{
    auto& explorer      = dctx.plugin<Explorer::DeviceDocumentPlugin>().explorer();
    m_source            = new Explorer::AddressAccessorEditWidget(explorer, 0);
    m_destination       = new Explorer::AddressAccessorEditWidget(explorer, 0);

    set_source          ( map.source() );
    set_destination     ( map.destination() );
    set_expression      ( map.expression() );

    m_form              = new QFormLayout;

    m_form->addRow ( tr ( "delete" ), m_minus_button );
    m_form->addRow ( tr ( "source" ), m_source );
    m_form->addRow ( tr ( "destination" ), m_destination );

    m_layout->addLayout ( m_form );
    m_layout->addWidget ( m_expression );

    m_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);

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

quarre::InspectorWidget::InspectorWidget(const quarre::ProcessModel& object,
        const score::DocumentContext &ctx, QWidget *parent) :

    InspectorWidgetDelegate_T   ( object, parent ),
    m_dispatcher                ( ctx.commandStack ),

    m_quarre_process    ( object ),
    m_layout            ( new QVBoxLayout(this) ),
    m_module            ( new QComboBox ),
    m_title             ( new QLineEdit ( object.interaction()->title())),
    m_description       ( new QLineEdit ( object.interaction()->description())),
    m_end_expression    ( new QLineEdit ( object.interaction()->end_expression())),
    m_length            ( new QSpinBox ),
    m_countdown         ( new QSpinBox ),
    m_dispatch_all      ( new QCheckBox ),
    m_mapping_area      ( new QScrollArea ),
    m_dctx              ( ctx )
{   
    auto& explorer              = ctx.plugin<Explorer::DeviceDocumentPlugin>().explorer();
    m_end_expression_source     = new Explorer::AddressAccessorEditWidget(explorer, this);

    m_interaction = object.interaction();

    State::AddressAccessor ahem;
    auto addr = m_interaction->end_expression_source();
    m_end_expression_source->setAddress(State::AddressAccessor::fromString(addr).value_or(ahem));

    m_length->setValue      ( m_interaction->length() );
    m_length->setRange      ( 0, 999 );

    m_countdown->setValue   ( m_interaction->countdown() );
    m_countdown->setRange   ( 0, 999 );

    setObjectName           ( "quarrè-process-inspector" );
    setParent               ( parent );

    m_module->addItem   ( "Default"  );
    m_module->addItem   ( "Tutorial" );
    m_module->addItem   ( "Vote" );
    m_module->addItem   ( "Transition" );
    m_module->addItem   ( "Gesture" );
    m_module->addItem   ( "Pads" );
    m_module->addItem   ( "Sliders" );
    m_module->addItem   ( "Strings" );
    m_module->addItem   ( "TouchSpatialization" );
    m_module->addItem   ( "SensorSpatialization" );
    m_module->addItem   ( "TouchTrajectories" );
    m_module->addItem   ( "TouchBirds" );

    m_module->setCurrentText    ( m_interaction->module() );
    m_dispatch_all->setChecked  ( m_interaction->dispatch_all() );

    auto form = new QFormLayout;
    form->addRow ( tr ("Title"), m_title );
    form->addRow ( tr ("Description"), m_description );
    form->addRow ( tr ("Module"), m_module );
    form->addRow ( tr ("Dispatch to all users"), m_dispatch_all );
    form->addRow ( tr ("Length"), m_length );
    form->addRow ( tr ("Countdown"), m_countdown);
    form->addRow ( tr ("Ending source"), m_end_expression_source);
    form->addRow ( tr ("Expression"), m_end_expression );

    auto plusb = new QPushButton("add mapping");

    m_layout->addLayout( form );
    m_layout->addWidget( plusb );
    m_layout->addWidget( m_mapping_area );

    m_mapping_layout = new QVBoxLayout(m_mapping_area);
    m_mapping_area->setWidgetResizable(true);
    m_mapping_layout->setSizeConstraint(QLayout::SetNoConstraint);

    for ( quarre::mapping* mapping : m_interaction->mappings() )
        on_mapping_added(*mapping);

    connect(m_dispatch_all, SIGNAL(clicked(bool)), m_interaction, SLOT(on_dispatch_all_changed(bool)));
    connect(m_title, SIGNAL(textChanged(QString)), m_interaction, SLOT(on_title_changed(QString)));
    connect(m_description, SIGNAL(textChanged(QString)), m_interaction, SLOT(on_description_changed(QString)));
    connect(m_module, SIGNAL(currentIndexChanged(QString)), m_interaction, SLOT(on_module_changed(QString)));
    connect(m_length, SIGNAL(valueChanged(int)), m_interaction, SLOT(on_length_changed(int)));
    connect(m_countdown, SIGNAL(valueChanged(int)), m_interaction, SLOT(on_countdown_changed(int)));
    connect(m_end_expression_source, SIGNAL(addressChanged(Device::FullAddressAccessorSettings)), this, SLOT (on_address_changed(Device::FullAddressAccessorSettings)));
    connect(m_end_expression, SIGNAL(textChanged(QString)), m_interaction, SLOT(on_end_expression_changed(QString)));
    connect(this, SIGNAL(address_changed(QString)), m_interaction, SLOT(on_end_expression_source_changed(QString)));
    connect(plusb, SIGNAL(released()), m_interaction, SLOT(on_mapping_added()));

    connect(m_length, SIGNAL(valueChanged(int)), this, SLOT(update(int)));
    connect(m_countdown, SIGNAL(valueChanged(int)), this, SLOT(update(int)));

    connect( m_interaction, SIGNAL(mapping_added(quarre::mapping&)), this, SLOT(on_mapping_added(quarre::mapping&)));
    connect( this, SIGNAL(mappingDeleteRequest(quarre::mapping*)), m_interaction, SLOT(on_mapping_removed(quarre::mapping*)));

}

void quarre::InspectorWidget::on_address_changed(Device::FullAddressAccessorSettings address)
{
    emit address_changed(address.address.toString());
}

void quarre::InspectorWidget::update(int)
{
    auto& interval      = m_quarre_process.interval();
    auto length         = m_quarre_process.interaction()->length()*1000;
    auto countdown      = m_quarre_process.interaction()->countdown()*1000;
    auto start_date     = m_quarre_process.start_event().date().msec();

    auto min_dur        = TimeVal::fromMsecs(countdown);
    auto max_dur        = TimeVal::fromMsecs(length+countdown);
    auto end_date       = TimeVal::fromMsecs(start_date+length+countdown);

    auto cmd_min = new Scenario::Command::SetMinDuration(interval, min_dur, false);
    auto cmd_max = new Scenario::Command::SetMaxDuration(interval, max_dur, false);

    auto cmd_default = new Scenario::Command::MoveEventMeta(
                m_quarre_process.parent_scenario(),
                m_quarre_process.end_event().id(),
                end_date,
                interval.heightPercentage(),
                ExpandMode::GrowShrink,
                LockMode::Free );

    m_dispatcher.submitCommand(cmd_default);
    m_dispatcher.submitCommand(cmd_min);
    m_dispatcher.submitCommand(cmd_max);

}

void quarre::InspectorWidget::on_mapping_added(quarre::mapping &mapping)
{
    auto mapview = new quarre::mapping_view ( m_dctx, mapping, this );
    QObject::connect(mapview, SIGNAL(mappingDeleteRequest(quarre::mapping*)), this, SLOT(on_mapping_removed(quarre::mapping*)));

    m_mapping_layout->addLayout(mapview->layout());
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
