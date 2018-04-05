#pragma once

#include <Device/Protocol/ProtocolFactoryInterface.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class ProtocolFactory final : public Device::ProtocolFactory
{
    SCORE_CONCRETE  ( "BBBF1836-E035-409F-8C2E-879CF480511D" )

    virtual QString prettyName() const override;
    virtual int visualPriority() const override;

    virtual Device::DeviceInterface* makeDevice(
            const Device::DeviceSettings &settings,
            const DocumentContext &ctx) override;

    virtual const Device::DeviceSettings& defaultSettings() const override;

    virtual Device::ProtocolSettingsWidget* makeSettingsWidget() override;

    virtual QVariant makeProtocolSpecificSettings(const VisitorVariant &visitor) const override;

    virtual void serializeProtocolSpecificSettings(
            const QVariant &data, const VisitorVariant &visitor) const override;

    virtual bool checkCompatibility(
            const Device::DeviceSettings &a,
            const Device::DeviceSettings &b) const override;

};

}
}
}
