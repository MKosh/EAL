#pragma once

#include <memory>
#include <vector>

#include "TDOMParser.h"
#include "TXMLDocument.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include "TList.h"
#include "TString.h"

namespace EAL {
namespace XML {
////////////////////////////////////////////////////////////////////////////////
/// Create a ROOT XML parser from an XML file
TXMLNode* Initialize(const char* file);
}
}