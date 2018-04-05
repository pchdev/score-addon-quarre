#pragma once

#include <QMetaType>
#include <QString>

namespace score     {
namespace addons    {
namespace quarre    {

struct SpecificSettings
{
    int osc_port    {};
    int ws_port     {};
    int max_users   {};
};

}
}
}

Q_DECLARE_METATYPE  ( score::addons::quarre::SpecificSettings )
