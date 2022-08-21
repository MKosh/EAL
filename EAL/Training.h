#include <vector>
#include <iostream>
#include <string>
#include <fstream>

#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/TMVAGui.h"

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

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
};

////////////////////////////////////////////////////////////////////////////////
///
class TMVATraining {
public:

  std::vector<TMVAMethod> m_methods;
  std::vector<std::string> m_training_variables;
  std::vector<std::string> m_training_spectators;
  std::vector<std::string> m_all_variables; // training variables and spectators

  TMVATraining(std::string settings_file) {
    std::ifstream TMVA_settings(settings_file);
    if (!TMVA_settings.is_open()) {
      std::cout << "ERROR: Cannot open TMVA settings file for configuration." << '\n';
    } else {
      json settings;
      TMVA_settings >> settings;

      for (const auto& method : settings["methods"]) {
        m_methods.emplace_back(method);
      }

      for (const auto& variable : settings["variables"]) {
        m_training_variables.emplace_back(variable.at("name"));
        m_all_variables.emplace_back(variable.at("name"));
      }

      for (const auto& spectator : settings["spectators"]) {
        m_training_spectators.emplace_back(spectator.at("name"));
        m_all_variables.emplace_back(spectator.at("name"));
      }

    }
  }


};