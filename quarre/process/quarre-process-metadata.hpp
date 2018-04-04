#pragma once

#include <Process/ProcessMetadata.hpp>

namespace score {
namespace addons {
namespace quarre {

class ProcessModel;

}
}
}

PROCESS_METADATA(
        ,
        score::addons::quarre::ProcessModel,
        "8437437-43989",
        "quarrè",
        "quarrè",
        "control",
        {},
        Process::ProcessFlags::SupportsAll | Process::ProcessFlags::PutInNewSlot
        )
