#pragma once

#include <score/plugins/panel/PanelDelegate.hpp>
#include <score/plugins/panel/PanelDelegateFactory.hpp>
#include <quarre/device/quarre-device.hpp>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>

namespace score     {
namespace addons    {
namespace quarre    {

class PanelDelegate final : public QObject, public score::PanelDelegate
{
    public: // ------------------------------------------------

    PanelDelegate( const score::GUIApplicationContext& ctx );

    static PanelDelegate* instance  ( );

    void on_user_changed            (const user &user );
    void on_server_instantiated     ( const quarre::quarre_device &device );

    private: //-----------------------------------------------
    static PanelDelegate* m_singleton;

    virtual QWidget* widget() override;
    virtual const score::PanelStatus& defaultPanelStatus() const override;

    virtual void on_modelChanged(
            score::MaybeDocument oldm,
            score::MaybeDocument newm ) override;

    virtual void setNewSelection(const Selection& s) override;

    QWidget* m_widget;
    QFormLayout* m_layout;
    QVector<QLabel*> m_user_connection_displays;

};


}
}
}
