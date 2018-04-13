#include "quarre-process-model.hpp"
#include <QString>
#include <QJSEngine>
#include <score/application/ApplicationContext.hpp>
#include <score/tools/IdentifierGeneration.hpp>
#include <quarre/device/quarre-device.hpp>

using namespace score::addons;

quarre::ProcessModel::ProcessModel(
        const TimeVal& duration,
        const Id<Process::ProcessModel>& id,
        QObject* parent ) : Process::ProcessModel(duration, id, "quarre-process", parent)
{
    metadata().setInstanceName<quarre::ProcessModel>(*this);
    m_interactions.push_back(new quarre::interaction(getStrongId(m_interactions), this));
}

QString quarre::ProcessModel::prettyName() const
{
    return tr ( "quarre-process" );
}

quarre::interaction* quarre::ProcessModel::interaction() const
{
    return m_interactions[0];
}

void quarre::ProcessModel::onInteractionTitleChanged(const QString &title)
{
    interaction()->set_title(title);
}

void quarre::ProcessModel::onInteractionDescriptionChanged(const QString &description)
{
    interaction()->set_description(description);
}

void quarre::ProcessModel::onInteractionModuleChanged(const QString &module_id)
{
    interaction()->set_module(module_id);
}

void quarre::ProcessModel::onInteractionLengthChanged(uint16_t length)
{
    interaction()->set_length(length);
}

void quarre::ProcessModel::onInteractionCountdownChanged(uint16_t countdown)
{
    interaction()->set_countdown(countdown);
}

void quarre::ProcessModel::onInteractionMappingsChanged(QVector<QStringList> mappings)
{

}

void quarre::ProcessModel::startExecution()
{
    // get quarre-server-device singleton
    // request potential user, who can use the requested sensors/gestures

    // send countdown

    // when countdown is over
    // start interaction

    const auto& mappings  = interaction()->m_mappings;
    auto qrdevice         = quarre::quarre_device::instance();
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
    readFrom<quarre::interaction>(*process.interaction() );
    insertDelimiter();
}

template <> void DataStreamWriter::write(
        quarre::ProcessModel& process )
{   
    writeTo<quarre::interaction>(*process.interaction());
    checkDelimiter();
}

template <> void JSONObjectReader::read(
        const quarre::ProcessModel& process )
{
    obj [ "Interaction" ] = toJsonObject(*process.interaction() );
}

template <> void JSONObjectWriter::write(
        quarre::ProcessModel& process )
{        
    auto json_obj = obj [ "Interaction" ].toObject();

    JSONObject::Deserializer deserializer ( json_obj );
    process.m_interactions.push_back(new quarre::interaction(deserializer, &process));
}



