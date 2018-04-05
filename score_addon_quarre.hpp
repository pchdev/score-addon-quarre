#pragma once

#include <score/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>
#include <score/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <score/plugins/qt_interfaces/FactoryFamily_QtInterface.hpp>
#include <score/plugins/qt_interfaces/GUIApplicationPlugin_QtInterface.hpp>
#include <score/plugins/qt_interfaces/CommandFactory_QtInterface.hpp>

#include <score/application/ApplicationContext.hpp>
#include <score/command/CommandGeneratorMap.hpp>
#include <score/command/Command.hpp>
#include <score/plugins/customfactory/FactoryFamily.hpp>

#include <QObject>
#include <utility>
#include <vector>

class score_addon_quarre final : public QObject,
	public score::Plugin_QtInterface,
    public score::FactoryInterface_QtInterface,
	public score::FactoryList_QtInterface,
	public score::ApplicationPlugin_QtInterface,
	public score::CommandFactory_QtInterface
{
	Q_OBJECT
    Q_PLUGIN_METADATA       ( IID FactoryInterface_QtInterface_iid )
    SCORE_PLUGIN_METADATA   ( 1, "2DFA6B7D-6A3A-4B43-A58B-33A302F49EA4" )

    Q_INTERFACES (
            score::Plugin_QtInterface
			score::FactoryInterface_QtInterface
			score::FactoryList_QtInterface
			score::ApplicationPlugin_QtInterface
			score::CommandFactory_QtInterface
			)

        public: // --------------------------------------------------------
		score_addon_quarre();
		virtual ~score_addon_quarre();

        private: //--------------------------------------------------------

        virtual std::vector<score::PluginKey> required() const override;

        virtual void updateSaveFile(
				QJsonObject& obj,
				score::Version obj_version,
				score::Version current_version) override;

        virtual std::vector<std::unique_ptr<score::InterfaceListBase>> factoryFamilies() override;

        virtual std::vector<std::unique_ptr<score::InterfaceBase>> factories(
				const score::ApplicationContext& ctx,
				const score::InterfaceKey& key) const override;
		
        virtual score::GUIApplicationPlugin* make_guiApplicationPlugin(
				const score::GUIApplicationContext& app) override;

        virtual std::pair<const CommandGroupKey, CommandGeneratorMap> make_commands() override;

};

