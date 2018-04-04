#pragma once

#include <quarre/process/quarre-process-model.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegate.hpp>
#include <Process/Inspector/ProcessInspectorWidgetDelegateFactory.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class ProcessModel;
class PolymorphicEntity;

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

    void on_entityAdded(const PolymorphicEntity&);
    void on_entityRemoved(const PolymorphicEntity&);

    CommandDispatcher<> m_dispatcher;

};

class InspectorFactory final :
        public Process::InspectorWidgetDelegateFactory_T<ProcessModel, InspectorWidget>
{
    SCORE_CONCRETE  ( " " )

};


}
}
}
