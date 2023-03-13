#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "yaml-cpp/yaml.h"
// #include "nlohmann/json.hpp"
#include "TFile.h"
#include "TTree.h"

#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"

// Compile command:
// g++ test.cc -o test -I../../external/ -L/home/markm/packages/yaml-cpp-yaml-cpp-0.7.0/build/ -lyaml-cpp $(root-config --glibs --cflags --libs) && ./test
//

int main() {

  YAML::Node config = YAML::LoadFile("test.yaml");
  auto files = config["files"];
  // if (files.IsSequence()) std::cout << "seq\n";
  // std::cout << files[0]["name"] << '\n';
   
  YAML::Node file_name = files[0]["name"];
  YAML::Node new_node;
  // temp["variables"].push_back("var1");
  std::string root_file{files[0]["directory"].as<std::string>()+files[0]["name"].as<std::string>()+".root"};

  ROOT::RDataFrame df{"TreeS", root_file};
  auto cols = df.GetColumnNames();
  // YAML::Emitter out;
  std::map<std::string, std::string> temp;
  temp["name"] = "";
  temp["type"] = "";
  temp["status"] = "";

  new_node["variables"] = YAML::Null;
  //new_node["variables"].push_back(var1);

  for (const auto& col : cols) {
    temp["name"] = col;
    temp["type"] = df.GetColumnType(col);
    temp["status"] = "variable";
    new_node["variables"].push_back(temp);
  }

  std::ofstream out{"file.yaml"};
  out << new_node;
  out.close();

  return 0;
}
