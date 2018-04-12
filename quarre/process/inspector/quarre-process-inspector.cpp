#include "quarre-process-inspector.hpp"
#include <score/document/DocumentContext.hpp>
#include <ossia/detail/logger.hpp>

using namespace score::addons;

quarre::InspectorWidget::InspectorWidget(
        quarre::ProcessModel &object,
        const score::DocumentContext &ctx, QWidget *parent) :

    InspectorWidgetDelegate_T   ( object, parent ),
    m_dispatcher                ( ctx.commandStack )
{
    auto interaction = object.interaction();
    interaction->layout()->setParent(this);
}
