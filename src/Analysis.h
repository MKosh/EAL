#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "./Sample.h"
#include "Process.h"
#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;
////////////////////////////////////////////////////////////////////////////////
/// 
class Analysis {
public:
  std::string m_common_directory;
  std::vector<EAL::Process> data_samples;
  std::vector<EAL::Process> signal_samples;
  std::vector<EAL::Process> background_samples;
  std::vector<EAL::Process> m_processes;

  Analysis(std::string input_file_name) {
    std::ifstream input_file(input_file_name);
    json dataset_list;
    input_file >> dataset_list;

    if (dataset_list.contains("global_directory"))
      m_common_directory = dataset_list["global_directory"];

    for(const auto& process : processes) {
      if (process.key() == "global_directory" && process.value() != "") {
        m_directory = process.value();
      } else {
        m_processes.emplace_back(process.value(), process.key());
      }
    }
  }
};