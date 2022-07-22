#pragma once

#include <vector>
#include <string>
#include "Sample.h"
#include "../include/nlohmann/json.hpp"


namespace EAL{
using json = nlohamann::json;
////////////////////////////////////////////////////////////////////////////////
/// 
class Process {
public:
  std::string m_process;
  std::string m_common_directory;
  std::vector<Sample> samples;
  std::string m_classification;
  Float_t m_luminosity;

  Process(json::object_t process_init, std::string process_name, std::string directory) {
    m_process = process_name;
    m_common_directory = directory;
    m_classification = process_init["class"];
    m_luminosity = process_init["lumi"];

    for(const auto& smpl : process_init["samples"]) {
      if (process_init.contains("sample_directory")) {
        samples.emplace_back(smpl, process_init["sample_directory"]);
      } else {
        samples.emplace_back(smpl, m_common_directory);
      }
    }
  // for(auto& it : j["VBS_EWK"]["filelist"].items()) { samples.push_back(it.value()["name"]); }
  }
};

}