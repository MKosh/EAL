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
#include "../src/Training.h"

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

  auto tau21_cut = [](Float_t tau21){ return (tau21 >= 0.0 && tau21 <= 1.0); };
  auto qgid_cut = [](Float_t qgid1, Float_t qgid2){ return ((qgid1 >= 0.0 && qgid1 <= 1.0)
    &&(qgid2 >= 0.0 && qgid2 <= 1.0)); };
  auto wv_boosted = [](Float_t lep2_pt, Float_t bos_PuppiAK8_pt) { return (lep2_pt<0 && bos_PuppiAK8_pt>0); };
  
  ROOT::RDataFrame df("Events", files, train.m_all_variables);
  auto modified_df = df.DefinePerSample("process_id", setProcessID)
                        .DefinePerSample("lumin", setSampleLumin)
                        .DefinePerSample("year", setSampleYear)
                        .DefinePerSample("mcWeight", setSampleWeight)
                        .Filter(tau21_cut, {"bos_PuppiAK8_tau2tau1"}, "tau21_cut")
                        .Filter(qgid_cut, {"vbf1_AK4_qgid","vbf2_AK4_qgid"}, "qgid_cut")
                        .Filter(wv_boosted, {"lep2_pt", "bos_PuppiAK8_pt"}, "wv_boosted")
                        .Snapshot("events", "intermediate2.root", train.m_all_variables);

  
  
   
  std::cout << "---Done!\n";
  return 0;
}

int main() {
  EALapplication();
}