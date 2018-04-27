#pragma once

#include <Engine/LocalTree/Scenario/ProcessComponent.hpp>
#include <Engine/LocalTree/LocalTreeComponent.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class ProcessModel;

class LocalTreeProcessComponent : public Engine::LocalTree::ProcessComponent_T<ProcessModel>
{
    COMPONENT_METADATA  ( "5F8208E0-C568-4128-8711-805872A6E009" )

    public: // ----------------------------------------------------
        LocalTreeProcessComponent(
            const Id<score::Component>& id,
            ossia::net::node_base& parent_node,
            score::addons::quarre::ProcessModel& scenario,
            Engine::LocalTree::DocumentPlugin& doc,
            QObject* parent );

    ~LocalTreeProcessComponent();

    private: // ---------------------------------------------------
    std::unique_ptr<Engine::LocalTree::BaseProperty> m_property;

};

using LocalTreeProcessComponentFactory = Engine::LocalTree::ProcessComponentFactory_T<LocalTreeProcessComponent>;

}
}
}
