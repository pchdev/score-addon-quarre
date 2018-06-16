#pragma once

#include <vector>
#include <QString>
#include <string>

namespace score     {
namespace addons    {
namespace quarre    {

struct module
{
    QString name;
    std::string address;
};

static const std::vector<score::addons::quarre::module> g_modules =
{
    { "Vote" },
    { "Trajectories" },
    { "XYTouch" },
    { "Birds" },
    { "XRotation", "/sensors/rotation" },
    { "YRotation", "/sensors/rotation" },
    { "ZRotation", "/sensors/rotation" },
    { "XYZRotation", "/sensors/rotation" },
    { "XAccel", "/sensors/accelerometers" },
    { "YAccel", "/sensors/accelerometers" },
    { "ZAccel", "/sensors/accelerometers" },
    { "XYZAccel", "/sensors/accelerometers" },
    { "Proximity", "/sensors/proximity" },
    { "GestureHammer", "/gestures/whip" },
    { "GesturePalm", "/gestures/cover"  },
    { "GestureShake", "/gestures/shake" },
    { "GestureTurnover", "/gestures/turnover" },
    { "GestureBlow" },
    { "Insects" }, // todo: requirements for this module
    { "VareResonator" },
    { "VareBody" },
    { "VareGranular" },
    { "VareRainbells" },
    { "VarePercs" },
    { "Transitions" },
    { "MarkhorPads" },
    { "MarkhorResonator" },
    { "MarkhorBody" },
    { "MarkhorGranular" },
    { "Strings" },
    { "Strings2" },
    { "AmmonReverb" },
    { "JomonArpeggiator"},
    { "JomonRhythm" },
    { "JomonReverb" },
    { "JomonPalm" },
    { "Tutorial" },
    { "WPN214" },
    { "AmmonBells" },
    { "JomonPalmZ" }
};

}
}
}
