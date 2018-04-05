#pragma once

#include <Process/Focus/FocusDispatcher.hpp>
#include <Process/LayerPresenter.hpp>
#include <Process/ZoomHelper.hpp>

#include <score/model/Identifier.hpp>

#include <quarre/process/quarre-process-model.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class ProcessLayerPresenter final : public Process::LayerPresenter
{
    public: //-----------------------------------------------------

    explicit ProcessLayerPresenter (
            const score::addons::quarre::ProcessModel& model,
            const score::addons::quarre::ProcessLayerView* view,
            const Process::ProcessPresenterContext& ctx,
            QObject* parent );

    virtual void setWidth       ( qreal width ) override;
    virtual void setHeight      ( qreal height ) override;
    virtual void putToFront     ( ) override;
    virtual void putBehind      ( ) override;

    virtual void on_zoomRatioChanged        ( ZoomRatio ) override;
    virtual void parentGeometryChanged      ( ) override;

    virtual const Process::ProcessModel& model          ( ) const override;
    virtual const Id<Process::ProcessModel>& modelId    ( ) const override;

    private: //----------------------------------------------------

    void on_doubleClicked();
    const ProcessModel& m_layer;
    quarre::ProcessLayerView* m_view {};

};


}
}
}


