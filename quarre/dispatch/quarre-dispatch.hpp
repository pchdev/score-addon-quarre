#pragma once
#include <quarre/user/quarre-user.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class dispatcher
{
    public:
    static bool dispatch_incoming_interaction   ( quarre::interaction& i );
    static void dispatch_active_interaction     ( quarre::interaction& i, const Device::DeviceList &devlist );
    static void dispatch_ending_interaction     ( quarre::interaction& i );
    static void dispatch_paused_interaction     ( quarre::interaction& i );
    static void dispatch_resumed_interaction    ( quarre::interaction& i );

    private:
    struct candidate
    {
        quarre::user* target;
        uint8_t priority;
    };
};

}
}
}
