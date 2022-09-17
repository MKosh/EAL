/** @cond */
#include <iostream>
#include <ranges>
#include <memory>
#include <vector>
#include <map>
#include <fstream>
#include <functional>
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

void ProcessesDataFrame(std::vector<std::string>& file_names, std::string& tree_name,
                              EAL::ML::TMVATraining& train,
                              EAL::Cut::DefinesList& defines,
                              EAL::Cut::CutList& cut_list,
                              std::function<bool(int32_t)> class_cut,
                              ROOT::RDF::RSnapshotOptions& opts) {
  ROOT::RDataFrame df(tree_name, file_names, train.m_all_variables);
  df.DefinePerSample("process_id", defines.set_process_ids)
    .DefinePerSample("lumi", defines.set_sample_lumi)
    .DefinePerSample("year", defines.set_sample_year)
    .DefinePerSample("mcWeight", defines.set_sample_weight)
    .DefinePerSample("class_id", defines.set_sample_class)
    .Filter(cut_list.tau21_cut, {"bos_PuppiAK8_tau2tau1"}, "tau21_cut")
    .Filter(cut_list.qgid_cut, {"vbf1_AK4_qgid","vbf2_AK4_qgid"}, "qgid_cut")
    .Filter(cut_list.wv_boosted_cut, {"lep2_pt", "bos_PuppiAK8_pt"}, "wv_boosted")
    .Filter(cut_list.lep_pt_cut, {"lep1_pt"}, "lep_pt")
    .Filter(cut_list.lep_eta_cut, {"lep1_m", "lep1_eta"}, "lep_eta")
    .Filter(cut_list.fatjet_pt_cut, {"bos_PuppiAK8_pt"}, "fatjet_pt")
    .Filter(cut_list.fatjet_eta_cut, {"bos_PuppiAK8_eta"}, "fatjet_eta")
    .Filter(cut_list.fatjet_tau21_cut, {"bos_PuppiAK8_tau2tau1"}, "fatjet_tau21")
    .Filter(cut_list.vbs_jets_mjj_cut, {"vbf_m"}, "vbs_jets_mjj")
    .Filter(cut_list.vbs_jets_pt_cut, {"vbf1_AK4_pt", "vbf2_AK4_pt"}, "vbs_jets_pt")
    .Filter(cut_list.vbs_deta_cut, {"vbf_deta"}, "vbs_deta")
    .Filter(cut_list.met_pt_cut, {"MET"}, "met_pt")
    .Filter(cut_list.btag_veto_cut, {"nBtag_loose"}, "btag_veto")
    .Filter(cut_list.iso_cut, {"AntiIsoInt", "bosCent"}, "isolation")
    .Filter(cut_list.zepp_lep_cut, {"zeppLep", "vbf_deta"}, "zepp_lep")
    .Filter(cut_list.zepp_had_cut, {"zeppHad", "vbf_deta"}, "zepp_had")
    .Filter(cut_list.wv_sr_cut, {"bos_PuppiAK8_m_sd0"}, "wv_sr")
    .Filter(class_cut, {"class_id"})
    .Snapshot("data", "intermediate3.root", train.m_all_variables, opts);
}

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

  std::vector<json> datasets;
  datasets.emplace_back(EAL::GetJSONContent("data/config/dataset_2016.json"));
  //std::cout << "debugging 2\n";
  datasets.emplace_back(EAL::GetJSONContent("data/config/dataset_2017.json"));
  //std::cout << "debugging 3\n";
  datasets.emplace_back(EAL::GetJSONContent("data/config/dataset_2018.json"));
  //std::cout << "debugging 4\n";

  EAL::Analysis anl = EAL::Analysis(datasets);

  auto data_files = anl.GetFileNames(EAL::ClassID::kData);
  auto signal_files = anl.GetFileNames(EAL::ClassID::kSignal);
  auto background_files = anl.GetFileNames(EAL::ClassID::kBackground);
  auto process_IDs = anl.GetProcessIDs();
  auto class_IDs = anl.GetClassIDs();
  auto samples = anl.GetAnalysisSamples();

  bool debugging = false;

  EAL::ML::TMVATraining train("data/config/TMVA_settings.json");
  std::cout << "\nAnalysis beginning:\n--- Creating intermediate ROOT files\n\n";
