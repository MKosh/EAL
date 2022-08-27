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
  auto class_IDs = anl.GetClassIDs();

  TMVATraining train("data/config/TMVA_settings.json");
  std::cout << "\nAnalysis beginning:\n--- Creating intermediate ROOT files\n";

  auto SetProcessID = [&process_IDs](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    int32_t pid = -999;
    for (const auto& process : process_IDs) {
      if (id.Contains(process.first)) { pid = process.second; }
    }
    return pid;
  };

   auto SetSampleYear = [](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    if (id.Contains("2016")) { return 2016; }
    if (id.Contains("2017")) { return 2017; }
    if (id.Contains("2018")) { return 2018; }
    return 0;
  };

  auto SetSampleLumin = [&anl](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    for (const auto& smpl : anl.GetAnalysisSamples()) {
      if (id.Contains(smpl.m_file_name)) return smpl.m_luminosity; 
    }
    return -999.0f;
  };

  auto SetSampleWeight = [&anl](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    for (const auto& smpl : anl.GetAnalysisSamples()) {
      if (id.Contains(smpl.m_file_name)) {
        //std::cout << "sample: " << smpl.m_sample << "\nWeight: " << smpl.m_sample_weight <<'\n';
        return smpl.m_sample_weight; 
      }
    }
    return -999.0f;
  };

  auto SetSampleClass = [&class_IDs](unsigned int slot, const ROOT::RDF::RSampleInfo& id) {
    for (const auto& classes : class_IDs) {
      if (id.Contains(classes.first)) { return classes.second; }
    }
    return -999;
  };

  EAL::Cut::Tau21 tau21_cut;
  EAL::Cut::QGid qgid_cut;
  EAL::Cut::WVBoosted wv_boosted_cut;
  EAL::Cut::LepPt lep_pt_cut;
  EAL::Cut::LepEta lep_eta_cut;
  EAL::Cut::FatjetPt fatjet_pt_cut;
  EAL::Cut::FatjetEta fatjet_eta_cut;
  EAL::Cut::FatjetTau21 fatjet_tau21_cut;
  EAL::Cut::VBSJetsMjj vbs_jets_mjj_cut;
  EAL::Cut::VBSJetsPt vbs_jets_pt_cut;
  EAL::Cut::VBSDeltaEta vbs_deta_cut;
  EAL::Cut::METpt met_pt_cut;
  EAL::Cut::bTagVeto btag_veto_cut;
  EAL::Cut::Isolation iso_cut;
  EAL::Cut::ZeppLep zepp_lep_cut;
  EAL::Cut::ZeppHad zepp_had_cut;
  EAL::Cut::WVSignalRegion wv_sr_cut;
  EAL::Cut::DataOnly data_cut;
  EAL::Cut::SignalOnly signal_cut;
  EAL::Cut::BackgroundOnly background_cut;


  ROOT::RDataFrame df("Events", files, train.m_all_variables);
  auto modified_df = df.DefinePerSample("process_id", SetProcessID)
                        .DefinePerSample("lumin", SetSampleLumin)
                        .DefinePerSample("year", SetSampleYear)
                        .DefinePerSample("mcWeight", SetSampleWeight)
                        .DefinePerSample("class_id", SetSampleClass)
                        .Filter(tau21_cut, {"bos_PuppiAK8_tau2tau1"}, "tau21_cut")
                        .Filter(qgid_cut, {"vbf1_AK4_qgid","vbf2_AK4_qgid"}, "qgid_cut")
                        .Filter(wv_boosted_cut, {"lep2_pt", "bos_PuppiAK8_pt"}, "wv_boosted")
                        .Filter(lep_pt_cut, {"lep1_pt"}, "lep_pt")
                        .Filter(lep_eta_cut, {"lep1_m", "lep1_eta"}, "lep_eta")
                        .Filter(fatjet_pt_cut, {"bos_PuppiAK8_pt"}, "fatjet_pt")
                        .Filter(fatjet_eta_cut, {"bos_PuppiAK8_eta"}, "fatjet_eta")
                        .Filter(fatjet_tau21_cut, {"bos_PuppiAK8_tau2tau1"}, "fatjet_tau21")
                        .Filter(vbs_jets_mjj_cut, {"vbf_m"}, "vbs_jets_mjj")
                        .Filter(vbs_jets_pt_cut, {"vbf1_AK4_pt", "vbf2_AK4_pt"}, "vbs_jets_pt")
                        .Filter(vbs_deta_cut, {"vbf_deta"}, "vbs_deta")
                        .Filter(met_pt_cut, {"MET"}, "met_pt")
                        .Filter(btag_veto_cut, {"nBtag_loose"}, "btag_veto")
                        .Filter(iso_cut, {"AntiIsoInt", "bosCent"}, "isolation")
                        .Filter(zepp_lep_cut, {"zeppLep", "vbf_deta"}, "zepp_lep")
                        .Filter(zepp_had_cut, {"zeppHad", "vbf_deta"}, "zepp_had")
                        .Filter(wv_sr_cut, {"bos_PuppiAK8_m_sd0"}, "wv_sr");
                        //.Snapshot("events", "intermediate3.root", train.m_all_variables);
  std::cout << "   filtering dataframes\n";
  ROOT::RDF::RSnapshotOptions opts;
  opts.fMode = "RECREATE";

  std::cout << "   Writing dataframes\n";
  auto data_df = modified_df.Filter(data_cut, {"class_id"}).Snapshot("data", "intermediate3.root", train.m_all_variables, opts);
  opts.fMode = "UPDATE";
  auto signal_df = modified_df.Filter(signal_cut, {"class_id"}).Snapshot("signal", "intermediate3.root", train.m_all_variables, opts);
  auto background_df = modified_df.Filter(background_cut, {"class_id"}).Snapshot("background", "intermediate3.root", train.m_all_variables, opts);

  std::cout << "---Done!\n";
  return 0;
}

int main() {
  EALapplication();
}