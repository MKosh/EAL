#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <initializer_list>

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"
#include "TMVA/Reader.h"
#include "ROOT/RDFHelpers.hxx"

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;
namespace EAL {
namespace ML {

class EvaluateMethod {
private:
  TMVA::Reader* m_reader;
  std::string m_method_name;
public:
  EvaluateMethod(TMVA::Reader* reader, std::string name) : m_reader{reader},
    m_method_name{name} {}

  float operator()(std::vector<float> inputs) {
    return m_reader->EvaluateMVA(inputs, m_method_name);
  }
};


class TMVAMethod {
public:
  std::string m_name;
  std::string m_type;
  std::string m_options;

  TMVAMethod(json::object_t json) {
    m_name = json.at("name");
    m_type = json.at("Types");
    m_options = json.at("options");
  }

  float EvaluateMethod(TMVA::Reader* reader, std::vector<float> inputs) {
    return reader->EvaluateMVA(inputs, m_name);
  }
};

////////////////////////////////////////////////////////////////////////////////
///
class TMVATraining {
public:

  std::vector<TMVAMethod> m_methods;
  std::vector<std::string> m_training_variables;
  std::vector<std::string> m_training_spectators;
  std::vector<std::string> m_all_variables; // training variables and spectators
  std::unordered_map<std::string, std::string> m_variable_types;
  std::string m_intermediate = "Dataset";
  std::string m_intermediate_file = "Dataset.root";
  std::string m_TMVA_output = "TMVAoutput";
  std::string m_TMVA_output_file = "TMVAoutput.root";
  std::string m_dataloader_settings;
  std::string m_factory_settings;
  std::string m_reader_settings;

  TMVATraining(std::string settings_file) {
    std::ifstream TMVA_settings(settings_file);
    if (!TMVA_settings.is_open()) {
      std::cout << "ERROR: Cannot open TMVA settings file for configuration." << '\n';
    } else {
      json settings;
      TMVA_settings >> settings;

      if (settings.at("analysis").contains("TMVAoutput_file")) {
        m_TMVA_output = settings.at("analysis").at("TMVAoutput_file");
        m_TMVA_output_file = m_TMVA_output+"_SBtmva.root";
      }

      if (settings.at("analysis").contains("intermediate")) {
        m_intermediate = settings.at("analysis").at("intermediate");
        m_intermediate_file = m_intermediate+".root";
      }

      if (settings.at("analysis").contains("dataloader")) {
        m_dataloader_settings = settings.at("analysis").at("dataloader");
      }
      if (settings.at("analysis").contains("factory")) {
        m_factory_settings = settings.at("analysis").at("factory");
      }
      if (settings.at("analysis").contains("reader")) {
        m_reader_settings = settings.at("analysis").at("reader");
      }

      for (const auto& method : settings["methods"]) {
        m_methods.emplace_back(method);
      }

      for (const auto& variable : settings["variables"]) {
        m_training_variables.emplace_back(variable.at("name"));
        m_all_variables.emplace_back(variable.at("name"));
        m_variable_types.insert(std::make_pair(variable.at("name"),variable.at("type")));
      }

      for (const auto& spectator : settings["spectators"]) {
        m_training_spectators.emplace_back(spectator.at("name"));
        m_all_variables.emplace_back(spectator.at("name"));
        m_variable_types.insert(std::make_pair(spectator.at("name"),spectator.at("type")));
      }

    }
  }
};

}
}