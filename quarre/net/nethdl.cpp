/*
 * =====================================================================================
 *
 *       Filename:  quarre-nethdl.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05.12.2017 18:27:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>

#include "nethdl.hpp"

using namespace score::addons::quarre;

nethdl::nethdl()
{
    auto mpx = std::make_unique<ossia::net::multiplex_protocol>();
    m_device = std::make_unique<ossia::net::generic_device>(std::move(mpx), QUARRE_SERVER_NAME);
}

nethdl::~nethdl()
{

}

void nethdl::start(uint16_t wsport, uint16_t oscport)
{
    auto mpx        = dynamic_cast<multiplex_protocol*>(&m_device->get_protocol());
    auto server     = std::make_unique<ossia::oscquery::oscquery_server_protocol>(oscport, wsport);
    mpx->expose_to(std::move(server));
}

void nethdl::stop()
{

}

void nethdl::make_tree()
{
    auto root = *m_device.get();

    auto incoming = ossia::net::create_nodes(root, "/clients/device_[1-4]/interactions/next/incoming");
    for(auto& node : incoming) node->create_parameter(ossia::val_type::VEC3F);

    auto start = ossia::net::create_nodes(root, "/clients/device_[1-4]/interactions/next/start");
    for(auto& node : start) node->create_parameter(ossia::val_type::INT);

    auto cancel = ossia::net::create_nodes(root, "/clients/device_[1-4]/interactions/next/cancel");
    for(auto& node : cancel) node->create_parameter(ossia::val_type::INT);

    auto stop = ossia::net::create_nodes(root, "/clients/device_[1-4]/interactions/current/stop");
    for(auto& node : stop) node->create_parameter(ossia::val_type::INT);

    auto sensors = ossia::net::create_nodes(root, "/clients/device_[1-4]/sensors/{accelerometers,rotation}/{x,y,z}");
    for(auto& node : sensors) node->create_parameter(ossia::val_type::FLOAT);

    auto sensors_av = ossia::net::create_nodes(root, "/clients/device_[1-4]/sensors/{accelerometers,rotation}/{x,y,z}/available");
    for(auto& node : sensors_av) node->create_parameter(ossia::val_type::BOOL);

    auto gestures = ossia::net::create_nodes(root, "/clients/device_[1-4]/gestures/{whip,cover,shake}");
    for(auto& node : gestures) node->create_parameter(ossia::val_type::IMPULSE);

    auto gestures_av = ossia::net::create_nodes(root, "/clients/device_[1-4]/gestures/{whip,cover,shake}/available");
    for(auto& node : gestures_av) node->create_parameter(ossia::val_type::BOOL);

}

