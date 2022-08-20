#pragma once

#include <vector>
#include <string>
#include <iostream>
#include "Sample.h"
#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;
namespace EAL{

enum ClassID {
  kData = 1,
  kSignal,
  kBackground
};

////////////////////////////////////////////////////////////////////////////////
/// 
class Process {
public:
  std::string m_process;
  std::string m_process_directory;
  std::vector<std::string> m_sample_file_names;
  std::vector<EAL::Sample> m_samples;
  EAL::ClassID m_classification;
  Float_t m_luminosity;
  Int_t m_process_id;

  Process(json::object_t process_init, std::string process_name, std::string directory) {
    m_process = process_name;
    m_luminosity = process_init.at("lumi");
    m_process_id = process_init.at("process_id");

    if (process_init["process_directory"].is_null())
      m_process_directory = directory;
    else
      m_process_directory = process_init.at("process_directory");

    if (process_init.at("class") == "data") {
      m_classification = EAL::ClassID::kData;
    } else if (process_init.at("class") == "signal") {
      m_classification = EAL::ClassID::kSignal;
    } else if (process_init.at("class") == "background") {
      m_classification = EAL::ClassID::kBackground;
    } else {
      std::cout << "--- Error setting Class for process: " << m_process << '\n';
    }

    for (const auto& smpl : process_init.at("samples")) {
      m_samples.emplace_back(smpl, m_process_directory);
      m_sample_file_names.emplace_back(m_samples.back().m_file_name);
    }
  }

  void addSamples(json::object_t samples_list);
};

}