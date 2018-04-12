#include "quarre-process-layer-presenter.hpp"
#include "quarre-process-layer-view.hpp"

#include <score/command/Dispatchers/CommandDispatcher.hpp>

using namespace score::addons;

// controller: establishes connections between view and model

quarre::ProcessLayerPresenter::ProcessLayerPresenter (const quarre::ProcessModel &layer,
        ProcessLayerView *view,
        const Process::ProcessPresenterContext &ctx,
        QObject *parent ) :

    LayerPresenter      ( ctx, parent ),
    m_layer             ( layer ),
    m_view              ( view )
{

    const quarre::ProcessModel& p = layer;

    // no connections to be made, as all is basically done with the inspector for now

}

void quarre::ProcessLayerPresenter::setWidth(qreal width)
{
    m_view->setWidth(width);
}

void quarre::ProcessLayerPresenter::setHeight(qreal height)
{
    m_view->setHeight(height);
}

void quarre::ProcessLayerPresenter::putToFront()
{
    m_view->setOpacity(1);
}

void quarre::ProcessLayerPresenter::putBehind()
{
    m_view->setOpacity(0.2);
}

void quarre::ProcessLayerPresenter::on_zoomRatioChanged(ZoomRatio)
{

}

void quarre::ProcessLayerPresenter::parentGeometryChanged()
{

}

const Process::ProcessModel& quarre::ProcessLayerPresenter::model() const
{
    return m_layer;
}

const Id<Process::ProcessModel>& quarre::ProcessLayerPresenter::modelId() const
{
    return m_layer.id();
}

void quarre::ProcessLayerPresenter::on_doubleClicked()
{

}
