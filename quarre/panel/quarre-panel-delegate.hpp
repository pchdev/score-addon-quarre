#pragma once

#include <score/plugins/panel/PanelDelegate.hpp>
#include <score/plugins/panel/PanelDelegateFactory.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class PanelDelegate final : public QObject, public score::PanelDelegate
{
    public: // ------------------------------------------------

    PanelDelegate( const score::GUIApplicationContext& ctx );

    void on_client_connected(std::string const& ip);
    void on_client_disconnected(std::string const& ip);

    private: //-----------------------------------------------

    virtual QWidget* widget() override;
    virtual const score::PanelStatus& defaultPanelStatus() const override;

    virtual void on_modelChanged(
            score::MaybeDocument oldm,
            score::MaybeDocument newm ) override;

    virtual void setNewSelection(const Selection& s) override;

    QWidget* m_widget;

};


}
}
}
