#include "quarre-process-inspector.hpp"

#include <score/document/DocumentContext.hpp>
#include <ossia/detail/logger.hpp>
#include <QTextEdit>

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

using namespace score::addons;

quarre::InspectorWidget::InspectorWidget(
        const quarre::ProcessModel &object,
        const score::DocumentContext &ctx, QWidget *parent) :

    InspectorWidgetDelegate_T   ( object, parent ),
    m_dispatcher                ( ctx.commandStack )
{

}

void quarre::InspectorWidget::onPlusMappingButtonPressed()
{

}

void quarre::InspectorWidget::onMinusMappingButtonPressed()
{

}
