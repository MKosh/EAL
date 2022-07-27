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

  std::vector<std::string> files;

  for (const auto& proc : *processes) {
    for (const auto& smpl : proc.m_sample_names) {
      files.emplace_back(smpl);
    }
  }
  std::vector<std::string> vars = {"AntiIsoInt","bos_AK4AK4_eta","bos_AK4AK4_m","bos_AK4AK4_pt","bosCent","bos_j1_AK4_eta","bos_j1_AK4_pt","bos_j2_AK4_eta","bos_j2_AK4_pt","bos_PuppiAK8_eta","bos_PuppiAK8_m_sd0","bos_PuppiAK8_m_sd0_corr","bos_PuppiAK8_phi","bos_PuppiAK8_pt","bos_PuppiAK8_tau2tau1","btagWeight_loose","dibos_eta","dibos_m","dibos_mt","dibos_phi","dibos_pt","dilep_eta","dilep_m","dilep_mt","dilep_phi","dilep_pt","evt","genWeight","gid","L1PFWeight","lep1_eta","lep1_idEffWeight","lep1_iso","lep1_m","lep1_phi","lep1_pt","lep1_q","lep1_trigEffWeight","lep2_eta","lep2_idEffWeight","lep2_phi","lep2_pt","lep2_trigEffWeight","lumin","mcWeight","MET","MET_phi","nBtag_loose","nBtag_medium","neu_pz_type0","nJet30","nJet30f","nJet50","nPV","puWeight","run","sid","vbf1_AK4_eta","vbf1_AK4_m","vbf1_AK4_phi","vbf1_AK4_pt","vbf1_AK4_qgid","vbf2_AK4_eta","vbf2_AK4_m","vbf2_AK4_phi","vbf2_AK4_pt","vbf2_AK4_qgid","vbf_deta","vbf_eta","vbf_m","vbf_phi","vbf_pt","year","zeppHad","zeppLep"};
  TCanvas* c = new TCanvas("c", "c", 600, 600);
  ROOT::RDataFrame df("Events", files, vars);
  auto h = df.Histo1D("bos_AK4AK4_eta");
  h->Draw();
  c->Print("df_plot.pdf");
  c->Close();
   
  return 0;
}

int main() {
  EALapplication();
}