#include "quarre-process-layer-view.hpp"

#include <Process/Style/ScenarioStyle.hpp>

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QQuickWidget>
#include <QGraphicsProxyWidget>
#include <QApplication>

using namespace score::addons;

quarre::ProcessLayerView::ProcessLayerView(QGraphicsItem *parent) : LayerView(parent)
{

}

void quarre::ProcessLayerView::setText(const QString & txt)
{
    m_text = txt;
    update();
}

void quarre::ProcessLayerView::paint_impl(QPainter* painter) const
{

}

void quarre::ProcessLayerView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
}

void quarre::ProcessLayerView::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    doubleClicked();
    event->accept();
}

