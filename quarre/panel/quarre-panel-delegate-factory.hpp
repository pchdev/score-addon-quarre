#pragma once

#include <score/plugins/panel/PanelDelegate.hpp>
#include <score/plugins/panel/PanelDelegateFactory.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class PanelDelegateFactory final: public score::PanelDelegateFactory
{
    SCORE_CONCRETE  ( " " )

    virtual std::unique_ptr<score::PanelDelegate> make(
            const GUIApplicationContext &ctx) override;

};


}
}
}
