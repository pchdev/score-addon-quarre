#include "quarre-process-inspector.hpp"
#include <score/document/DocumentContext.hpp>
#include <ossia/detail/logger.hpp>
#include <QFormLayout>
#include <QVBoxLayout>

using namespace score::addons;

quarre::InspectorWidget::InspectorWidget(const ProcessModel &object,
        const score::DocumentContext &ctx, QWidget *parent) :

    InspectorWidgetDelegate_T   ( object, parent ),
    m_dispatcher                ( ctx.commandStack ),
    m_layout            ( new QVBoxLayout(this) ),
    m_module            ( new QLineEdit ( object.interaction()->module())),
    m_title             ( new QLineEdit ( object.interaction()->title())),
    m_description       ( new QLineEdit ( object.interaction()->description())),
    m_length            ( new QSpinBox ),
    m_countdown         ( new QSpinBox )
{

    quarre::interaction* interaction = object.interaction();

    m_length->setValue      ( interaction->length() );
    m_countdown->setValue   ( interaction->countdown() );
    setObjectName           ( "quarrè-process-inspector" );
    setParent               ( parent );

    auto form = new QFormLayout;
    form->addRow ( tr ("Title"), m_title );
    form->addRow ( tr ("Description"), m_description );
    form->addRow ( tr ("Module"), m_module );
    form->addRow ( tr ("Length"), m_length );
    form->addRow ( tr ("Countdown"), m_countdown);

    auto plusb = new QPushButton("+");

    m_layout->addLayout( form );
    m_layout->addWidget( plusb );

    connect(m_title, SIGNAL(textChanged(QString)), interaction, SLOT(onTitleChanged(QString)));
    connect(m_description, SIGNAL(textChanged(QString)), interaction, SLOT(onDescriptionChanged(QString)));
    connect(m_module, SIGNAL(textChanged(QString)), interaction, SLOT(onModuleChanged(QString)));
    connect(m_length, SIGNAL(valueChanged(int)), interaction, SLOT(onLengthChanged(int)));
    connect(m_countdown, SIGNAL(valueChanged(int)), interaction, SLOT(onCountdownChanged(int)));
    connect(plusb, SIGNAL(pressed()), interaction, SLOT(onPlusMappingButtonPressed()));

    connect( interaction, SIGNAL(mapping_added(quarre::mapping)), this, SLOT(on_mapping_added(quarre::mapping)));

}

void quarre::InspectorWidget::on_mapping_added(const quarre::mapping &mapping)
{
    m_layout->addLayout(mapping.layout());
}
