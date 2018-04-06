#include "quarre-process-inspector.hpp"

#include <score/document/DocumentContext.hpp>
#include <ossia/detail/logger.hpp>

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
    quarre::Interaction interaction = object.interaction;
    m_selected_gestures = interaction.gestures;
    m_selected_sensors = interaction.sensors;

    m_layout = new QFormLayout(this);

    auto title_le       = new QLineEdit ( interaction.title );
    auto descr_le       = new QLineEdit ( interaction.description );
    auto module_le      = new QLineEdit ( interaction.module );

    auto length_sb      = new QSpinBox;
    length_sb           ->setValue ( interaction.length );

    auto countdown_sb   = new QSpinBox;
    countdown_sb        ->setValue ( interaction.countdown );

    m_layout->addRow     ( tr ( "Interaction title" ),           title_le );
    m_layout->addRow     ( tr ( "Interaction description" ),     descr_le );
    m_layout->addRow     ( tr ( "Interaction module" ),          module_le );
    m_layout->addRow     ( tr ( "Interaction length" ),          length_sb );
    m_layout->addRow     ( tr ( "Interaction countdown" ),       countdown_sb );

    connect ( title_le,     SIGNAL(textChanged(QString)),   &object, SLOT(onInteractionTitleChanged(QString)));
    connect ( descr_le,     SIGNAL(textChanged(QString)),   &object, SLOT(onInteractionDescriptionChanged(QString)));
    connect ( module_le,    SIGNAL(textChanged(QString)),   &object, SLOT(onInteractionModuleChanged(QString)));
    connect ( length_sb,    SIGNAL(valueChanged(int)),      &object, SLOT(onInteractionLengthChanged(uint16_t)));
    connect ( countdown_sb, SIGNAL(valueChanged(int)),      &object, SLOT(onInteractionCountdownChanged(uint16_t)));

    // ------ SENSORS --------------------------------------------------

    QListWidget* sensors = new QListWidget;

    for ( const auto& sensor : sensors_list )
    {
        QListWidgetItem* item = new QListWidgetItem(sensor, sensors);
        if ( interaction.sensors.contains(sensor) )
             item->setCheckState ( Qt::Checked );

        else item->setCheckState ( Qt::Unchecked );
    }

    connect ( sensors, SIGNAL(itemClicked(QListWidgetItem*))), this, SLOT(onSensorsSelectionChanged(QListWidgetItem*));

    // ------ GESTURES --------------------------------------------------

    QListWidget* gestures = new QListWidget;

    for ( const auto& gesture : gestures_list )
    {
        QListWidgetItem* item = new QListWidgetItem(gesture, gestures);
        if ( interaction.gestures.contains(gesture) )
            item->setCheckState ( Qt::Checked );

        else item->setCheckState ( Qt::Unchecked );
    }

    connect ( gestures, SIGNAL(itemClicked(QListWidgetItem*))), this, SLOT(onGesturesSelectionChanged(QListWidgetItem*));

    // ------------------------------------------------------------------

    connect ( this, SIGNAL(sensorListChanged(QStringList)), &object, SLOT(onInteractionSensorsChanged(QStringList)));
    connect ( this, SIGNAL(gestureListChanged(QStringList)), &object, SLOT(onInteractionGesturesChanged(QStringList)));

}

void quarre::InspectorWidget::onSensorsSelectionChanged(QListWidgetItem* item)
{
    if ( item->checkState() == Qt::Unchecked )
        m_selected_sensors.removeAll(item->text());

    else if ( ! m_selected_sensors.contains(item->text()) )
            m_selected_sensors.push_back(item->text());

    emit sensorListChanged(m_selected_sensors);
}

void quarre::InspectorWidget::onGesturesSelectionChanged(QListWidgetItem* item)
{
    if ( item->checkState() == Qt::Unchecked )
        m_selected_gestures.removeAll(item->text());

    else if ( ! m_selected_gestures.contains(item->text()) )
            m_selected_gestures.push_back(item->text());

    emit gestureListChanged(m_selected_gestures);
}
