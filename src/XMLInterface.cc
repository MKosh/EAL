#include <memory>
#include "TString.h"
#include "TDOMParser.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include "TXMLDocument.h"
#include "../include/EAL/XMLInterface.h"

////////////////////////////////////////////////////////////////////////////////
/// Parse "file" and return a pointer to an TXMLDocument.
/// @param file file name for the XML file to parse
TXMLNode* EAL::XML::Initialize(const char* file) {
  auto parser = new TDOMParser();
  parser->SetValidate(false);
  parser->ParseFile(file);

  return parser->GetXMLDocument()->GetRootNode();
}

////////////////////////////////////////////////////////////////////////////////
/// Get the s

