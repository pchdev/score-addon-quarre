#include "quarre-process-model.hpp"
#include <QString>
#include <QJSEngine>
#include <score/application/ApplicationContext.hpp>

#include <quarre/device/quarre-device.hpp>

using namespace score::addons;

quarre::ProcessModel::ProcessModel(
        const TimeVal& duration,
        const Id<Process::ProcessModel>& id,
        QObject* parent ) : ProcessModel(duration, id, "quarrè-process", parent)
{
    metadata().setInstanceName(*this);
}

QString quarre::ProcessModel::prettyName() const
{
    return tr ( "quarrè-process" );
}

QString quarre::ProcessModel::prettyShortName() const
{
    return tr ( "quarrè" );
}

void quarre::ProcessModel::onInteractionTitleChanged(const QString &title)
{
    interaction.title = title;
}

void quarre::ProcessModel::onInteractionDescriptionChanged(const QString &description)
{
    interaction.description = description;
}

quarre::ProcessModel::onInteractionModuleChanged(const QString &module_id)
{
    interaction.module = module_id;
}

quarre::ProcessModel::onInteractionLengthChanged(uint16_t length)
{
    interaction.length = length;
}

quarre::ProcessModel::onInteractionCountdownChanged(uint16_t countdown)
{
    interaction.countdown = countdown;
}

quarre::ProcessModel::onInteractionMappingsChanged(QVector<QStringList> mappings)
{

}

void quarre::ProcessModel::startExecution()
{
    // get quarre-server-device singleton
    // request potential user, who can use the requested sensors/gestures

    // send countdown

    // when countdown is over
    // start interaction

    auto mappings   = interaction->mappings();
    auto device     = quarre::Device::instance();

    auto node = device->get_interaction_candidate(mappings);

    if ( !node ) return;
}

void quarre::ProcessModel::stopExecution()
{
    // end interaction

}

void quarre::ProcessModel::reset()
{
    // ?

}

ProcessStateDataInterface* quarre::ProcessModel::startStateData() const
{
    return nullptr;
}

ProcessStateDataInterface* quarre::ProcessModel::endStateData() const
{
    return nullptr;
}

Selection quarre::ProcessModel::selectableChildren() const
{
    return {};
}

Selection quarre::ProcessModel::selectedChildren() const
{
    return {};
}

void quarre::ProcessModel::setSelection(const Selection &s) const
{

}

void quarre::ProcessModel::setDurationAndScale(const TimeVal &newDuration)
{

}

void quarre::ProcessModel::setDurationAndGrow(const TimeVal &newDuration)
{

}

void quarre::ProcessModel::setDurationAndShrink(const TimeVal &newDuration)
{

}

// SERIALIZATION --------------------------------------------------------------------------

template <> void DataStreamReader::read(
        const quarre::ProcessModel& process )
{
    readFrom ( process.interaction );
    insertDelimiter();
}

template <> void DataStreamWriter::write(
        quarre::ProcessModel& process )
{   
    writeTo ( process.interaction );
    checkDelimiter();
}

template <> void JSONObjectReader::read(
        const quarre::ProcessModel& process )
{
    obj [ "Interaction" ] = toJsonObject( process.interaction );
}

template <> void JSONObjectWriter::write(
        quarre::ProcessModel& process )
{        
    auto json_obj = obj [ "Interaction" ].toObject();

    JSONObject::Deserializer deserializer ( json_obj );
    process.interaction = quarre::Interaction(deserializer, &process);
}



