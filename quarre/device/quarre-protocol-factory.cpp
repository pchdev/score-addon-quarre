#include "quarre-protocol-factory.hpp"
#include "quarre-protocol-settings-widget.hpp"
#include "quarre-protocol-specific-settings.hpp"
#include "quarre-device.hpp"
#include <Device/Protocol/DeviceSettings.hpp>

using namespace score::addons;

QString quarre::ProtocolFactory::prettyName() const
{
    return QObject::tr ( "quarre-server" );
}

int quarre::ProtocolFactory::visualPriority() const
{
    return 1;
}

Device::DeviceInterface* quarre::ProtocolFactory::makeDevice(
        const Device::DeviceSettings &settings, const DocumentContext &ctx)
{
    return quarre::quarre_device::instance(settings);
}

const Device::DeviceSettings& quarre::ProtocolFactory::defaultSettings() const
{
    static const Device::DeviceSettings settings = [&]() {

        Device::DeviceSettings s;
        s.protocol = concreteKey();
        s.name = "quarre-server";

        quarre::SpecificSettings qsettings;
        qsettings.max_users     = 4;
        qsettings.osc_port      = 1234;
        qsettings.ws_port       = 5678;

        s.deviceSpecificSettings= QVariant::fromValue(qsettings);

        return s;
    }();

    return settings;
}

Device::ProtocolSettingsWidget* quarre::ProtocolFactory::makeSettingsWidget()
{
    return new quarre::ProtocolSettingsWidget;
}

QVariant quarre::ProtocolFactory::makeProtocolSpecificSettings(const VisitorVariant &visitor) const
{
    return makeProtocolSpecificSettings_T<quarre::SpecificSettings>(visitor);
}

void quarre::ProtocolFactory::serializeProtocolSpecificSettings(
        const QVariant &data, const VisitorVariant &visitor) const
{
    serializeProtocolSpecificSettings_T<quarre::SpecificSettings>(data, visitor);
}

bool quarre::ProtocolFactory::checkCompatibility(
        const Device::DeviceSettings &a, const Device::DeviceSettings &b) const
{
    return a.name != b.name;
}
