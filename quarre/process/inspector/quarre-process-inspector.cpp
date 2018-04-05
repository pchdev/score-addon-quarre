#include "quarre-process-inspector.hpp"

#include <score/document/DocumentContext.hpp>
#include <ossia/detail/logger.hpp>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

using namespace score::addons;

quarre::InspectorWidget::InspectorWidget(
        const quarre::ProcessModel &object,
        const score::DocumentContext &ctx, QWidget *parent) :
    InspectorWidgetDelegate_T(object, parent),
    m_dispatcher(ctx.commandStack)
{
    auto title_le       = new QLineEdit ( "Untitled" );
    auto descr_le       = new QLineEdit ( "No description" );
    auto module_le      = new QLineEdit ( "Unknown" );
    auto length_sb      = new QSpinBox;
    auto countdown_sb   = new QSpinBox;
    auto sensors_cb     = new QComboBox;
    auto gestures_cb    = new QComboBox;

    sensors_cb -> addItems( { "Accelerometer_x", "Accelerometer_y", "Accelerometer_z",
                              "Rotation_x", "Rotation_y", "Rotation_z",
                              "Proximiy" });

    gestures_cb -> addItems( { "Whip", "Cover", "Turnover", "Freefall", "Pickup",
                               "Shake", "Shake/Left", "Shake/Right", "Shake/Up", "Shake/Down",
                               "Twist", "Twist/Left", "Twist/Right" });

    auto lay = new QFormLayout(this);

    lay->addRow     ( tr ( "Interaction title" ),           title_le );
    lay->addRow     ( tr ( "Interaction description" ),     descr_le );
    lay->addRow     ( tr ( "Interaction module" ),          module_le );
    lay->addRow     ( tr ( "Interaction length" ),          length_sb );
    lay->addRow     ( tr ( "Interaction countdown" ),       countdown_sb );
    lay->addRow     ( tr ( "Interaction sensors" ),         sensors_cb );
    lay->addRow     ( tr ( "Interaction gestures" ),        gestures_cb );

    connect ( title_le, SIGNAL(textChanged(QString)), &object, SLOT(onInteractionTitleChanged(QString)));
    connect ( descr_le, SIGNAL(textChanged(QString)), &object, SLOT(onInteractionDescriptionChanged(QString)));
    connect ( module_le, SIGNAL(textChanged(QString)), &object, SLOT(onInteractionModuleChanged(QString)));
    connect ( length_sb, SIGNAL(valueChanged(int)), &object, SLOT(onInteractionLengthChanged(uint16_t)));
    connect ( countdown_sb, SIGNAL(valueChanged(int)), &object, SLOT(onInteractionCountdownChanged(uint16_t)));
    connect ( sensors_cb, SIGNAL(currentIndexChanged(QString)), &object, SLOT(onInteractionSensorsChanged(QStringList)));
    connect ( gestures_cb, SIGNAL(currentIndexChanged(QString)), &object, SLOT(onInteractionGesturesChanged(QStringList)));

}
