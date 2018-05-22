#pragma once

#include <QJSEngine>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <ossia/ossia.hpp>
#include <Engine/OSSIA2score.hpp>
#include <Engine/score2OSSIA.hpp>

namespace score     {
namespace addons    {
namespace quarre    {
namespace js        {

void            append          ( QJSValueList& arguments, const ossia::value& v , QJSEngine& engine );
void            parse_and_push  ( const QJSValue& jsv, const Device::DeviceList& devlist );
ossia::value    parse_atom      ( const QJSValue& jsv , ossia::val_type vt );

}
}
}
}
