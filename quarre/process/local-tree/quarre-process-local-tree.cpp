#include "quarre-process-local-tree.hpp"
#include <quarre/process/quarre-process-model.hpp>

using namespace score::addons;

quarre::LocalTreeProcessComponent::LocalTreeProcessComponent(
        const Id<score::Component> &id,
        ossia::net::node_base &parent,
        quarre::ProcessModel &scenario,
        Engine::LocalTree::DocumentPlugin &doc,
        QObject *parent ) :

    Engine::LocalTree::ProcessComponent_T<quarre::ProcessModel>(
        parent, scenario, doc, id, "quarrè-component", parent)
{

}

quarre::LocalTreeProcessComponent::~LocalTreeProcessComponent()
{

}
