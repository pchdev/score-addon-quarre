#pragma once

#include "quarre-process-model.hpp"
#include <Process/GenericProcessFactory.hpp>

namespace score {
namespace addons {
namespace quarre {

using ProcessFactory = Process::ProcessFactory_T<quarre::ProcessModel>;

}
}
}
