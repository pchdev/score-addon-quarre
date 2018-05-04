#pragma once

#include <quarre/mapping/quarre-mapping.hpp>
#include <quarre/device/quarre-device.hpp>
#include <score/model/IdentifiedObject.hpp>
#include <quarre/process/quarre-process-model.hpp>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class InspectorWidget;

class interaction final : public IdentifiedObject<interaction>
{
    Q_OBJECT
    SCORE_SERIALIZE_FRIENDS

    friend class quarre::ProcessModel;
    friend class quarre::mapping;

    public: //-----------------------------------------------------
    interaction ( const Id<interaction>& id, QObject* parent );

    template <typename impl> interaction(impl& vis, QObject* parent) :
        IdentifiedObject(vis, parent)
    {
        vis.writeTo(*this);
    }

    int length                        ( ) const;
    int countdown                     ( ) const;
    bool dispatch_all                 ( ) const;
    QString module                    ( ) const;
    QString title                     ( ) const;
    QString description               ( ) const;
    QString end_expression            ( ) const;
    QString end_expression_source     ( ) const;
    const QString& end_expression_js  ( ) const;

    std::vector<ossia::value> to_list ( ) const;

    QStringList inputs()    const;
    const std::vector<quarre::mapping*>& mappings();
    ossia::time_sync& get_ossia_tsync() const;

    signals:
    void mapping_added      ( quarre::mapping& mapping );
    void mapping_removed    ( quarre::mapping& mapping );

    public slots: //-----------------------------------------------
    void on_module_changed                  ( QString module );
    void on_title_changed                   ( QString title );
    void on_description_changed             ( QString description );
    void on_length_changed                  ( int length );
    void on_countdown_changed               ( int countdown );
    void on_end_expression_changed          ( QString expression );
    void on_end_expression_source_changed   ( QString source );
    void on_dispatch_all_changed            ( bool dispatch_all );

    void on_mapping_added          ( );
    void on_mapping_removed        ( quarre::mapping* mapping );

    private:  //---------------------------------------------------
    QString         m_module;
    QString         m_title;
    QString         m_description;
    QString         m_end_expression;
    QString         m_end_expression_js;
    QString         m_end_expression_source;
    int             m_length;
    int             m_countdown;
    bool            m_dispatch_all;

    quarre::InspectorWidget* m_inspector;
    Explorer::DeviceDocumentPlugin* m_devdoc_plugin;

    std::vector<quarre::mapping*> m_mappings;
};

}
}
}
