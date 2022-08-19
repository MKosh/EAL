#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <map>
#include "Sample.h"
#include "Process.h"
#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;
////////////////////////////////////////////////////////////////////////////////
/// 
namespace EAL {
class Analysis {
private:
  std::string m_common_directory;
  int m_year;
  std::vector<EAL::Process> m_processes;
  std::map<std::string, int> m_process_IDs;
  std::vector<std::string> m_analysis_sample_files;

public:
  Analysis(std::string input_file_name) {
    std::ifstream input_file(input_file_name);
    json dataset_list;
    input_file >> dataset_list;

    m_common_directory = dataset_list.at("analysis").at("directory");
    m_year = dataset_list.at("analysis").at("year");
    for (const auto& proc : dataset_list.items()) {
      if (!(proc.key() == "analysis")) {
        m_processes.emplace_back(proc.value(), proc.key(), m_common_directory);
        for (const auto& smpl : m_processes.back().m_sample_file_names) {
          if (!(proc.key() == "analysis_directory")) {
            m_analysis_sample_files.emplace_back(smpl);
          }
        }
      }
    }

    for (const auto& process : m_processes) {
      for (const auto& sample_file : process.m_sample_file_names) {
        m_process_IDs[sample_file] = process.m_process_id;
      }
    }

    input_file.close();
  }

  auto getSampleFileNames() {return m_analysis_sample_files; }
  auto getProcessIDs()  {return m_process_IDs; }
  auto getAnalysisYear() { return m_year; }
};
}