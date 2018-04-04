#pragma once

#include <score/model/Entity.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class SimpleEntity final : public score::Entity<SimpleEntity>
{
    Q_OBJECT

    public:
    SimpleEntity( const Id<SimpleEntity>& id, QObject* parent);
    ~SimpleEntity();

};

}
}
}
