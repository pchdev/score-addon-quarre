#pragma once

#include <score/model/IdentifiedObject.hpp>

namespace score {
namespace addons {
namespace quarre {

class SimpleElement final : public IdentifiedObject<SimpleElement>
{
    Q_OBJECT

    public: //-----------------------------------------------------
    SimpleElement ( const Id<SimpleElement>& id, QObject* parent );

    template <typename impl> SimpleElement(impl& vis, QObject* parent) :
        IdentifiedObject(vis, parent)
    {
        vis.writeTo(*this);
    }

    ~SimpleElement();

};

}
}
}
