#pragma once

#include <Process/GenericProcessFactory.hpp>
#include <Process/LayerModelPanelProxy.hpp>

#include <quarre/process/layer/quarre-process-layer-presenter.hpp>
#include <quarre/process/layer/quarre-process-layer-view.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

using LayerFactory = Process::LayerFactory_T<
    score::addons::quarre::ProcessModel,
    score::addons::quarre::ProcessLayerPresenter,
    score::addons::quarre::ProcessLayerView,
    Process::GraphicsViewLayerPanelProxy>;

}
}
}

