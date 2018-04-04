#pragma once

#include <quarre/polymorphic-entity/quarre-polymorphic-entity-factory.hpp>
#include <score/plugins/customfactory/SerializableInterface.hpp>
#include <score/model/Entity.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class PolymorphicEntity :
        public score::Entity<PolymorphicEntity>,
        public score::SerializableInterface<PolymorphicElementFactory>
{
    Q_OBJECT
    SCORE_SERIALIZE_FRIENDS

    public: // --------------------------------------------------------

        PolymorphicEntity(
            const Id<PolymorphicEntity>& id,
            QObject* parent);

    template <typename impl>
    PolymorphicEntity( impl& vis, QObject* parent ) :
        score::Entity<PolymorphicEntity>(vis, parent)
    {
        vis.writeTo(*this);
    }

    ~PolymorphicEntity();
};


}
}
}

#define POLYMORPHIC_ENTITY_METADATA(Export, Model, Uuid, ObjectKey, PrettyName) \
MODEL_METADATA(Export, Tutorial::PolymorphicElementFactory, Model, Uuid, ObjectKey, PrettyName)
