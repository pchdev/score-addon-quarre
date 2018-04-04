#pragma once

#include <Process/Process.hpp>
#include "quarre-process-metadata.hpp"

#include <quarre/simple-element/simple-element.hpp>
#include <quarre/polymorphic-entity/polymorphic-entity.hpp>

#include <score/model/EntityMap.hpp>

namespace score {
namespace addons {
namespace quarre {

class ProcessModel final : public Process::ProcessModel
{
    SCORE_SERIALIZE_FRIENDS
    PROCESS_METADATA_IMPL       ( quarre::ProcessModel )

    //Q_PROPERTY      ( )

    Q_OBJECT

    public: //--------------------------------------------
        ProcessModel(const TimeVal& duration,
                     const Id<Process::ProcessModel>& id,
                     QObject* parent);

    template<typename impl>
    ProcessModel(impl& vis, QObject* parent) :
        Process::ProcessModel(vis, parent)
    {
        vis.writeTo(*this);
    }

    score::EntityMap<SimpleElement> simple_elements;
    score::EntityMap<PolymorphicEntity> polymorphic_entities;

    private: //--------------------------------------------

    virtual QString prettyName() const override;

    virtual void startExecution() override;
    virtual void stopExecution() override;
    virtual void reset() override;

    virtual ProcessStateDataInterface* startStateData() const override;
    virtual ProcessStateDataInterface* endStateData() const override;

    virtual Selection selectableChildren() const override;
    virtual Selection selectedChildren() const override;

    virtual void setSelection(const Selection &s) const override;

    virtual void setDurationAndScale(const TimeVal &newDuration) override;
    virtual void setDurationAndGrow(const TimeVal &newDuration) override;
    virtual void setDurationAndShrink(const TimeVal& newDuration) override;
};

}
}
}
