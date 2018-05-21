#pragma once

#include <Process/Process.hpp>
#include "quarre-process-metadata.hpp"
#include <quarre/interaction/quarre-interaction.hpp>
#include <score/model/EntityMap.hpp>

#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/TimeSync/TimeSyncModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Engine/Executor/IntervalComponent.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class interaction;

class ProcessModel final : public Process::ProcessModel
{
    SCORE_SERIALIZE_FRIENDS
    PROCESS_METADATA_IMPL       ( score::addons::quarre::ProcessModel )

    Q_OBJECT

    public: //--------------------------------------------
    explicit ProcessModel ( const TimeVal& duration,
                   const Id<Process::ProcessModel>& id,
                   QObject* parent);

    template<typename impl>
    explicit ProcessModel(impl& vis, QObject* parent) :
        Process::ProcessModel(vis, parent)
    {
        // get elements for inspector use
        vis.writeTo(*this);
        initialize_parent_pointers();
    }

    // clone
    explicit ProcessModel(
            const ProcessModel& other,
            const Id<Process::ProcessModel>& id,
            QObject* parent) :
        Process::ProcessModel (other, id, Metadata<ObjectKey_k,ProcessModel>::get(), parent )
    {
        metadata().setInstanceName<quarre::ProcessModel>(*this);
    }

    void initialize_parent_pointers();

    quarre::interaction*        interaction         ( ) const;
    Scenario::IntervalModel&    interval            ( ) const;
    Scenario::EventModel&       start_event         ( ) const;
    Scenario::EventModel&       end_event           ( ) const;
    Scenario::TimeSyncModel&    end_tsync           ( ) const;
    Scenario::ProcessModel&     parent_scenario     ( ) const;

    void set_dispatched_parameter   ( ossia::net::parameter_base& p );
    ossia::net::parameter_base& get_dispatched_parameter ();

    void set_interval_component ( Engine::Execution::IntervalComponent& component );
    ossia::time_sync& get_ossia_tsync() const;

    private: //-------------------------------------------------------------------

    std::vector<quarre::interaction*>       m_interactions;
    Engine::Execution::IntervalComponent*   m_interval_component;

    Scenario::IntervalModel*    m_interval;
    Scenario::ProcessModel*     m_parent_scenario;

    ossia::net::parameter_base*      m_dispatched;

    virtual QString prettyName      ( ) const override;

    virtual void startExecution     ( ) override;
    virtual void stopExecution      ( ) override;
    virtual void reset              ( ) override;

    virtual ProcessStateDataInterface* startStateData   ( ) const override;
    virtual ProcessStateDataInterface* endStateData     ( ) const override;

    virtual Selection selectableChildren    ( ) const override;
    virtual Selection selectedChildren      ( ) const override;

    virtual void setSelection ( const Selection& s ) const override;

    virtual void setDurationAndScale    ( const TimeVal& newDuration ) override;
    virtual void setDurationAndGrow     ( const TimeVal& newDuration ) override;
    virtual void setDurationAndShrink   ( const TimeVal& newDuration ) override;
};

}
}
}
