/** @cond */
#include <iostream>
#include <ranges>
#include <memory>
#include <vector>
#include <map>
#include <fstream>
#include <functional>
#include <string_view>
#include <algorithm>
#include "TFile.h"
#include "TObjArray.h"  
#include "TCanvas.h"
#include "TTree.h"
#include "TCut.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"
#include "TMVA/Reader.h"
#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "../include/nlohmann/json.hpp"
#include "../EAL/Analysis.h"
#include "../EAL/Training.h"
#include "../EAL/Functions.h"
/** @endcond */
using json = nlohmann::json;

void ProcessesDataFrame(std::vector<std::string>& file_names, std::string_view tree_name,
                              EAL::ML::TMVATraining& train,
                              EAL::Define::DefinesList& defines,
                              EAL::Cut::CutList& cut_list,
                              std::function<bool(int32_t)> class_cut,
                              std::string_view out_tree_name,
                              std::string_view out_file_name,
                              ROOT::RDF::RSnapshotOptions& opts) {
  ROOT::RDataFrame df(tree_name, file_names, train.m_all_variables);
  df.DefinePerSample("gid", defines.set_process_ids)
    .DefinePerSample("lumin", defines.set_sample_lumi)
    .DefinePerSample("year", defines.set_sample_year)
    .DefinePerSample("mcWeight", defines.set_sample_weight)
    .DefinePerSample("class_id", defines.set_sample_class)
    //.Filter(cut_list.tau21_cut, {"bos_PuppiAK8_tau2tau1"}, "tau21_cut") // boosted
    .Filter(cut_list.qgid_cut, {"vbf1_AK4_qgid","vbf2_AK4_qgid"}, "qgid_cut")
    //.Filter(cut_list.wv_boosted_cut, {"lep2_pt", "bos_PuppiAK8_pt"}, "wv_boosted") // boosted
    .Filter(cut_list.wv_resolved_cut, {"lep2_pt", "bos_AK4AK4_pt"}, "wv_resolved") // resolved
    .Filter(cut_list.lep_pt_cut, {"lep1_pt"}, "lep_pt")
    .Filter(cut_list.lep_eta_cut, {"lep1_m", "lep1_eta"}, "lep_eta")
    //.Filter(cut_list.fatjet_pt_cut, {"bos_PuppiAK8_pt"}, "fatjet_pt") // boosted
    //.Filter(cut_list.fatjet_eta_cut, {"bos_PuppiAK8_eta"}, "fatjet_eta") // boosted
    //.Filter(cut_list.fatjet_tau21_cut, {"bos_PuppiAK8_tau2tau1"}, "fatjet_tau21") // boosted
    .Filter(cut_list.resolved_jet_pt, {"bos_j1_AK4_pt", "bos_j2_AK4_pt"}, "resolved_jets") // resolved
    .Filter(cut_list.vbs_jets_mjj_cut, {"vbf_m"}, "vbs_jets_mjj")
    .Filter(cut_list.vbs_jets_pt_cut, {"vbf1_AK4_pt", "vbf2_AK4_pt"}, "vbs_jets_pt")
    .Filter(cut_list.vbs_deta_cut, {"vbf_deta"}, "vbs_deta")
    .Filter(cut_list.met_pt_cut, {"MET"}, "met_pt")
    .Filter(cut_list.btag_veto_cut, {"nBtag_loose"}, "btag_veto")
    .Filter(cut_list.iso_cut, {"AntiIsoInt", "bosCent"}, "isolation")
    .Filter(cut_list.zepp_lep_cut, {"zeppLep", "vbf_deta"}, "zepp_lep")
    .Filter(cut_list.zepp_had_cut, {"zeppHad", "vbf_deta"}, "zepp_had")
    //.Filter(cut_list.wv_sr_cut_b, {"bos_PuppiAK8_m_sd0"}, "wv_sr_b") // boosted
    .Filter(cut_list.wv_sr_cut_r, {"bos_AK4AK4_m"}, "wv_sr_r") // resolved
    .Filter(class_cut, {"class_id"})
    .Snapshot(out_tree_name, out_file_name, train.m_all_variables, opts);
}

