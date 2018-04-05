#pragma once
#include <score/command/Command.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

inline const CommandGroupKey& CommandFactoryName()
{
    static const CommandGroupKey key ( "quarrè" );
    return key;
}

}
}
}
