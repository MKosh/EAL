#include <iostream>
#include <ranges>
#include <memory>
#include <vector>
#include <map>
#include <fstream>
#include <TFile.h>
#include "TObjArray.h"  
#include "TCanvas.h"
#include "TTree.h"
#include "ROOT/RDataFrame.hxx"
#include "../include/nlohmann/json.hpp"
#include "../include/EAL/headers.h"

using json = nlohmann::json;

void app() {
  ROOT::EnableImplicitMT();
  EAL::Analysis anl2016 = EAL::Analysis("data/config/dataset.json");

  TMVATraining train("data/config/TMVA_settings.json");
  auto files = anl2016.getSampleFileNames();
  auto process_IDs = anl2016.getProcessIDs();

  auto setProcessID = [&process_IDs](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    int gid = -999;
    for (const auto& process : process_IDs) {
      if (id.Contains(process.first)) { gid = process.second; }
    }
    return gid;
  };

  auto setSampleYear = [&anl2016](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    return 2016;
  };
  
  ROOT::RDataFrame df("Events", files, train.m_all_variables);
  auto modified_df = df.DefinePerSample("gid", setProcessID)
                        .DefinePerSample("lumin", [](unsigned int slot, const ROOT::RDF::RSampleInfo& id){return id.Contains("Data") ? 1. : 2.;})
                        .DefinePerSample("sid", [](unsigned int slot, const ROOT::RDF::RSampleInfo& id){return 2;})
                        .DefinePerSample("year", [](unsigned int slot, const ROOT::RDF::RSampleInfo& id){return 2016;})
                        .DefinePerSample("mcWeight", [](unsigned int slot, const ROOT::RDF::RSampleInfo& id){return 1.;});
  modified_df.Snapshot("events", "intermediate.root", train.m_all_variables);
}

int main() {
  app();
}