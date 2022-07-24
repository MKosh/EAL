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
  // Load the XML files and read the contents into an initializer map. 
  // Make "Sample"s from the maps.
  // Assign each sample to a process.
  // Convert sample's trees into a large process-based dataframe.
  // Save the df's as new intermediate trees.
  // Load those trees into TMVA.
  // Train and apply.
  // Graph the results.
  ROOT::EnableImplicitMT();
  std::ifstream input_file("data/dataset.json");
  json dataset_list;
  input_file >> dataset_list;

  std::string common_directory;
  if (dataset_list.contains("analysis_directory"))
    common_directory = dataset_list["analysis_directory"];
  else
    common_directory = "";

  std::vector<EAL::Process>* processes = new std::vector<EAL::Process>;
  for (const auto& proc : dataset_list.items()) {
    if (!(proc.key() == "analysis_directory"))
      processes->emplace_back(proc.value(), proc.key(), common_directory);
    //std::cout << "Process: " << proc.key() << '\n' << proc.value() << '\n';
  }

  for (auto& proc : *processes) {
    proc.GetSampleNames();
    //std::cout << "Process: " << proc.m_process << '\n';
    //for (const auto& smpl : proc.m_samples) {
    //  std::cout << "Sample: " << smpl.m_file_name << '\n';
    //}
  }
  TCanvas* c = new TCanvas("c", "c", 600, 600);
  ROOT::RDataFrame df("Events", (*processes)[0].m_sample_names, {"lep1_pt"});
  auto h = df.Histo1D("lep1_pt");
  h->Draw();
  c->Print("df_plot.pdf");
  c->Close();
   
/*
    TObjArray files = EAL::OpenFiles(processes[0].m_samples);
    TCanvas* c = new TCanvas("c", "c", 600, 600);
    static_cast<TFile*>(files[0])->Get<TTree>("Events")->Draw("lep1_pt");
    c->Print("plot.pdf");
*/
  return 0;
}

int main() {
  EALapplication();
}