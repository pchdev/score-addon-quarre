#include <stdlib.h>
#include "quarre-device.hpp"

using namespace score::addons;
using namespace ossia::net;
using namespace ossia::oscquery;

quarre::Device::Device(const Device::DeviceSettings &settings) :
    Engine::Network::OwningOSSIADevice(settings)
{
    auto mpx = std::make_unique<multiplex_protocol>( );
    m_dev = std::make_unique<device_base>( std::move(mpx), QUARRE_SERVER_NAME );

    auto server     = std::make_unique<oscquery_server_protocol>( osc_port, ws_port );
    mpx->expose_to  ( std::move(server) );

    make_tree();
}

quarre::Device::~Device() {}

inline void make_common_parameter(generic_device& root, ossia::string_view name, ossia::val_type ty)
{
    auto node = ossia::net::find_or_create_node(root, name, true);
    node->create_parameter(ty);
}

inline void make_user_parameter(generic_device& root, std::string pattern, ossia::val_type ty)
{
    std::string base_pattern = "/clients/[1-4]";
    auto nodes  = create_nodes(root, base_pattern + pattern);
    for ( auto& node : nodes )
        node->create_parameter(ty);
}

bool quarre::Device::reconnect(const Device::Node& n)
{

}

void quarre::Device::recreate(const Device::Node &n)
{

}

void quarre::Device::make_tree()
{
    auto root   = *m_device;
    std::vector<node_base*> nodes;
    node_base*  nd;

    // COMMON --------------------------------------------------------------------------------

    make_common_parameter   ( root, "/scenario/start",  ossia::val_type::IMPULSE );
    make_common_parameter   ( root, "/scenario/end",    ossia::val_type::IMPULSE );
    make_common_parameter   ( root, "/scenario/pause",  ossia::val_type::IMPULSE );
    make_common_parameter   ( root, "/scenario/stop",   ossia::val_type::IMPULSE );
    make_common_parameter   ( root, "/scenario/reset",  ossia::val_type::IMPULSE );

    make_common_parameter   ( root, "/scenario/name",        ossia::val_type::STRING );
    make_common_parameter   ( root, "/scenario/scene/name",  ossia::val_type::STRING );

    // ID --------------------------------------------------------------------------------

    make_user_parameter     ( root, "/connected", ossia::val_type::BOOL );
    make_user_parameter     ( root, "/address", ossia::val_type::STRING );

    // INTERACTIONS ----------------------------------------------------------------------

    make_user_parameter     ( root, "/interactions/next/incoming", ossia::val_type::LIST );
    make_user_parameter     ( root, "/interactions/next/begin", ossia::val_type::LIST );
    make_user_parameter     ( root, "/interactions/next/cancel", ossia::val_type::INT );
    make_user_parameter     ( root, "/interactions/current/end", ossia::val_type::INT );
    make_user_parameter     ( root, "/interactions/current/force", ossia::val_type::LIST );
    make_user_parameter     ( root, "/interactions/reset", ossia::val_type::IMPULSE );

    // SENSORS --------------------------------------------------------------------------

    make_user_parameter     ( root, "/sensors/{accelerometers,rotation}/data/{x,y,z}", ossia::val_type::FLOAT );
    make_user_parameter     ( root, "/sensors/{accelerometers,rotation,proximity}/{available,active}", ossia::val_type::BOOL );
    make_user_parameter     ( root, "/sensors/proximity/data/close", ossia::val_type::BOOL );

    // GESTURES --------------------------------------------------------------------------

    make_user_parameter     ( root, "/gestures/{whip,cover,turnover,freefall,twist,twist/left,twist/right,shake,shake/left,shake/right,shake/up,shake/down,pickup,swipe}/{active,available}", ossia::val_type::BOOL);
    make_user_parameter     ( root, "/gestures/{whip,cover,turnover,freefall,twist,twist/left,twist/right,shake,shake/left,shake/right,shake/up,shake/down,pickup,swipe}/trigger", ossia::val_type::IMPULSE );


}

