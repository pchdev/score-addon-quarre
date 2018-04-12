#include "quarre-panel-delegate-factory.hpp"
#include "quarre-panel-delegate.hpp"
#include <quarre/device/quarre-device.hpp>

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>

using namespace score::addons;

quarre::PanelDelegate::PanelDelegate(const score::GUIApplicationContext &ctx) :
    score::PanelDelegate(ctx)
{
    m_widget = new QWidget;
    auto lay = new QFormLayout ( m_widget );

    auto dev = quarre::quarre_device::instance();
    auto max_u = dev->max_users();

    for ( int i = 0; i < max_u; ++i )
    {
        QString tr = "user ";
        tr += i;
        tr += " :";

        auto qle = new QLineEdit("disconnected");
        lay->addRow(tr, qle);
    }

    auto& server = quarre::quarre_device::instance()->device();
    auto proto = dynamic_cast<ossia::oscquery::oscquery_server_protocol*>(&server.get_protocol());

    // a qlineedit for each user with ip address
    // or 'disconnected'
    // to be completed with controls to test interactions quickly

}

void quarre::PanelDelegate::on_client_connected(const std::string &ip)
{

}

void quarre::PanelDelegate::on_client_disconnected(const std::string &ip)
{

}

QWidget* quarre::PanelDelegate::widget() { return m_widget; }

const score::PanelStatus& quarre::PanelDelegate::defaultPanelStatus() const
{
    static const score::PanelStatus status
    {
        true,
        Qt::LeftDockWidgetArea,
        10,
        QObject::tr("quarrè"),
        QObject::tr("Ctrl+W")
    };

    return status;
}

void quarre::PanelDelegate::on_modelChanged(score::MaybeDocument oldm, score::MaybeDocument newm)
{

}

void quarre::PanelDelegate::setNewSelection(const Selection &s)
{

}

// FACTORIES -------------------------------------------------------------------------------------

std::unique_ptr<score::PanelDelegate> quarre::PanelDelegateFactory::make(
        const GUIApplicationContext &ctx)
{
    return std::make_unique<quarre::PanelDelegate>(ctx);
}


