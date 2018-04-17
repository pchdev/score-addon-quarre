#pragma once

#include <quarre/process/quarre-process-model.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>

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

class ProcessModel;

class InspectorWidget final :
        public Process::InspectorWidgetDelegate_T<quarre::ProcessModel>,
        public Nano::Observer
{
    public: //----------------------------------------------------------

    explicit InspectorWidget(
            const ProcessModel &object,
            const score::DocumentContext& ctx,
            QWidget* parent );

    public slots:
    void on_mapping_added(const quarre::mapping& mapping);


    private: //---------------------------------------------------------
    CommandDispatcher<> m_dispatcher;
    QLineEdit*         m_module;
    QLineEdit*         m_title;
    QLineEdit*         m_description;
    QSpinBox*          m_length;
    QSpinBox*          m_countdown;
    QVBoxLayout*       m_layout;
};

class InspectorFactory final :
        public Process::InspectorWidgetDelegateFactory_T<ProcessModel, InspectorWidget>
{
    SCORE_CONCRETE  ( "276587EB-0AC9-43A4-BAB8-4536A73BDE85" )

};


}
}
}
