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
            const quarre::ProcessModel& object,
            const score::DocumentContext& ctx,
            QWidget* parent );

    signals: //------------------------------------------------------------
    void mappingsChanged(QVector<quarre::Mapping>);

    protected slots: //----------------------------------------------------
    void onPlusMappingButtonPressed();
    void onMinusMappingButtonPressed();

    private: //---------------------------------------------------------
    QVBoxLayout* m_layout;
    CommandDispatcher<> m_dispatcher;
};

class InspectorFactory final :
        public Process::InspectorWidgetDelegateFactory_T<ProcessModel, InspectorWidget>
{
    SCORE_CONCRETE  ( "276587EB-0AC9-43A4-BAB8-4536A73BDE85" )

};


}
}
}
