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
                              EAL::Cut::DefinesList& defines,
                              EAL::Cut::CutList& cut_list,
                              std::function<bool(int32_t)> class_cut,
                              std::string_view out_tree_name,
                              std::string_view out_file_name,
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
    .Snapshot(out_tree_name, out_file_name, train.m_all_variables, opts);
}

void CreateIntermediateRootFiles(EAL::Cut::DefinesList& defines,
                                  EAL::Cut::CutList& df_cuts,
                                  EAL::Cut::DataOnly& data_cut,
                                  EAL::Cut::SignalOnly& signal_cut,
                                  EAL::Cut::BackgroundOnly& background_cut,
                                  std::vector<std::string>& data_files,
                                  std::vector<std::string>& signal_files,
                                  std::vector<std::string>& background_files,
                                  EAL::ML::TMVATraining& train,
                                  std::string_view out_file) {
  std::cout << "EAL : Creating intermediate ROOT Files\n    :\n";
  ROOT::RDF::RSnapshotOptions opts;
  opts.fMode = "RECREATE";

  std::cout << "    : filtering Data dataframe\n    :\n";
  ProcessesDataFrame(data_files, "Events", train, defines, df_cuts, data_cut, "data", out_file, opts);

  opts.fMode = "UPDATE";
  std::cout << "    : filtering Signal dataframe\n    :\n";
  ProcessesDataFrame(signal_files, "Events", train, defines, df_cuts, signal_cut, "signal", out_file, opts);

  std::cout << "    : filtering Background dataframe\n    :\n";
  ProcessesDataFrame(background_files, "Events", train, defines, df_cuts, background_cut, "background", out_file, opts);

  std::cout << "EAL : Done writing intermediate ROOT Files\n\n";
}

std::unique_ptr<TMVA::DataLoader> LoadData(EAL::ML::TMVATraining train,
                                            std::string in_file) {
  auto input_file = TFile::Open(in_file.c_str());
  auto signal_tree = input_file->Get<TTree>("signal");
  auto background_tree = input_file->Get<TTree>("background");

  auto data_loader = std::make_unique<TMVA::DataLoader>("TMVAoutput");
  data_loader->AddSignalTree(signal_tree);
  data_loader->SetSignalWeightExpression("mcWeight*genWeight");
  data_loader->AddBackgroundTree(background_tree);
  data_loader->SetBackgroundWeightExpression("mcWeight*genWeight");

  for (const auto& variable : train.m_training_variables) {
    data_loader->AddVariable(variable);
  }
  for (const auto& spectator : train.m_training_spectators) {
    data_loader->AddSpectator(spectator, train.m_variable_types.at(spectator)[0]);
  }

  TCut signal_cut = "";
  TCut background_cut = "";
  data_loader->PrepareTrainingAndTestTree(signal_cut, background_cut, 
    "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");

  return data_loader;
}

std::unique_ptr<TMVA::Factory> CreateTMVAFactory(TMVA::DataLoader* data_loader, 
                                                  EAL::ML::TMVATraining train,
                                                  TFile* out_file) {
  auto factory = std::make_unique<TMVA::Factory>("TMVAClassification", out_file, 
                                                  "!V:!Silent:Color:DrawProgressBar:"
                                                  "AnalysisType=Classification");

  for (const auto& method : train.m_methods) {
    if (method.m_type == "kBDT") {
      factory->BookMethod(data_loader, TMVA::Types::kBDT, method.m_name, method.m_options);
    } else if (method.m_type == "kDL") {
      factory->BookMethod(data_loader, TMVA::Types::kDL, method.m_name, method.m_options);
    } else {
      std::cout << "EAL ERROR : I Don't recognize TMVA method - " << method.m_type << '\n';
    }
  }

  return factory;
}


