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
EAL::XML::XMLDocument* EAL::XML::Initialize(const char* file) {

}

TXMLNode* EAL::XML::XMLDocument::GetNextElementNode() {
  if (m_next_node && m_next_node->GetNodeType() == TXMLNode::kXMLElementNode) 
      return m_next_node;

  if (m_current_node->HasChildren()) {
    return m_current_node->GetChildren()->TXMLNode::GetNextNode();
  }

}
////////////////////////////////////////////////////////////////////////////////
/// Get the s

