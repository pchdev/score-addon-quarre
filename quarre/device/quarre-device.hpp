#pragma once

#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/ossia.hpp>
#include <Engine/Protocols/OSSIADevice.hpp>
#include <quarre/interaction/quarre-interaction.hpp>
#include <quarre/panel/quarre-panel-delegate.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>

#include <QJSEngine>

using namespace ossia::net;

using pdata_t   = std::pair<std::string,ossia::val_type>;
using parptr_t  = std::shared_ptr<parameter_base>;

namespace score     {
namespace addons    {
namespace quarre    {

class interaction;
class user;
class PanelDelegate;

class dispatcher
{
    public:
    static bool dispatch_incoming_interaction   ( quarre::interaction& i );
    static void dispatch_active_interaction     ( quarre::interaction& i , const Device::DeviceList &devlist );
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

class server : public Engine::Network::OwningOSSIADevice
{
    Q_OBJECT

    friend class dispatcher;
    friend class PanelDelegate;

    public:
    server (::Device::DeviceSettings const& settings );
    static quarre::server& instance();
    virtual bool reconnect ( ) override;
    virtual void recreate ( const Device::Node& ) override;

    parameter_base& get_parameter_from_string       ( std::string& address );
    parameter_base& get_parameter_from_string       ( QString& address );
    parameter_base& get_user_parameter_from_string  ( const quarre::user& usr, std::string address );

    void on_client_connected        ( std::string const& ip_address );
    void on_client_disconnected     ( std::string const& ip_address );
    generic_device& get_device      ( );

    private:
    static quarre::server* m_singleton;
    void make_common_tree ( );

    quarre::user* m_user_zero;
    std::vector<quarre::user*> m_users;
    uint8_t m_n_max_users;
};

enum class user_status
{
    DISCONNECTED        = 0,
    IDLE                = 1,
    INCOMING            = 2,
    ACTIVE              = 3,
    INCOMING_ACTIVE     = 4
};

class user
{
    friend class quarre::PanelDelegate;
    friend class quarre::dispatcher;
    friend class quarre::server;

    public: //-------------------------------------------------------------
    user (uint8_t index, quarre::server& server);

    void update_net_address             ( const std::string& net_address );
    uint8_t get_active_countdown        ( ) const;

    void set_incoming_interaction      ( quarre::interaction& i );
    void set_active_interaction        ( quarre::interaction& i, const Device::DeviceList& devlist );
    void end_interaction               ( quarre::interaction& i );
    void pause_current_interaction     ( quarre::interaction& i );
    void resume_current_interaction    ( quarre::interaction& i );

    bool supports_input                ( QString input );
    void activate_input                ( QString input );
    void deactivate_input              ( QString input );

    protected: //----------------------------------------------------------
    void replace_user_wildcard              ( QString& target );
    void get_input_base_address             ( QString& target );
    void sanitize_input_name                ( QString &input_name );
    parameter_base& get_input_parameter     ( QString input, QString replacement );

    void make_user_tree     ( );

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
};

}
}
}
