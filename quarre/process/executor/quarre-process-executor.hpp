#pragma once

#include <Engine/Executor/ProcessComponent.hpp>
#include <Device/Protocol/DeviceList.hpp>
#include <quarre/process/quarre-process-model.hpp>
#include <Engine/Executor/IntervalComponent.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class ProcessModel;

class ProcessExecutor final : public ossia::time_process
{
    public: //-----------------------------
    ProcessExecutor(
            const Device::DeviceList& list,
            Engine::Execution::IntervalComponent& interval,
            quarre::ProcessModel& model);

    virtual void start      ( ossia::state &st ) override;
    virtual void stop       ( ) override;
    virtual void pause      ( ) override;
    virtual void resume     ( ) override;

    virtual ossia::state_element state ( ossia::time_value date, double pos ) override;

    bool dispatchable ( ) const;

    private: // --------------------------
    const Device::DeviceList& m_devices;
    Engine::Execution::IntervalComponentBase& m_interval;
    quarre::ProcessModel& m_model;

};

class ProcessExecutorComponent final : public ::Engine::Execution::ProcessComponent_T<
        score::addons::quarre::ProcessModel,
        score::addons::quarre::ProcessExecutor>
{
    COMPONENT_METADATA  ( "B165C486-70B2-4252-BD9A-D840F251ABDD" )

    public:
        ProcessExecutorComponent(
            Engine::Execution::IntervalComponent& parent_interval,
            ProcessModel& element,
            const Engine::Execution::Context& ctx,
            const Id<score::Component>& id,
            QObject* parent );

};

using ProcessExecutorComponentFactory =
        Engine::Execution::ProcessComponentFactory_T<ProcessExecutorComponent>;

}
}
}
