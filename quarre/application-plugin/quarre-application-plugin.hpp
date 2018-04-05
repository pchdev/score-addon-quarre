#pragma once

#include <score/plugins/application/GUIApplicationPlugin.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class ApplicationPlugin final : public score::GUIApplicationPlugin
{
    // quarrè addon consists in:

    // - an oscquery server device, allowing predetermined number of remote client connections
    // - meaning a new type of device in the device explorer
    //
    // - a new panel under the device explorer, to visualize client connections more clearly
    //
    // - a new 'quarrè-interaction' process, with a form layout, containing:
    //      - the name of the interaction
    //      - the id of the interaction module that will get displayed on the remote
    //      - the length of the interaction (-1 = inf)
    //      - a description of the interaction
    //      - a maximum countdown length, to notify the user of the interaction

    public: //----------------------------------------------------
    ApplicationPlugin(const score::GUIApplicationContext& appctx);

    protected: //-------------------------------------------------

    virtual void on_newDocument         ( score::Document& doc ) override;
    virtual void on_documentChanged     ( Document *olddoc, Document *newdoc ) override;
    virtual void on_initDocument        ( score::Document& doc ) override;
    virtual void on_loadedDocument      ( score::Document& doc ) override;
    virtual void on_createdDocument     ( score::Document& doc ) override;

    virtual void on_activeWindowChanged     ( ) override;
    virtual bool handleStartup              ( ) override;
    virtual void prepareNewDocument         ( ) override;

    virtual score::GUIElements makeGUIElements ( ) override;
};

}
}
}
