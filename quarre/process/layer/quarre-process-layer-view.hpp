#pragma once

#include <Process/LayerView.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class ProcessLayerView final : public Process::LayerView
{
    Q_OBJECT

    public: //---------------------------------------------------------------------------

    explicit ProcessLayerView(QGraphicsItem* parent);
    void setText    ( const QString& );

    signals:
    void doubleClicked();

    private: //--------------------------------------------------------------------------

    virtual void paint_impl                 ( QPainter * ) const override;
    virtual void mousePressEvent            ( QGraphicsSceneMouseEvent *event ) override;
    virtual void mouseDoubleClickEvent      ( QGraphicsSceneMouseEvent *event ) override;

    QString m_text;
};

}
}
}
