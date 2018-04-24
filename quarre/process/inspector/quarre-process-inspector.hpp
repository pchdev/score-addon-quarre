#pragma once

#include <quarre/process/quarre-process-model.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <Explorer/Widgets/AddressAccessorEditWidget.hpp>

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QListWidget>

namespace score     {
namespace addons    {
namespace quarre    {

class mapping_view : public QObject
{
    Q_OBJECT

    public:
    mapping_view    ( const score::DocumentContext& dctx,
                      mapping &map, QWidget *parent);

    ~mapping_view();

    QVBoxLayout* layout        () const;
    const QString source       () const;
    const QString destination  () const;
    const QString expression   () const;

    void set_source             ( const QString &source );
    void set_destination        ( const QString &source );
    void set_expression         ( const QString &source );

    protected slots:
    void on_source_address_changed ( const Device::FullAddressAccessorSettings );
    void on_destination_address_changed ( const Device::FullAddressAccessorSettings );
    void on_expression_changed          ( );
    void on_minus_button_pressed        ( );

    signals: // ---------------------------------------------------
    void sourceChanged          ( QString );
    void destinationChanged     ( QString );
    void expressionChanged      ( QString );
    void mappingDeleteRequest   ( quarre::mapping* );

    private: // ---------------------------------------------------
    Explorer::AddressAccessorEditWidget* m_source;
    Explorer::AddressAccessorEditWidget* m_destination;
    QTextEdit* m_expression;
    QPushButton* m_minus_button;
    QVBoxLayout* m_layout;
    QFormLayout* m_form;
    quarre::mapping* m_mapmodel;

};

class ProcessModel;

class InspectorWidget final :
        public Process::InspectorWidgetDelegate_T<quarre::ProcessModel>,
        public Nano::Observer
{
    Q_OBJECT

    friend class quarre::mapping;

    public: //----------------------------------------------------------

    explicit InspectorWidget(
            const ProcessModel &object,
            const score::DocumentContext& ctx,
            QWidget* parent );

    signals:
    void mappingDeleteRequest( quarre::mapping* );
    void address_changed ( QString );

    public slots:
    void update                 ( int );
    void on_address_changed     ( Device::FullAddressAccessorSettings );
    void on_mapping_added       ( quarre::mapping& mapping );
    void on_mapping_removed     ( quarre::mapping* mapping );

    private: //---------------------------------------------------------
    CommandDispatcher<> m_dispatcher;
    QComboBox*         m_module;
    QLineEdit*         m_title;
    QLineEdit*         m_description;
    QSpinBox*          m_length;
    QSpinBox*          m_countdown;
    QVBoxLayout*       m_layout;

    QLineEdit* m_end_expression;
    Explorer::AddressAccessorEditWidget* m_end_expression_source;

    QVector<quarre::mapping_view*> m_mapping_views;
    const score::DocumentContext& m_dctx;
    quarre::interaction* m_interaction;
    const quarre::ProcessModel& m_quarre_process;
};

class InspectorFactory final :
        public Process::InspectorWidgetDelegateFactory_T<ProcessModel, InspectorWidget>
{
    SCORE_CONCRETE  ( "276587EB-0AC9-43A4-BAB8-4536A73BDE85" )

};


}
}
}
