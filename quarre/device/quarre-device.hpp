#pragma once

#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/ossia.hpp>
#include <Engine/Protocols/OSSIADevice.hpp>
#include <quarre/interaction/quarre-interaction.hpp>
#include <QJSEngine>

using namespace ossia::net;

using pdata_t   = std::pair<std::string,ossia::val_type>;
using parptr_t  = std::shared_ptr<parameter_base>;

namespace score     {
namespace addons    {
namespace quarre    {

class interaction;

namespace user
{
enum class status_t
{
    DISCONNECTED        = 0,
    IDLE                = 1,
    INCOMING            = 2,
    ACTIVE              = 3,
    INCOMING_ACTIVE     = 4
};

struct data
{
    uint8_t                 index;
    uint8_t                 interaction_count;
    bool                    connected;
    status_t                status;
    std::string             base_address;
    std::string             net_address;
    quarre::interaction*    incoming_interaction;
    quarre::interaction*    active_interaction;
    QJSEngine               js_engine;
};

data& instantiate_user             ( uint8_t index );
void initialize_user_node_tree     ( const quarre::user::data& u );

void update_net_address             ( quarre::user::data& u, const std::string& net_address );

void set_incoming_interaction      ( quarre::user::data& u, const quarre::interaction& i );
void set_active_interaction        ( quarre::user::data& u, const quarre::interaction& i );
void end_interaction               ( quarre::user::data& u, const quarre::interaction& i );
void pause_current_interaction     ( quarre::user::data& u, const quarre::interaction& i );
void resume_current_interaction    ( quarre::user::data& u, const quarre::interaction& i );

bool supports_input                ( const quarre::user::data& u, QString input );
void activate_input                ( const quarre::user::data& u, QString input );
void deactivate_input              ( const quarre::user::data& u, QString input );
}

namespace dispatching
{
struct candidate
{
    user::data* target;
    uint8_t priority;
};

bool dispatch_incoming_interaction      ( const quarre::server::data& server, const quarre::interaction& i );
void dispatch_active_interaction        ( const quarre::server::data& server, const quarre::interaction& i );
void dispatch_ending_interaction        ( const quarre::server::data& server, const quarre::interaction& i );
void dispatch_paused_interaction        ( const quarre::server::data& server, const quarre::interaction& i );
void dispatch_resumed_interaction       ( const quarre::server::data& server, const quarre::interaction& i );
}

namespace server
{
parameter_base& get_parameter_from_string ( std::string& address );
parameter_base& get_parameter_from_string ( QString& address );
parameter_base& get_user_parameter_from_string ( const quarre::user::data& u, std::string& address );

class data : public Engine::Network::OwningOSSIADevice
{
    Q_OBJECT

    public:
    data (::Device::DeviceSettings const& settings );
    virtual bool reconnect ( ) override;
    virtual void recreate ( ) override;

    user::data* user_zero;
    std::vector<user::data*> users;
    uint8_t n_max_users;
};

data* g_server;

void make_server_common_tree ( quarre::server::data& server );
generic_device& get_device   ( quarre::server::data const& server );

void on_client_connected        ( std::string const& ip_address );
void on_client_disconnected     ( std::string const& ip_address );

}
}
}
}
