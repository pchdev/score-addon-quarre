#pragma once

#include <Device/Protocol/ProtocolSettingsWidget.hpp>
#include <Device/Protocol/DeviceSettings.hpp>

class QLineEdit;
class QSpinBox;
class QWidget;

namespace score     {
namespace addons    {
namespace quarre    {

class ProtocolSettingsWidget final : public Device::ProtocolSettingsWidget
{
    public: //----------------------------------------------------

    ProtocolSettingsWidget(QWidget* parent = nullptr);

    virtual Device::DeviceSettings getSettings() const override;
    virtual void setSettings(const Device::DeviceSettings &settings) override;

    protected:
    void setDefaults();

    protected: // ------------------------------------------------
    QSpinBox* m_osc_port_sbox;
    QSpinBox* m_ws_port_sbox;
    QSpinBox* m_max_users_sbox;


};

}
}
}
