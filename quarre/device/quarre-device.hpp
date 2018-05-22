#pragma once

#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/ossia.hpp>
#include <Engine/Protocols/OSSIADevice.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <quarre/interaction/quarre-interaction.hpp>
#include <quarre/panel/quarre-panel-delegate.hpp>
#include <quarre/user/quarre-user.hpp>

using namespace ossia::net;

struct pdata_t
{
    std::string address;
    ossia::val_type type;
    bool critical;
};

namespace score     {
namespace addons    {
namespace quarre    {

class interaction;
class PanelDelegate;

class server : public Engine::Network::OwningOSSIADevice
{
    Q_OBJECT

    friend class dispatcher;
    friend class PanelDelegate;

    public: //---------------------------------------------------------------------------------------

    server (::Device::DeviceSettings const& settings );
    static quarre::server& instance();
    virtual bool reconnect ( ) override;
    virtual void recreate ( const Device::Node& ) override;

    parameter_base& make_parameter          ( std::string name, ossia::val_type type, bool critical );
    parameter_base& make_user_parameter     ( uint8_t index, std::string addr, ossia::val_type type, bool critical );
    parameter_base* get_common_parameter    ( std::string address );
    parameter_base* get_common_parameter    ( QString& address );
    parameter_base* get_user_parameter      ( const quarre::user& usr, std::string address );

    void on_client_connected        ( std::string const& ip_address );
    void on_client_disconnected     ( std::string const& ip_address );
    void parse_vote_result          ( );
    generic_device& get_device      ( );

    private: //--------------------------------------------------------------------------------------

    static quarre::server* m_singleton;
    void make_common_tree ( );

    quarre::user* m_user_zero;
    std::vector<quarre::user*> m_users;
    uint8_t m_n_max_users;
};
}
}
}
