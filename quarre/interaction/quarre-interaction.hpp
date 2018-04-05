#pragma once

#include <score/model/IdentifiedObject.hpp>

namespace score     {
namespace addons    {
namespace quarre    {

class Interaction final : public IdentifiedObject<Interaction>
{
    Q_OBJECT

    public: //-----------------------------------------------------
    Interaction ( const Id<Interaction>& id, QObject* parent );

    template <typename impl> Interaction(impl& vis, QObject* parent) :
        IdentifiedObject(vis, parent)
    {
        vis.writeTo(*this);
    }

    QString         module;
    QString         title;
    QString         description;
    uint16_t        length;
    uint16_t        countdown;
    QStringList     sensors;
    QStringList     gestures;

    ~Interaction();

    private:  //---------------------------------------------------

};

}
}
}
