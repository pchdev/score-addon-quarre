#include "quarre-process-local-tree.hpp"
#include <quarre/process/quarre-process-model.hpp>

using namespace score::addons;

quarre::LocalTreeProcessComponent::LocalTreeProcessComponent(const Id<score::Component> &id,
        ossia::net::node_base &parent_node,
        quarre::ProcessModel &scenario,
        Engine::LocalTree::DocumentPlugin &doc,
        QObject *parent ) :

    Engine::LocalTree::ProcessComponent_T<quarre::ProcessModel>(
        parent_node, scenario, doc, id, "quarrè-component", parent)
{
    auto n_dispatched = parent_node.create_child("dispatched");
    n_dispatched->create_parameter(ossia::val_type::BOOL);
}

quarre::LocalTreeProcessComponent::~LocalTreeProcessComponent()
{

}
