#include "quarre-panel-delegate-factory.hpp"
#include "quarre-panel-delegate.hpp"

using namespace score::addons;
quarre::PanelDelegate* quarre::PanelDelegate::m_singleton = 0;

quarre::PanelDelegate::PanelDelegate(const score::GUIApplicationContext &ctx) :
    score::PanelDelegate(ctx),
    m_widget(new QWidget),
    m_layout(new QFormLayout)
{
    m_singleton = this;
    if ( auto device = quarre::quarre_device::instance() )
        on_server_instantiated(*device);
}

quarre::PanelDelegate* quarre::PanelDelegate::instance()
{
    return m_singleton;
}

void quarre::PanelDelegate::on_user_changed(const quarre::user& user)
{
    auto target_label = m_user_connection_displays[user.index()-1];
    target_label->setText(QString::fromStdString(user.address()));
}

void quarre::PanelDelegate::on_server_instantiated(const quarre::quarre_device &device)
{
    m_layout = new QFormLayout ( m_widget );
    auto max_u = device.max_users();

    for ( int i = 0; i < max_u; ++i )
    {
        QString tr = "user ";
        tr += QString::number(i+1);
        tr += " :";

        auto qle = new QLabel("disconnected");
        m_layout->addRow ( tr, qle );
        m_user_connection_displays.push_back(qle);
    }
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


