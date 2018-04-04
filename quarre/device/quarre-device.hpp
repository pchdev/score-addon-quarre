#pragma once

#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/ossia.hpp>
#include <Engine/Protocols/OSSIADevice.hpp>

namespace score {
namespace addons {
namespace quarre {

class QuarreDevice final : public Engine::Network::OwningOSSIADevice
{
    Q_OBJECT

    public: //------------------------------------------------
    QuarreDevice  (const Device::DeviceSettings& settings );
    ~QuarreDevice ( );

    virtual bool reconnect  ( ) override;
    virtual void recreate   ( const Device::Node& n ) override;

    void   make_tree        ( );

    signals: // ----------------------------------------------
    void sig_command    ( );
    void sig_disconnect ( );

    private slots: //-----------------------------------------
    void slot_command();

    private: //-----------------------------------------------
    uint16_t   m_wsport;
    uint16_t   m_oscport;

};
}
}
}
