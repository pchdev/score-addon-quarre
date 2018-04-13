#pragma once

#include <quarre/mapping/quarre-mapping.hpp>
#include <quarre/device/quarre-device.hpp>
#include <score/model/IdentifiedObject.hpp>
#include <quarre/process/quarre-process-model.hpp>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSpinBox>

namespace score     {
namespace addons    {
namespace quarre    {

class user;

class interaction final : public IdentifiedObject<interaction>
{
    Q_OBJECT
    SCORE_SERIALIZE_FRIENDS

    friend class quarre::ProcessModel;

    public: //-----------------------------------------------------
    interaction ( const Id<interaction>& id, QObject* parent );

    template <typename impl> interaction(impl& vis, QObject* parent) :
        IdentifiedObject(vis, parent)
    {
        vis.writeTo(*this);
    }

    const QString module        () const;
    const QString title         () const;
    const QString description   () const;
    int length                  () const;
    int countdown               () const;

    QStringList inputs()    const;

    public slots: //-----------------------------------------------
    void onModuleChanged            ( QString module );
    void onTitleChanged             ( QString title );
    void onDescriptionChanged       ( QString description );
    void onLengthChanged            ( int length );
    void onCountdownChanged         ( int countdown );

    void onPlusMappingButtonPressed     ( );
    void onMinusMappingButtonPressed    ( );

    private:  //---------------------------------------------------
    QString         m_module;
    QString         m_title;
    QString         m_description;
    int             m_length;
    int             m_countdown;

    std::shared_ptr<quarre::user> m_host;
    score::EntityMap<quarre::mapping> m_mappings;
};

}
}
}
