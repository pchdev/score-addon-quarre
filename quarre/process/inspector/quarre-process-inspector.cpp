#include "quarre-process-inspector.hpp"

#include <score/document/DocumentContext.hpp>
#include <ossia/detail/logger.hpp>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>

using namespace score::addons;

quarre::InspectorWidget::InspectorWidget(
        const quarre::ProcessModel &object,
        const score::DocumentContext &ctx, QWidget *parent) :
    InspectorWidgetDelegate_T(object, parent),
    m_dispatcher(ctx.commandStack)
{

    auto lay = new QFormLayout(this);
}

void quarre::InspectorWidget::on_entityAdded(const PolymorphicEntity &)
{

}

void quarre::InspectorWidget::on_entityRemoved(const PolymorphicEntity &)
{

}
