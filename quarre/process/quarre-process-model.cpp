#include "quarre-process-model.hpp"
#include <QString>
#include <QJSEngine>
#include <score/application/ApplicationContext.hpp>
#include <score/tools/IdentifierGeneration.hpp>
#include <quarre/device/quarre-device.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>

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

void quarre::ProcessModel::on_interaction_length_changed(int length)
{
    qDebug() << parent()->objectName();
    auto itv = qobject_cast<Scenario::IntervalModel*>(parent());

    if (!itv)
    {
        qDebug() << "cast did not succeed";
        return;
    }

    TimeVal default_duration, max_duration;

    if ( length == -1 )
        max_duration = TimeVal::infinite();

    else
    {
        default_duration = TimeVal::fromMsecs(
                        length*1000 + interaction()->countdown()*1000 );
        itv->duration.setDefaultDuration(default_duration);
    }

    itv->duration.setMaxDuration(max_duration);

}

void quarre::ProcessModel::on_interaction_countdown_changed(int duration)
{
    qDebug() << parent()->objectName();
    auto itv = qobject_cast<Scenario::IntervalModel*>(parent());

    if (!itv)
    {
        qDebug() << "cast did not succeed";
        return;
    }

    TimeVal min_duration = TimeVal::fromMsecs(duration*1000);
    TimeVal default_duration;

    if ( interaction()->length() != -1 )
    {
        default_duration = TimeVal::fromMsecs(
                    duration*1000+interaction()->length()*1000);

        itv->duration.setDefaultDuration( default_duration );
    }

    itv->duration.setMinDuration( min_duration );
}

void quarre::ProcessModel::startExecution()
{
    // handled by executor
}

void quarre::ProcessModel::stopExecution()
{
    // handled by executor
}

void quarre::ProcessModel::reset()
{

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