/*
  auto SetProcessID = [&process_IDs](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    int32_t pid = -999;
    for (const auto& process : process_IDs) {
      if (id.Contains(process.first)) { pid = process.second; }
    }
    return pid;
  };

  std::string current_dataframe = "data";
  auto SetSampleYear = [&current_dataframe, debugging](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    if (debugging) std::cout << "Processing " << current_dataframe << " dataframe\n--- " << id.AsString() << "\n\n";
    if (id.Contains("2016")) { return 2016; }
    if (id.Contains("2017")) { return 2017; }
    if (id.Contains("2018")) { return 2018; }
    return 0;
  };

  auto SetSampleLumin = [&anl, class_IDs](unsigned int slot, const ROOT::RDF::RSampleInfo& id){
    for (const auto& smpl : anl.GetAnalysisSamples()) {
      if (id.Contains(smpl.m_file_name) && class_IDs.at(smpl.m_file_name) 
        != static_cast<int32_t>(EAL::ClassID::kData)) { 
        return smpl.m_luminosity;
      }
      else if (id.Contains(smpl.m_file_name) && class_IDs.at(smpl.m_file_name) 
        == static_cast<int32_t>(EAL::ClassID::kData)) { 
        return 1.0f;
      }
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
*/
  //EAL::Cut::Tau21 tau21_cut;
  //EAL::Cut::QGid qgid_cut;
  //EAL::Cut::WVBoosted wv_boosted_cut;
  //EAL::Cut::LepPt lep_pt_cut;
  //EAL::Cut::LepEta lep_eta_cut;
  //EAL::Cut::FatjetPt fatjet_pt_cut;
  //EAL::Cut::FatjetEta fatjet_eta_cut;
  //EAL::Cut::FatjetTau21 fatjet_tau21_cut;
  //EAL::Cut::VBSJetsMjj vbs_jets_mjj_cut;
  //EAL::Cut::VBSJetsPt vbs_jets_pt_cut;
  //EAL::Cut::VBSDeltaEta vbs_deta_cut;
  //EAL::Cut::METpt met_pt_cut;
  //EAL::Cut::bTagVeto btag_veto_cut;
  //EAL::Cut::Isolation iso_cut;
  //EAL::Cut::ZeppLep zepp_lep_cut;
  //EAL::Cut::ZeppHad zepp_had_cut;
  //EAL::Cut::WVSignalRegion wv_sr_cut;
  EAL::Cut::DefinesList defines{process_IDs, class_IDs, samples};
  EAL::Cut::CutList df_cuts;
  EAL::Cut::DataOnly data_cut;
  EAL::Cut::SignalOnly signal_cut;
  EAL::Cut::BackgroundOnly background_cut;

  ROOT::RDF::RSnapshotOptions opts;
  opts.fMode = "RECREATE";

  std::cout << "    filtering Data dataframe\n\n";
  ROOT::RDataFrame df("Events", data_files, train.m_all_variables);
  auto modified_df = df.DefinePerSample("process_id", defines.set_process_ids)
                        .DefinePerSample("lumi", defines.set_sample_lumi)
                        .DefinePerSample("year", defines.set_sample_year)
                        .DefinePerSample("mcWeight", defines.set_sample_weight)
                        .DefinePerSample("class_id", defines.set_sample_class)
                        .Filter(df_cuts.tau21_cut, {"bos_PuppiAK8_tau2tau1"}, "tau21_cut")
                        .Filter(df_cuts.qgid_cut, {"vbf1_AK4_qgid","vbf2_AK4_qgid"}, "qgid_cut")
                        .Filter(df_cuts.wv_boosted_cut, {"lep2_pt", "bos_PuppiAK8_pt"}, "wv_boosted")
                        .Filter(df_cuts.lep_pt_cut, {"lep1_pt"}, "lep_pt")
                        .Filter(df_cuts.lep_eta_cut, {"lep1_m", "lep1_eta"}, "lep_eta")
                        .Filter(df_cuts.fatjet_pt_cut, {"bos_PuppiAK8_pt"}, "fatjet_pt")
                        .Filter(df_cuts.fatjet_eta_cut, {"bos_PuppiAK8_eta"}, "fatjet_eta")
                        .Filter(df_cuts.fatjet_tau21_cut, {"bos_PuppiAK8_tau2tau1"}, "fatjet_tau21")
                        .Filter(df_cuts.vbs_jets_mjj_cut, {"vbf_m"}, "vbs_jets_mjj")
                        .Filter(df_cuts.vbs_jets_pt_cut, {"vbf1_AK4_pt", "vbf2_AK4_pt"}, "vbs_jets_pt")
                        .Filter(df_cuts.vbs_deta_cut, {"vbf_deta"}, "vbs_deta")
                        .Filter(df_cuts.met_pt_cut, {"MET"}, "met_pt")
                        .Filter(df_cuts.btag_veto_cut, {"nBtag_loose"}, "btag_veto")
                        .Filter(df_cuts.iso_cut, {"AntiIsoInt", "bosCent"}, "isolation")
                        .Filter(df_cuts.zepp_lep_cut, {"zeppLep", "vbf_deta"}, "zepp_lep")
                        .Filter(df_cuts.zepp_had_cut, {"zeppHad", "vbf_deta"}, "zepp_had")
                        .Filter(df_cuts.wv_sr_cut, {"bos_PuppiAK8_m_sd0"}, "wv_sr")
                        .Filter(data_cut, {"class_id"})
                        .Snapshot("data", "intermediate5.root", train.m_all_variables, opts);

  opts.fMode = "UPDATE";
  current_dataframe = "Signal"; 
  std::cout << "    filtering Signal dataframe\n\n";

  ROOT::RDataFrame df2("Events", signal_files, train.m_all_variables);
  auto signal_df = df2.DefinePerSample("process_id", SetProcessID)
                        .DefinePerSample("lumi", SetSampleLumin)
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
                        .Filter(wv_sr_cut, {"bos_PuppiAK8_m_sd0"}, "wv_sr")
                        .Filter(signal_cut, {"class_id"})
                        .Snapshot("signal", "intermediate5.root", train.m_all_variables, opts);

  current_dataframe = "background";
  std::cout << "    filtering Background dataframe\n";
  ROOT::RDataFrame df3("Events", background_files, train.m_all_variables);
  auto background_df = df3.DefinePerSample("process_id", SetProcessID)
                        .DefinePerSample("lumi", SetSampleLumin)
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
                        .Filter(wv_sr_cut, {"bos_PuppiAK8_m_sd0"}, "wv_sr")
                        .Filter(background_cut, {"class_id"})
                        .Snapshot("background", "intermediate5.root", train.m_all_variables, opts);

  std::cout << "\n--- Done!\n";
  return 0;
}

int main() {
  //std::cout << "debugging 1\n";
  EALapplication();
}