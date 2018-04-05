#pragma once

#include <score/plugins/panel/PanelDelegate.hpp>
#include <score/plugins/panel/PanelDelegateFactory.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class PanelDelegateFactory final: public score::PanelDelegateFactory
{
    SCORE_CONCRETE  ( "992D40B0-C06C-419B-B6BD-96C1129987AC" )

    virtual std::unique_ptr<score::PanelDelegate> make(
            const GUIApplicationContext &ctx) override;

};


}
}
}
