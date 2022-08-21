/** @cond */
#include <iostream>
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
#include "../include/nlohmann/json.hpp"
#include "../EAL/Analysis.h"
#include "../EAL/Training.h"
#include "../EAL/Functions.h"
/** @endcond */
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

  auto dataset = EAL::GetJSONContent("data/config/dataset.json");
  EAL::Analysis anl = EAL::Analysis(dataset);

  auto process_IDs = anl.GetProcessIDs();
  auto files = anl.GetSampleFileNames();

  TMVATraining train("data/config/TMVA_settings.json");
  std::cout << "\nAnalysis beginning:\n--- Creating intermediate ROOT files\n";

  auto setProcessID = [&process_IDs](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    int pid = -999;
    for (const auto& process : process_IDs) {
      if (id.Contains(process.first)) { pid = process.second; }
    }
    return pid;
  };

   auto setSampleYear = [](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    if (id.Contains("2016")) { return 2016; }
    if (id.Contains("2017")) { return 2017; }
    if (id.Contains("2018")) { return 2018; }
    return 0;
  };

  auto setSampleLumin = [&anl](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    for (const auto& smpl : anl.GetAnalysisSamples()) {
      if (id.Contains(smpl.m_file_name)) return smpl.m_luminosity; 
    }
    return -999.0f;
  };

  auto setSampleWeight = [&anl](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    for (const auto& smpl : anl.GetAnalysisSamples()) {
      if (id.Contains(smpl.m_file_name)) {
        //std::cout << "sample: " << smpl.m_sample << "\nWeight: " << smpl.m_sample_weight <<'\n';
        return smpl.m_sample_weight; 
      }
    }
    return -999.0f;
  };

  EAL::tau21_cut cut1;
  EAL::qgid_cut cut2;
  EAL::wv_boosted cut3;

  ROOT::RDataFrame df("Events", files, train.m_all_variables);
  auto modified_df = df.DefinePerSample("process_id", setProcessID)
                        .DefinePerSample("lumin", setSampleLumin)
                        .DefinePerSample("year", setSampleYear)
                        .DefinePerSample("mcWeight", setSampleWeight)
                        .Filter(cut1, {"bos_PuppiAK8_tau2tau1"}, "tau21_cut")
                        .Filter(cut2, {"vbf1_AK4_qgid","vbf2_AK4_qgid"}, "qgid_cut")
                        .Filter(cut3, {"lep2_pt", "bos_PuppiAK8_pt"}, "wv_boosted")
                        .Snapshot("events", "intermediate.root", train.m_all_variables);

  std::cout << "---Done!\n";
  return 0;
}

int main() {
  EALapplication();
}