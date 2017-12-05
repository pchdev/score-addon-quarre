/*
 * =====================================================================================
 *
 *       Filename:  score_addon_quarre.cpp
 *
 *    Description:  main source
 *
 *        Version:  1.0
 *        Created:  05.12.2017 17:31:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  pchd 
 *   Organization:  quarre
 *
 * =====================================================================================
 */


#include "score_addon_quarre.hpp"

#include <AppInteraction/Process/AppInteractionProcessFactory.hpp>
#include <AppInteraction/Process/Executor/AppInteractionProcessExecutor.hpp>
#include <AppInteraction/Process/Inspector/AppInteractionProcessInspector.hpp>
#include <AppInteraction/Process/LocalTree/AppInteractionProcessLocalTree.hpp>
#include <AppInteraction/Process/Layer/AppInteractionProcessLayerFactory.hpp>
#include <AppInteraction/ApplicationPlugin/AppInteractionApplicationPlugin.hpp>
#include <AppInteraction/DocumentPlugin/AppInteractionDocumentPlugin.hpp>
#include <AppInteraction/PolymorphicEntity/PolymorphicEntityFactory.hpp>
#include <AppInteraction/PolymorphicEntity/Implementation/ConcretePolymorphicEntity.hpp>
#include <AppInteraction/Panel/AppInteractionPanelDelegate.hpp>

#include <score/plugins/customfactory/FactorySetup.hpp>

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

std::vector<std::unique_ptr<score::InterfaceListBase> >
score_addon_quarre::factoryFamilies()
{
    return make_ptr_vector<score::InterfaceListBase,
            AppInteraction::PolymorphicElementFactoryList>();
}

std::vector<std::unique_ptr<score::InterfaceBase> >
score_addon_quarre::factories(
        const score::ApplicationContext& ctx,
        const score::InterfaceKey& key) const
{
    return instantiate_factories<
            score::ApplicationContext,
        FW<
           Process::ProcessModelFactory, // An abstract factory
           AppInteraction::ProcessFactory // followed by all the matching concrete factories
          >,
        FW<
           Process::LayerFactory, // Another abstract factory
           AppInteraction::LayerFactory // etc...
          >,
        FW<Process::InspectorWidgetDelegateFactory,
           AppInteraction::InspectorFactory>,
        FW<Engine::Execution::ProcessComponentFactory,
           AppInteraction::ProcessExecutorComponentFactory>,
        FW<Engine::LocalTree::ProcessComponentFactory,
           AppInteraction::LocalTreeProcessComponentFactory>,
        FW<score::PanelDelegateFactory,
           AppInteraction::PanelDelegateFactory>,
        FW<

           AppInteraction::PolymorphicElementFactory,
           AppInteraction::ConcretePolymorphicElementFactory
          >
    >(ctx, key);
}

score::GUIApplicationPlugin*
score_addon_quarre::make_applicationPlugin(
        const score::GUIApplicationContext& app)
{
    return new AppInteraction::ApplicationPlugin{app};
}

std::pair<const CommandGroupKey, CommandGeneratorMap>
score_addon_quarre::make_commands()
{
    using namespace AppInteraction;
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
