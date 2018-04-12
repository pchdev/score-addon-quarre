#include "score_addon_quarre.hpp"
#include <score/plugins/customfactory/FactorySetup.hpp>

#include <quarre/application-plugin/quarre-application-plugin.hpp>

#include <quarre/process/executor/quarre-process-executor.hpp>
#include <quarre/process/quarre-process-factory.hpp>
#include <quarre/process/quarre-process-model.hpp>
#include <quarre/process/inspector/quarre-process-inspector.hpp>
#include <quarre/panel/quarre-panel-delegate-factory.hpp>
#include <quarre/device/quarre-protocol-factory.hpp>

using namespace score::addons;

score_addon_quarre::score_addon_quarre()
{

}

score_addon_quarre::~score_addon_quarre()
{

}

auto score_addon_quarre::required() const
  -> std::vector<score::PluginKey>
{
    return {};
}

void score_addon_quarre::updateSaveFile(
        QJsonObject& obj,
        score::Version obj_version,
        score::Version current_version)
{

}

std::vector<std::unique_ptr<score::InterfaceListBase>> score_addon_quarre::factoryFamilies()
{
    return {};
}

std::vector<std::unique_ptr<score::InterfaceBase>> score_addon_quarre::factories(
        const score::ApplicationContext& ctx,
        const score::InterfaceKey& key) const
{
    return instantiate_factories<score::ApplicationContext,

            FW<Process::ProcessModelFactory, quarre::ProcessFactory>,
            FW<Process::InspectorWidgetDelegateFactory, quarre::InspectorFactory>,
            FW<Engine::Execution::ProcessComponentFactory, quarre::ProcessExecutor>,
            FW<score::PanelDelegateFactory, quarre::PanelDelegateFactory>,
            FW<Device::ProtocolFactory, quarre::ProtocolFactory>

    >(ctx, key);
}

score::GUIApplicationPlugin* score_addon_quarre::make_guiApplicationPlugin(
        const score::GUIApplicationContext& app)
{
    return new quarre::ApplicationPlugin ( app );
}

std::pair<const CommandGroupKey, CommandGeneratorMap> score_addon_quarre::make_commands()
{
    std::pair<const CommandGroupKey, CommandGeneratorMap> cmds{
        CommandFactoryName(),
        CommandGeneratorMap{}};

    // CMake generates the "addon_commands.hpp" and "addon_commands_file.hpp"
    // by scanning the source files for \ref score_COMMAND_DECL or \ref score_COMMAND_DECL_T.
    using Types = TypeList<
#include <score_addon_quarre_commands.hpp>
      >;

    for_each_type<Types>(score::commands::FactoryInserter{cmds.second});

    return cmds;
}
