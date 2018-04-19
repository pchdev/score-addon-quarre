#pragma once

#include <Process/GenericProcessFactory.hpp>
#include <Process/LayerModelPanelProxy.hpp>
#include <Process/WidgetLayer/WidgetProcessFactory.hpp>

#include <quarre/process/layer/quarre-process-layer-presenter.hpp>
#include <quarre/process/layer/quarre-process-layer-view.hpp>
#include <quarre/process/inspector/quarre-process-inspector.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

using LayerFactory = Process::GenericLayerFactory<
    score::addons::quarre::ProcessModel,
    score::addons::quarre::ProcessLayerPresenter,
    score::addons::quarre::ProcessLayerView,
    Process::GraphicsViewLayerPanelProxy>;

}
}
}