void CreateIntermediateRootFiles(EAL::Define::DefinesList& defines,
                                  EAL::Cut::CutList& df_cuts,
                                  EAL::Cut::DataOnly& data_cut,
                                  EAL::Cut::SignalOnly& signal_cut,
                                  EAL::Cut::BackgroundOnly& background_cut,
                                  std::vector<std::string>& data_files,
                                  std::vector<std::string>& signal_files,
                                  std::vector<std::string>& background_files,
                                  EAL::ML::TMVATraining& train) {
  EAL::LogLine("Creating intermediate ROOT file:");
  EAL::Log(train.m_intermediate_file);

  ROOT::RDF::RSnapshotOptions opts;
  opts.fMode = "RECREATE";

  EAL::Log("filtering Data dataframe");
  ProcessesDataFrame(data_files, "Events", train, defines, df_cuts, data_cut, "data", train.m_intermediate_file, opts);

  opts.fMode = "UPDATE";
  EAL::Log("filtering Signal dataframe");
  ProcessesDataFrame(signal_files, "Events", train, defines, df_cuts, signal_cut, "signal", train.m_intermediate_file, opts);

  EAL::Log("filtering Background dataframe");
  ProcessesDataFrame(background_files, "Events", train, defines, df_cuts, background_cut, "background", train.m_intermediate_file, opts);

  EAL::Log("Done writing intermediate ROOT Files");
}

std::unique_ptr<TMVA::DataLoader> LoadData(EAL::ML::TMVATraining train) {
  auto input_file = TFile::Open((train.m_intermediate_file).c_str());
  if (!input_file->IsOpen()) {
    EAL::Log("input intermediate file not open");
  }
  auto signal_tree = input_file->Get<TTree>("signal");
  auto background_tree = input_file->Get<TTree>("background");

  auto data_loader = std::make_unique<TMVA::DataLoader>(train.m_TMVA_output);
  data_loader->AddSignalTree(signal_tree);
  data_loader->SetSignalWeightExpression("mcWeight*genWeight");
  data_loader->AddBackgroundTree(background_tree);
  data_loader->SetBackgroundWeightExpression("mcWeight*genWeight");

  for (const auto& variable : train.m_training_variables) {
    data_loader->AddVariable(variable);
    EAL::LogLine("variable = " + variable + ", type = " + train.m_variable_types.at(variable)[0]);
  }
  for (const auto& spectator : train.m_training_spectators) {
    data_loader->AddSpectator(spectator, train.m_variable_types.at(spectator)[0]);
    EAL::LogLine("variable = " + spectator + ", type = " + train.m_variable_types.at(spectator)[0]);
  }

  TCut signal_cut = "";
  TCut background_cut = "";
  data_loader->PrepareTrainingAndTestTree(signal_cut, background_cut, train.m_dataloader_settings);

  return data_loader;
}

std::unique_ptr<TMVA::Factory> CreateTMVAFactory(TMVA::DataLoader* data_loader, 
                                                  EAL::ML::TMVATraining train,
                                                  TFile* out_file) {
  auto factory = std::make_unique<TMVA::Factory>("TMVAClassification", out_file, 
                                                  train.m_factory_settings);

  for (const auto& method : train.m_methods) {
    if (method.m_type == "kBDT") {
      factory->BookMethod(data_loader, TMVA::Types::kBDT, method.m_name, method.m_options);
    } else if (method.m_type == "kDL") {
      factory->BookMethod(data_loader, TMVA::Types::kDL, method.m_name, method.m_options);
    } else if (method.m_type == "kMLP") {
      factory->BookMethod(data_loader, TMVA::Types::kMLP, method.m_name, method.m_options);
    } else if (method.m_type == "kFisher") {
      factory->BookMethod(data_loader, TMVA::Types::kFisher, method.m_name, method.m_options);
    } else {
      std::cout << "EAL ERROR : I Don't recognize TMVA method - " << method.m_type << '\n';
    }
  }

  return factory;
}


auto ApplyDefines(ROOT::RDF::RNode df, EAL::ML::TMVATraining train, 
                  TMVA::Reader* reader, unsigned int i = 0) {
  if (i == train.m_methods.size()) return df;

  EAL::ML::EvaluateMethod eval{reader, train.m_methods[i].m_name};

  return ApplyDefines(df.Define(train.m_methods[i].m_name,  
                      ROOT::Internal::RDF::PassAsVec<15, float>(eval),
                      train.m_training_variables), train, reader, ++i);
}

