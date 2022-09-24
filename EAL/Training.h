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

/*
template<typename T, typename... Args>
T EvalMVA(Args... args) {
  std::vector<T> vec = {args...};
}*/
template<typename T>
T returnOne(float a, float b, float c, float d, float e,
            float f, float g, float h, float i, float j,
            float k, float l, float m, float n, float o) {
  return 1.0f;
}

class Something {
public:
  float operator()(float a, float b, float c, float d, float e,
                    float f, float g, float h, float i, float j,
                    float k, float l, float m, float n, float o) {
    return 1.0;
  }
};

class EvaluateMethod {
private:
  std::vector<float> m_inputs;
  TMVA::Reader* m_reader;
public:
  EvaluateMethod(TMVA::Reader* reader) : m_reader{reader} {}
/*
  template<typename T>
  float returnOne(float a, float b, float c, float d, float e,
            float f, float g, float h, float i, float j,
            float k, float l, float m, float n, float o) {
  std::vector<float> vec{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o};
  return m_reader->EvaluateMVA(vec, "BDT");
}
/*
  template<typename T>
  void bar(T t) {}

  template<typename... Args>
  float operator()(Args &&... args) {
    std::vector<float> dummy = { ((void)bar(std::forward<Args>(args)), args) ... };
    return m_reader->EvaluateMVA(dummy, "BDT");
  }
*/
  template<typename T>
  T VecHelper (T element) { return T{element}; }
  
  template<typename... Args>
  float operator()(Args&&... args) {
    std::vector<float> inputs{ VecHelper(args) ...};
    return m_reader->EvaluateMVA(inputs, "BDT");
  }
/*
  float operator()(float a, float b, float c, float d, float e,
                    float f, float g, float h, float i, float j,
                    float k, float l, float m, float n, float o){
    m_inputs = {a, b, c, d, e, f, g, h, i, j, k, l, m, n, o};
    return m_reader->EvaluateMVA(m_inputs, "BDT");
  }
/*
  template<typename T>
  T operator()(T last) {
    m_inputs.push_back(last);
    return m_reader->EvaluateMVA(m_inputs, "BDT");
  }

  template<typename T, typename... Args>
  void operator()(T first, Args... args) {
    m_inputs.push_back(first);
  }

  template<class T>
  T operator()(std::initializer_list<T> list) {
    for (auto element : list) {
      m_inputs.push_back(element);
    }
    return m_reader->EvaluateMVA(m_inputs, "BDT");
  }
*/
};

/*
class EvaluateMethod {
private:
  std::vector<float> m_inputs;
  TMVA::Reader* m_reader;
public:
  EvaluateMethod(TMVA::Reader* reader) : m_reader{reader} {}

  auto operator()(float a, float b, float c, float d, float e,
                  float f, float g, float h, float i, float j,
                  float k, float l, float m, float n, float o){
    m_inputs = {a, b, c, d, e, f, g, h, i, j, k, l, m, n, o};
    m_reader->EvaluateMVA(m_inputs, "BDT");
  }
};
*/

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
  std::unordered_map<std::string, std::string> m_variable_types;
  std::string m_outfile_name = "TMVAoutput.root";

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