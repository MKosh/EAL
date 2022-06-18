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
/// \class A class to hold relevant information for the current XML Document
class XMLDocument {
public:

  TXMLDocument* GetDocument() { return m_xml_doc; }
  TXMLNode* GetRootNode() { return m_root_node; }
  TDOMParser* GetParser() { return m_parser; }
  TXMLNode* GetCurrentNode() { return m_current_node; }
  TXMLNode* GetNextNode() { m_previous_node = m_current_node;
                            m_current_node = m_next_node; 
                            m_next_node = m_current_node->TXMLNode::GetNextNode()->GetNextNode();
                            return m_current_node; }

  TXMLNode* GetNextElementNode();

  XMLDocument(TString file) {
    m_parser = new TDOMParser();
    m_parser->SetValidate(false);
    m_parser->ParseFile(file);

    m_xml_doc = m_parser->GetXMLDocument();
    m_root_node = m_xml_doc->GetRootNode();
    m_current_node = m_root_node;
    m_next_node = m_current_node->TXMLNode::GetChildren()->TXMLNode::GetNextNode();
  }

  ~XMLDocument() {
    delete m_current_node;
    delete m_next_node;
    delete m_previous_node;
    delete m_root_node;
    delete m_xml_doc;
    delete m_parser;
  }

private:
  TXMLDocument* m_xml_doc;
  TXMLNode* m_root_node;
  TDOMParser* m_parser;
  TXMLNode* m_next_node;
  TXMLNode* m_current_node;
  TXMLNode* m_previous_node;
};

////////////////////////////////////////////////////////////////////////////////
/// Create a ROOT XML parser from an XML file
XMLDocument* Initialize(const char* file);
}
void test();
}