void ApplyMethod(EAL::ML::TMVATraining train) {
  std::string function_name{"ApplyMethod"};
  EAL::EventHolder event{train};

  auto reader = std::make_unique<TMVA::Reader>(train.m_reader_settings);

  for (const auto& variable : train.m_training_variables) {
    if (train.m_variable_types.at(variable) == "F") {
      reader->AddVariable(variable, &event.variables_f.at(variable));
      EAL::LogLine("Adding variable : " + variable);
    } else if (train.m_variable_types.at(variable) == "I") {
      reader->AddVariable(variable, &event.variables_i.at(variable));
      EAL::LogLine("Adding variable : " + variable);
    }
  }

  for (const auto& spectator : train.m_training_spectators) {
    if (train.m_variable_types.at(spectator) == "F") {
      reader->AddSpectator(spectator, &event.variables_f.at(spectator));
      EAL::LogLine("Adding spectator : " + spectator + ", F type = " + train.m_variable_types.at(spectator));
    } else if (train.m_variable_types.at(spectator) == "I") {
      reader->AddSpectator(spectator, &event.variables_i.at(spectator));
      EAL::LogLine("Adding spectator : " + spectator + ", I type = " + train.m_variable_types.at(spectator));
    }
  }

  std::string weight_file = train.m_TMVA_output+"/weights/TMVAClassification_";
  for (const auto& method : train.m_methods) {
    reader->BookMVA(method.m_name, weight_file+method.m_name+".weights.xml");
  }

  EAL::LogFromFunction(function_name, "reader methods booked");

  ROOT::RDF::RSnapshotOptions opts;
  opts.fMode = "UPDATE";
  opts.fOverwriteIfExists = true;

  std::vector<std::string> all_vars{train.m_all_variables};
  EAL::LogLine("Applying Models");

  ROOT::RDataFrame df("data", train.m_intermediate_file);
  //auto df_with_defines = ApplyDefines(df, train, reader.get());
  auto latest_df = std::make_unique<ROOT::RDF::RNode>(df);

  for (const auto& method : train.m_methods) {
    EAL::ML::EvaluateMethod eval{reader.get(), method.m_name};
    EAL::Log("\tAppying "+method.m_name);
    all_vars.push_back(method.m_name);
    latest_df = std::make_unique<ROOT::RDF::RNode>(latest_df->Define(method.m_name,
                                ROOT::Internal::RDF::PassAsVec<15, float>(eval),
                                train.m_training_variables));
    //df.Define(method.m_name, ROOT::Internal::RDF::<15, float>(eval), train.m_training_variables)
    //.Snapshot(train.m_TMVA_output+"/data", train.m_TMVA_output_file, all_vars, opts);
  }
  latest_df = std::make_unique<ROOT::RDF::RNode>(latest_df->Define("classID", [](){ return 3; }));
  all_vars.emplace_back("classID");
  latest_df->Snapshot(train.m_TMVA_output+"/DataTree", train.m_TMVA_output_file, all_vars, opts);
  //df_with_defines.Snapshot(train.m_TMVA_output+"/data", train.m_TMVA_output_file, all_vars, opts);

  EAL::LogFromFunction(function_name, "Dataframe saved to file:"+train.m_TMVA_output_file);
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
  //EAL::Log("ahhh1");
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

  EAL::ML::TMVATraining train("data/config/TMVA_settings_resolved.json");
  EAL::Log("Analysis beginning");

  EAL::Define::DefinesList defines{process_IDs, class_IDs, samples};
  EAL::Cut::CutList df_cuts;
  EAL::Cut::DataOnly data_cut;
  EAL::Cut::SignalOnly signal_cut;
  EAL::Cut::BackgroundOnly background_cut;

  std::unique_ptr<TFile> out_file(TFile::Open(train.m_TMVA_output_file.c_str(), "RECREATE"));
  //CreateIntermediateRootFiles(defines, df_cuts, data_cut, signal_cut, background_cut, data_files, signal_files, background_files, train);
  
  auto data_loader = LoadData(train);

  auto factory = CreateTMVAFactory(data_loader.get(), train, out_file.get());

  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();

  std::ofstream ROC_file;
  ROC_file.open("docs/ROC/Methods.md", std::ios_base::app);
  ROC_file << "----------------------------------------\n";
  ROC_file << "|Method\t|AUC\t|Time (s)\t|Hyperparameters\t|\n";
  ROC_file << "|:---:|:---:|:---:|:---:|\n";
  for (const auto& method : train.m_methods) {
    ROC_file << "| " << method.m_name << "\t| " << factory->GetROCIntegral(data_loader.get(), method.m_name) << "\t| \t| \t|\n";
  }
  ROC_file << "----------------------------------------\n\n";
  ROC_file.close();

  out_file->Close();
  
  //ApplyMethod(train);

  EAL::Log("Done!");

  //ROOT::RDataFrame df{"background", "Dataset.root"};
  //df.Histo1D({"hist", "hist", 20, 0.0, })

  return 0;
}

int main() {
  //std::cout << "debugging 1\n";
  EALapplication();
}