#pragma once

#include <vector>
#include <string>
#include <iostream>
#include "Sample.h"
#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;
namespace EAL{

////////////////////////////////////////////////////////////////////////////////
/// 
class Process {
public:
  std::string m_process;
  std::string m_process_directory;
  std::vector<std::string> m_sample_names;
  std::vector<EAL::Sample> m_samples;
  std::string m_classification;
  Float_t m_luminosity;

  Process(json::object_t process_init, std::string process_name, std::string directory) {
    m_process = process_name;
    if (process_init["process_directory"].is_null())
      m_process_directory = directory;
    else
      m_process_directory = process_init.at("process_directory");
    m_classification = process_init.at("class");
    m_luminosity = process_init.at("lumi");

    for (const auto& smpl : process_init.at("samples")) {
      m_samples.emplace_back(smpl, m_process_directory);
    }

  // for(auto& it : j["VBS_EWK"]["filelist"].items()) { samples.push_back(it.value()["name"]); }
  }

  void GetSampleNames() {
    for (const auto& smpl : m_samples) {
      m_sample_names.emplace_back(smpl.m_file_name);
    }
  }

  void addSamples(json::object_t samples_list);
};

}