#pragma once

#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/ossia.hpp>
#include <Engine/Protocols/OSSIADevice.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class Device final : public Engine::Network::OwningOSSIADevice
{
    Q_OBJECT

    public: //------------------------------------------------
    static score::addons::quarre::Device* instance();
    static score::addons::quarre::Device* instance(const Device::DeviceSettings& settings);

    ~Device ( );

    virtual bool reconnect  ( ) override;
    virtual void recreate   ( const Device::Node& n ) override;

    void   make_tree        ( );

    signals: // ----------------------------------------------
    void sig_command    ( );
    void sig_disconnect ( );

    private slots: //-----------------------------------------
    void slot_command();

    private: //-----------------------------------------------
    Device     (const Device::DeviceSettings& settings );
    Device*    m_singleton;
    uint16_t   m_wsport;
    uint16_t   m_oscport;

};
}
}
}
