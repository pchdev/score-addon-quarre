#include "quarre-protocol-factory.hpp"
#include "quarre-protocol-settings-widget.hpp"
#include "quarre-protocol-specific-settings.hpp"
#include "quarre-device.hpp"

using namespace score::addons::quarre;

QString QuarreProtocolFactory::prettyName() const
{
    return QObject::tr("quarrè-server");
}

int QuarreProtocolFactory::visualPriority() const
{
    return 1;
}

Device::DeviceInterface* QuarreProtocolFactory::makeDevice(
        const Device::DeviceSettings &settings, const DocumentContext &ctx)
{
    return new QuarreDevice { settings };
}

const Device::DeviceSettings& QuarreProtocolFactory::defaultSettings()
{
    static const Device::DeviceSettings settings = [&]() {

        Device::DeviceSettings s;
        s.protocol = concreteKey();
        s.name = "Quarrè";

        QuarreSpecificSettings qsettings;
        qsettings.max_users     = 4;
        qsettings.osc_port      = 1234;
        qsettings.ws_port       = 5678;

        s.deviceSpecificSettings= QVariant::fromValue(qsettings);

        return s;
    }();

    return settings;
}

Device::ProtocolSettingsWidget* QuarreProtocolFactory::makeSettingsWidget()
{
    return new QuarreProtocolSettingsWidget;
}

QVariant QuarreProtocolFactory::makeProtocolSpecificSettings(const VisitorVariant &visitor) const
{
    return makeProtocolSpecificSettings_T<QuarreSpecificSettings>(visitor);
}

void QuarreProtocolFactory::serializeProtocolSpecificSettings(
        const QVariant &data, const VisitorVariant &visitor) const
{
    serializeProtocolSpecificSettings_T<QuarreSpecificSettings>(data, visitor);
}

bool QuarreProtocolFactory::checkCompatibility(
        const Device::DeviceSettings &a, const Device::DeviceSettings &b) const
{
    return a.name != b.name;
}
