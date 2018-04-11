#include "quarre-protocol-settings-widget.hpp"
#include "quarre-protocol-specific-settings.hpp"


#include <QSpinBox>
#include <QLabel>
#include <QFormLayout>

using namespace score::addons;

quarre::ProtocolSettingsWidget::ProtocolSettingsWidget(QWidget *parent)
{
    // OSCQUERY-OSC-PORT --------------------------
    m_osc_port_sbox = new QSpinBox(this);
    m_osc_port_sbox ->setRange(0, 65535);

    // OSCQUERY-WS-PORT --------------------------
    m_ws_port_sbox = new QSpinBox(this);
    m_ws_port_sbox ->setRange(0, 65535);

    // N_CLIENTS_MAX ----------------------------
    m_max_users_sbox = new QSpinBox(this);
    m_max_users_sbox ->setRange(1, 50);

    QFormLayout* layout = new QFormLayout;

    layout->addRow ( tr("Server OSC Port"), m_osc_port_sbox );
    layout->addRow ( tr("Server WS Port"), m_ws_port_sbox );
    layout->addRow ( tr("Max Clients"), m_max_users_sbox );

    setLayout(layout);
    setDefaults();
}

Device::DeviceSettings quarre::ProtocolSettingsWidget::getSettings() const
{
    Device::DeviceSettings settings;
    settings.name = "quarrè-server";

    quarre::SpecificSettings qsettings;

    qsettings.osc_port      = m_osc_port_sbox->value();
    qsettings.ws_port       = m_ws_port_sbox->value();
    qsettings.max_users     = m_max_users_sbox->value();

    settings.deviceSpecificSettings = QVariant::fromValue(qsettings);

    return settings;
}

void quarre::ProtocolSettingsWidget::setSettings(const Device::DeviceSettings &settings)
{
    quarre::SpecificSettings qsettings;

    if ( settings.deviceSpecificSettings
         .canConvert<quarre::SpecificSettings>() )
    {
        qsettings = settings.deviceSpecificSettings.value<quarre::SpecificSettings>();

        m_osc_port_sbox     -> setValue   ( qsettings.osc_port );
        m_ws_port_sbox      -> setValue   ( qsettings.ws_port );
        m_max_users_sbox    -> setValue   ( qsettings.max_users );
    }
}

void quarre::ProtocolSettingsWidget::setDefaults()
{
    SCORE_ASSERT    ( m_osc_port_sbox );
    SCORE_ASSERT    ( m_ws_port_sbox );
    SCORE_ASSERT    ( m_max_users_sbox );

    m_osc_port_sbox     -> setValue ( 1234 );
    m_ws_port_sbox      -> setValue ( 5678 );
    m_max_users_sbox    -> setValue ( 4 );
}
