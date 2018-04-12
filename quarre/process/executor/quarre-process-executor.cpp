#include <quarre/process/executor/quarre-process-executor.hpp>
#include <quarre/process/quarre-process-model.hpp>
#include <quarre/device/quarre-device.hpp>

#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Engine/score2OSSIA.hpp>
#include <ossia/editor/state/state_element.hpp>
#include <Engine/Executor/DocumentPlugin.hpp>

using namespace score::addons;

quarre::ProcessExecutor::ProcessExecutor(const Device::DeviceList& list) : m_devices(list)
{

}

void quarre::ProcessExecutor::start(ossia::state &st)
{
    // parse the interaction


}

void quarre::ProcessExecutor::stop()
{

}

void quarre::ProcessExecutor::pause()
{

}

void quarre::ProcessExecutor::resume()
{

}

ossia::state_element quarre::ProcessExecutor::state(ossia::time_value date, double pos)
{
    return {};
}


quarre::ProcessExecutorComponent::ProcessExecutorComponent(
        Engine::Execution::IntervalComponent &parent_interval,
        ProcessModel &element,
        const Engine::Execution::Context &ctx,
        const Id<score::Component> &id,
        QObject *parent ) :

    ProcessComponent_T ( parent_interval, element, ctx, id, "quarrè-executor-component", parent )
{
    m_ossia_process = std::make_shared<quarre::ProcessExecutor>(ctx.devices.list());
}