void ApplyMethod(EAL::ML::TMVATraining train) {
  EAL::EventHolder event{train};
  std::cout << "debug 1\n";
  auto reader = std::make_unique<TMVA::Reader>("!Color:!Silent");
  std::cout << "debug 2\n";
  for (const auto& variable : train.m_training_variables) {
    if (train.m_variable_types.at(variable) == "F") {
      reader->AddVariable(variable, &event.variables_f.at(variable));
    } else if (train.m_variable_types.at(variable) == "I") {
      reader->AddVariable(variable, &event.variables_i.at(variable));
    }
  }
std::cout << "debug 3\n";
  for (const auto& spectator : train.m_training_spectators) {
    if (train.m_variable_types.at(spectator) == "F") {
      reader->AddSpectator(spectator, &event.variables_f.at(spectator));
    } else if (train.m_variable_types.at(spectator) == "I") {
      reader->AddSpectator(spectator, &event.variables_i.at(spectator));
    }
  }
std::cout << "debug 4\n";
  // Fix this! Make the naming programatic, and when bookin the weight file names
  // should be entered programatically. Right now it's just a proof of concept.
  for (const auto& method : train.m_methods) {
    if (method.m_type == "kBDT") {
      reader->BookMVA(method.m_name, "TMVAoutput/weights/TMVAClassification_BDT.weights.xml");
    } else if (method.m_type == "kDNN") {
      reader->BookMVA(method.m_name, "TMVAoutput/weights/TMVAClassification_DNN_GPU.weights.xml");
    }
  }
  std::cout << "EAL : reader methods booked\n    :\n";

  auto EvaluateMethods = [&reader](float a, float b, float c, float d, float e,
                                  float f, float g, float h, float i, float j,
                                  float k, float l, float m, float n, float o){
    //std::vector<float> input{l, b, n, d, g, e, f, h, j, i, o, m, a, c, k};                   
    std::vector<float> inputs{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o};
    return reader->EvaluateMVA(inputs, "BDT");
  };
  auto evalMethod = [&reader](std::vector<float> args) {
    return reader->EvaluateMVA(args, "BDT");
  };

  EAL::ML::Something something;
  EAL::ML::EvaluateMethod evaluate{reader.get()};

  std::cout << "EAL : evaluation object constructed\n    :\n";
  ROOT::RDF::RSnapshotOptions opts;
  //opts.fMode = "RECREATE";
  opts.fOverwriteIfExists = true;
  std::vector<std::string> allvars{train.m_training_variables};
  allvars.push_back("BDT_response");
  std::cout << "EAL : Applying Model\n    :\n";
  ROOT::RDataFrame df("data", "intermediate.root");
  float a{}, b{}, c{}, d{}, e{}, f{}, g{}, h{}, i{}, j{}, k{}, l{}, m{}, n{}, o{};

  //float output = evaluate(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o);
  //std::cout << " output " << output << '\n';

  df.Define("BDT_response", ROOT::Internal::RDF::PassAsVec<15, float>(evalMethod), train.m_training_variables)
    .Snapshot("data", "TMVAapplied2.root", allvars, opts);
  std::cout << "EAL : Dataframe saved\n    :\n";
  //ROOT::RDataFrame df2("signal")

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
  std::cout << "\nEAL : Analysis beginning\n    : \n";

  EAL::Cut::DefinesList defines{process_IDs, class_IDs, samples};
  EAL::Cut::CutList df_cuts;
  EAL::Cut::DataOnly data_cut;
  EAL::Cut::SignalOnly signal_cut;
  EAL::Cut::BackgroundOnly background_cut;
  std::string input_file = "TMVAoutput.root";
  std::string intermediate_file = "intermediate.root";
  std::string output_file = "TMVAoutput.root";
  //std::unique_ptr<TFile> out_file(TFile::Open(output_file.c_str(), "RECREATE"));
  //CreateIntermediateRootFiles(defines, df_cuts, data_cut, signal_cut, background_cut, data_files, signal_files, background_files, train, intermediate_file);
  
  //auto data_loader = LoadData(train, intermediate_file);

  //auto factory = CreateTMVAFactory(data_loader.get(), train, out_file.get());

  //factory->TrainAllMethods();
  //factory->TestAllMethods();
  //factory->EvaluateAllMethods();
  //out_file->Close();
  ApplyMethod(train);

  std::cout << "\nEAL : Done!\n";
  return 0;
}

int main() {
  //std::cout << "debugging 1\n";
  EALapplication();
}