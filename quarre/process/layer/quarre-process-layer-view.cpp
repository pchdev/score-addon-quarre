#include "quarre-process-layer-view.hpp"

#include <Process/Style/ScenarioStyle.hpp>

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QQuickWidget>
#include <QGraphicsProxyWidget>
#include <QApplication>

using namespace score::addons;

quarre::ProcessLayerView::ProcessLayerView(QGraphicsItem *parent) :
    LayerView(parent), m_text("quarrè-interaction")
{

}

void quarre::ProcessLayerView::setText(const QString & txt)
{
    m_text = txt;
    update();
}

void quarre::ProcessLayerView::paint_impl(QPainter* painter) const
{
    // display a summary of the interaction here
    auto font = score::Skin::instance().SansFont;
      font.setPointSize(14);
      painter->setFont(font);
      painter->setPen(Qt::white);

  painter->drawText(QPointF{5., 30.}, m_text);

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

