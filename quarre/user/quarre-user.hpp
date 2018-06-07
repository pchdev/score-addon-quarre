#pragma once

#include <quarre/device/quarre-device.hpp>
#include <quarre/user/quarre-modules.hpp>

#include <QJSEngine>
#include <QMutex>

using namespace ossia::net;

namespace score     {
namespace addons    {
namespace quarre    {

class dispatcher;

enum class user_status
{
    DISCONNECTED        = 0,
    IDLE                = 1,
    INCOMING            = 2,
    ACTIVE              = 3,
    INCOMING_ACTIVE     = 4
};

class interaction;
class PanelDelegate;
class server;

class user
{
    friend class quarre::PanelDelegate;
    friend class quarre::dispatcher;
    friend class quarre::server;

    public: //-------------------------------------------------------------
    user (uint8_t index, quarre::server& server);

    void update_net_address            ( const std::string& net_address );
    uint16_t get_active_countdown      ( );

    void set_incoming_interaction      ( quarre::interaction& i );
    void set_active_interaction        ( quarre::interaction& i, const Device::DeviceList& devlist );
    void end_interaction               ( quarre::interaction& i );
    void cancel_incoming_interaction   ( quarre::interaction& i );
    void pause_current_interaction     ( quarre::interaction& i );
    void resume_current_interaction    ( quarre::interaction& i );
    bool supports_interaction          ( quarre::interaction& i );

    protected: //----------------------------------------------------------
    parameter_base& get_parameter           ( std::string path );
    parameter_base& get_input_parameter     ( QString path );
    void sanitize_input_name                ( QString &input_name );
    void clear_input                        ( QString path );

    uint8_t                 m_index;
    uint8_t                 m_interaction_count;
    bool                    m_connected;
    user_status             m_status;
    std::string             m_base_address;
    std::string             m_net_address;
    quarre::interaction*    m_incoming_interaction;
    quarre::interaction*    m_active_interaction;
    quarre::server&         m_server;
    QJSEngine               m_js_engine;
    QMutex                  m_mtx;
};
}
}
}
