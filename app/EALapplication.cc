#include <iostream>
#include <ranges>
#include <vector>
#include "TDOMParser.h"
#include "TXMLDocument.h"
#include "TXMLNode.h"
#include "TXMLAttr.h"
#include "../src/Samples.h"
#include "../include/EAL/XMLInterface.h"


int main() {
  EAL::test();
  //auto node = EAL::XML::Initialize("../datasets/dataset_2016.xml");

  //node = node->GetChildren()->GetNextNode();
  
  //TList* attr_list = node.get()->GetAttributes();
  //std::cout << node->GetNodeName() << "\n";

  return 0;
}