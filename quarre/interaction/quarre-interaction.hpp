#pragma once

#include <quarre/mapping/quarre-mapping.hpp>
#include <quarre/device/quarre-device.hpp>
#include <score/model/IdentifiedObject.hpp>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSpinBox>

namespace score     {
namespace addons    {
namespace quarre    {

class user;
class mapping;

class interaction final : public IdentifiedObject<interaction>
{
    Q_OBJECT
    SCORE_SERIALIZE_FRIENDS

    Q_PROPERTY  ( QString module READ module WRITE set_module NOTIFY moduleChanged )
    Q_PROPERTY  ( QString title READ title WRITE set_title NOTIFY titleChanged )
    Q_PROPERTY  ( QString description READ description WRITE set_description NOTIFY descriptionChanged )
    Q_PROPERTY  ( int length READ length WRITE set_length NOTIFY lengthChanged )
    Q_PROPERTY  ( int countdown READ countdown WRITE set_countdown NOTIFY countdownChanged )

    public: //-----------------------------------------------------
    interaction ( const Id<interaction>& id, QObject* parent );

    template <typename impl> interaction(impl& vis, QObject* parent) :
        IdentifiedObject(vis, parent)
    {
        vis.writeTo(*this);
    }

    QVBoxLayout* layout         () const;

    const QString module        () const;
    const QString title         () const;
    const QString description   () const;
    int length                  () const;
    int countdown               () const;

    void set_module         ( QString const& module );
    void set_title          ( QString const& title );
    void set_description    ( QString const& description );
    void set_length         ( const int length );
    void set_countdown      ( const int countdown );

    QStringList inputs()    const;

    void set_current_host ( std::shared_ptr<quarre::user> host );
    std::shared_ptr<quarre::user> current_host() const;

    std::shared_ptr<score::EntityMap<mapping> > mappings() const;

    signals: //----------------------------------------------------
    void moduleChanged      ();
    void titleChanged       ();
    void descriptionChanged ();
    void lengthChanged      ();
    void countdownChanged   ();

    public slots: //-----------------------------------------------
    void onPlusMappingButtonPressed();
    void onMinusMappingButtonPressed();

    private:  //---------------------------------------------------
    QLineEdit*         m_module;
    QLineEdit*         m_title;
    QLineEdit*         m_description;
    QSpinBox*          m_length;
    QSpinBox*          m_countdown;

    QVBoxLayout*    m_layout;

    std::shared_ptr<quarre::user> m_host;
    score::EntityMap<quarre::mapping> m_mappings;

};

}
}
}
