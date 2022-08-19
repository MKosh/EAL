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

int EALapplication() {
  // Load the JSON files and read the contents. 
  // Make Processes and Samples from the JSON file.
  // Make a dataframe for each process,
  // add each samples file, and set the active branches.
  // Save the df's as new intermediate trees.
  // Load those trees into TMVA.
  // Train and apply.
  // Graph the results.
  ROOT::EnableImplicitMT();
  std::ifstream input_file("data/config/dataset.json");
  json dataset_list;
  input_file >> dataset_list;

  std::string common_directory;
  if (dataset_list.contains("analysis_directory"))
    common_directory = dataset_list["analysis_directory"];
  else
  common_directory = "";

  //std::vector<EAL::Process>* processes = new std::vector<EAL::Process>;
  auto processes = std::make_unique<std::vector<EAL::Process>>();
  std::vector<std::string> files;
  std::map<std::string, int> process_IDs;

  for (const auto& proc : dataset_list.items()) {
    if (!(proc.key() == "analysis_directory")) {
      processes->emplace_back(proc.value(), proc.key(), common_directory);
      for (const auto& smpl : (*processes).back().m_sample_file_names) {
        if (!(proc.key() == "analysis_directory")) {
          files.emplace_back(smpl);
        }
      }
    }
  }

  for (const auto& process : *processes) {
    for (const auto& sample_file : process.m_sample_file_names) {
      process_IDs[sample_file] = process.m_process_id;
    }
  }

  std::cout << '\n';
  TMVATraining train("data/config/TMVA_settings.json");

  auto setProcessID = [&process_IDs](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    int gid = -999;
    for (const auto& process : process_IDs) {
      if (id.Contains(process.first)) { gid = process.second; }
    }
    return gid;
  };

  auto setSampleYear = [&process_IDs](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    int gid = -999;
    for (const auto& process : process_IDs) {
      if (id.Contains(process.first)) { gid = process.second; }
    }
    return gid;
  };
  
  ROOT::RDataFrame df("Events", files, train.m_all_variables);
  auto modified_df = df.DefinePerSample("gid", setProcessID)
                        .DefinePerSample("lumin", [](unsigned int slot, const ROOT::RDF::RSampleInfo& id){return id.Contains("Data") ? 1. : 2.;})
                        .DefinePerSample("sid", [](unsigned int slot, const ROOT::RDF::RSampleInfo& id){return 2;})
                        .DefinePerSample("year", [](unsigned int slot, const ROOT::RDF::RSampleInfo& id){return 2016;})
                        .DefinePerSample("mcWeight", [](unsigned int slot, const ROOT::RDF::RSampleInfo& id){return 1.;});
  modified_df.Snapshot("events", "intermediate.root", train.m_all_variables);

  //TCanvas* c = new TCanvas("c", "c", 600, 600);
  //auto h = modified_df.Histo1D("lumin");
  //h->Draw();
  //c->Print("df_plot.pdf");
  //c->Close();
  

  std::cout << "done\n";
  return 0;
}

int main() {
  EALapplication();
}