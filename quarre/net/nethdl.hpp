/*
 * =====================================================================================
 *
 *       Filename:  quarre-nethdl.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05.12.2017 18:26:48
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#pragma once

#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/ossia.hpp>

namespace score {
namespace addons {
namespace quarre {

typedef std::unique_ptr<ossia::net::generic_device> unique_device;
typedef std::unique_ptr<ossia::oscquery::oscquery_server_protocol> unique_oscquery;

class nethdl
{

public:
    nethdl();
    ~nethdl();

    void            start(uint16_t wsport, uint16_t oscport);
    void            make_tree();
    void            stop();

private:
    unique_device       m_device;
    uint16_t            m_wsport;
    uint16_t            m_oscport;
    uint8_t             m_n_client;




};
}
}
}
