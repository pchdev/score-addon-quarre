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

    // get elements for inspector use
    m_interval = qobject_cast<Scenario::IntervalModel*>(parent);
    m_parent_scenario = qobject_cast<Scenario::ProcessModel*>(m_interval->parent());

    auto& start_state = m_parent_scenario->state    ( m_interval->startState());
    auto& end_state = m_parent_scenario->state      ( m_interval->endState() );
    m_end_event = &m_parent_scenario->event         ( end_state.eventId() );
    m_end_tsync = &m_parent_scenario->timeSync      ( m_end_event->timeSync() );
    m_start_event = &m_parent_scenario->event       ( start_state.eventId());
}

QString quarre::ProcessModel::prettyName() const
{
    return tr ( "quarre-process" );
}

quarre::interaction* quarre::ProcessModel::interaction() const
{
    return m_interactions[0];
}

Scenario::IntervalModel& quarre::ProcessModel::interval() const
{
    return *m_interval;
}

Scenario::EventModel& quarre::ProcessModel::start_event() const
{
    return *m_start_event;
}

Scenario::EventModel& quarre::ProcessModel::end_event() const
{
    return *m_end_event;
}

Scenario::TimeSyncModel& quarre::ProcessModel::end_tsync() const
{
    return *m_end_tsync;
}

Scenario::ProcessModel& quarre::ProcessModel::parent_scenario() const
{
    return *m_parent_scenario;
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



