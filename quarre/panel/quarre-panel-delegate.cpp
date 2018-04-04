#include "quarre-panel-delegate-factory.hpp"
#include "quarre-panel-delegate.hpp"

#include <QFormLayout>
#include <QLabel>

using namespace score::addons;

quarre::PanelDelegate::PanelDelegate(const score::GUIApplicationContext &ctx) :
    score::PanelDelegate(ctx)
{
    m_widget = new QWidget;
    auto lay = new QFormLayout ( m_widget );

    lay->addWidget(new QLabel(tr("This is a panel.")));

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


