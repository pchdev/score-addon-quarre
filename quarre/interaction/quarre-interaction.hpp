#pragma once

#include <quarre/mapping/quarre-mapping.hpp>
#include <score/model/IdentifiedObject.hpp>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSpinBox>

namespace score     {
namespace addons    {
namespace quarre    {

class Interaction final : public IdentifiedObject<Interaction>
{
    Q_OBJECT
    SCORE_SERIALIZE_FRIENDS

    Q_PROPERTY  ( QString module READ module WRITE set_module NOTIFY moduleChanged )
    Q_PROPERTY  ( QString title READ title WRITE set_title NOTIFY titleChanged )
    Q_PROPERTY  ( QString description READ description WRITE set_description NOTIFY descriptionChanged )
    Q_PROPERTY  ( int length READ length WRITE set_length NOTIFY lengthChanged )
    Q_PROPERTY  ( int countdown READ countdown WRITE set_countdown NOTIFY countdownChanged )

    public: //-----------------------------------------------------
    Interaction ( const Id<Interaction>& id, QObject* parent );

    template <typename impl> Interaction(impl& vis, QObject* parent) :
        IdentifiedObject(vis, parent)
    {
        vis.writeTo(*this);
    }

    QVBoxLayout* layout         () const;

    const QString& module       () const;
    const QString& title        () const;
    const QString& description  () const;
    const int length            () const;
    const int countdown         () const;

    void set_module         ( QString const& module );
    void set_title          ( QString const& title );
    void set_description    ( QString const& description );
    void set_length         ( const int length );
    void set_countdown      ( const int countdown );

    QVector<quarre::Mapping> mappings();

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

    score::EntityMap<quarre::Mapping> m_mappings;

};

}
}
}
