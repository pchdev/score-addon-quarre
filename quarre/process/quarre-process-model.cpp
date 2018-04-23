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
        QObject* parent ) :
    Process::ProcessModel(duration, id, Metadata<ObjectKey_k, quarre::ProcessModel>::get(), parent)
{
    metadata().setInstanceName<quarre::ProcessModel>(*this);

    m_interactions.push_back(new quarre::interaction(getStrongId(m_interactions), this));

    initialize_parent_pointers              ( );
    m_interval->duration.setRigid           ( false );
    m_interval->duration.setMinNull         ( true );
    m_interval->duration.setMaxInfinite     ( true );

    end_tsync().setActive       ( true );
    end_tsync().setExpression   ( State::defaultFalseExpression() );
}

void quarre::ProcessModel::initialize_parent_pointers()
{
    m_interval          = qobject_cast<Scenario::IntervalModel*>(parent());
    m_parent_scenario   = qobject_cast<Scenario::ProcessModel*>(m_interval->parent());
}


ossia::time_sync& quarre::ProcessModel::get_ossia_tsync() const
{
    auto base = dynamic_cast<Engine::Execution::IntervalComponentBase*>(m_interval_component);
    auto itv = base->OSSIAInterval();
    auto& tsync = itv->get_end_event().get_time_sync();

    return tsync;
}

void quarre::ProcessModel::set_interval_component(Engine::Execution::IntervalComponent &component)
{
    m_interval_component = &component;
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
    auto& start_state = m_parent_scenario->state(m_interval->startState());
    return m_parent_scenario->event(start_state.eventId());
}

Scenario::EventModel& quarre::ProcessModel::end_event() const
{
    auto& end_state = m_parent_scenario->state(m_interval->endState());
    return m_parent_scenario->event(end_state.eventId());
}

Scenario::TimeSyncModel& quarre::ProcessModel::end_tsync() const
{
    auto& end_state = m_parent_scenario->state(m_interval->endState());
    auto& end_event = m_parent_scenario->event(end_state.eventId());
    auto& end_tsync = m_parent_scenario->timeSync(end_event.timeSync() );

    return end_tsync;
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
    process.m_interactions.push_back(new quarre::interaction(getStrongId(process.m_interactions), &process));
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



