#include <quarre/process/executor/quarre-process-executor.hpp>
#include <quarre/process/quarre-process-model.hpp>
#include <quarre/device/quarre-device.hpp>

#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Engine/score2OSSIA.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <Engine/Executor/DocumentPlugin.hpp>

using namespace score::addons;

quarre::ProcessExecutor::ProcessExecutor(const Device::DeviceList& list,
        Engine::Execution::IntervalComponent& interval, ProcessModel &model) :
    m_devices(list),
    m_interval(interval),
    m_model(model)
{
    model.set_interval_component(interval);
}

void quarre::ProcessExecutor::start(ossia::state &st)
{    
    // update the local node tree
    // if interaction has not been dispatched due to its requirements
    // this allows to make an automated parallel process in such cases

    if ( quarre::dispatcher::dispatch_incoming_interaction(*m_model.interaction()))
    {
        auto& dispatched = m_model.get_dispatched_parameter();
        dispatched.push_value ( true );
    }
}

void quarre::ProcessExecutor::stop()
{
    quarre::dispatcher::dispatch_ending_interaction(*m_model.interaction());
}

void quarre::ProcessExecutor::pause()
{
    quarre::dispatcher::dispatch_paused_interaction(*m_model.interaction());
}

void quarre::ProcessExecutor::resume()
{
    quarre::dispatcher::dispatch_resumed_interaction(*m_model.interaction());
}

ossia::state_element quarre::ProcessExecutor::state(ossia::time_value date, double pos)
{
    // when countdown is over (date is microseconds)
    // start the interaction

    auto cd = m_model.interaction()->countdown();
    cd*=1000000;
    if ( date >= cd && date < cd+5000 )
    {
        qDebug() << "trigger interacton";
        quarre::dispatcher::dispatch_active_interaction(*m_model.interaction());
    }

    return {};
}


quarre::ProcessExecutorComponent::ProcessExecutorComponent(
        Engine::Execution::IntervalComponent& parent_interval,
        ProcessModel &element,
        const Engine::Execution::Context &ctx,
        const Id<score::Component> &id,
        QObject *parent ) :

    ProcessComponent_T ( parent_interval, element, ctx, id, "quarrè-executor-component", parent )
{
    m_ossia_process = std::make_shared<quarre::ProcessExecutor>(ctx.devices.list(), parent_interval, element);
}
