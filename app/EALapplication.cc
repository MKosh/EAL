#include <iostream>
#include <ranges>
#include <vector>
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

  std::vector<EAL::Process>* processes = new std::vector<EAL::Process>;
  std::vector<std::string> files;

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

  TMVATraining train("data/config/TMVA_settings.json");
  
  TCanvas* c = new TCanvas("c", "c", 600, 600);
  ROOT::RDataFrame df("Events", files, train.m_all_variables);
  auto h = df.Histo1D("lep1_pt");
  h->Draw();
  c->Print("df_plot.pdf");
  c->Close();
   
  return 0;
}

int main() {
  EALapplication();
}