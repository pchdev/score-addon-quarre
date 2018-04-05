#pragma once

#include <quarre/process/quarre-process-model.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>

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

    private: //---------------------------------------------------------

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
