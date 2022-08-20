#include <iostream>
#include <fstream>
#include <ranges>
#include <memory>
#include <vector>
#include <map>
#include <fstream>
#include "TFile.h"
#include "TObjArray.h"  
#include "TCanvas.h"
#include "TTree.h"
#include "ROOT/RDataFrame.hxx"
#include "nlohmann/json.hpp"
#include "EAL/headers.h"

using json = nlohmann::json;


json GetJSONContent(std::string file_name) {
  std::ifstream input_file(file_name);
  json file_content;
  input_file >> file_content;
  return file_content;
}

void app() {
  ROOT::EnableImplicitMT();
  EAL::Analysis anl = EAL::Analysis("data/config/dataset.json");

  TMVATraining train("data/config/TMVA_settings.json");
  auto files = anl.getSampleFileNames();
  auto process_IDs = anl.getProcessIDs();

  auto setProcessID = [&process_IDs](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    int gid = -999;
    for (const auto& process : process_IDs) {
      if (id.Contains(process.first)) { gid = process.second; }
    }
    return gid;
  };

  auto setSampleYear = [](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    if (id.Contains("2016")) { return 2016; }
    if (id.Contains("2017")) { return 2017; }
    if (id.Contains("2018")) { return 2018; }
    return 0;
  };
  
  ROOT::RDataFrame df("Events", files, train.m_all_variables);
  auto modified_df = df.DefinePerSample("gid", setProcessID)
                        .DefinePerSample("lumin", [](unsigned int slot, const ROOT::RDF::RSampleInfo& id){return id.Contains("Data") ? 1. : 2.;})
                        .DefinePerSample("sid", [](unsigned int slot, const ROOT::RDF::RSampleInfo& id){return 2;})
                        .DefinePerSample("year", setSampleYear)
                        .DefinePerSample("mcWeight", [](unsigned int slot, const ROOT::RDF::RSampleInfo& id){return 1.;});
  modified_df.Snapshot("events", "intermediate.root", train.m_all_variables);
}

int main() {
  app();
}