#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
////////////////////////////////////////////////////////////////////////////////
/// 
namespace EAL {

enum ClassID {
  kData = 1,
  kSignal,
  kBackground
};

class Sample {
public:

  std::string m_sample; // Sample (file) name
  std::string m_sample_directory; // File location // Optional
  std::string m_year;
  Long_t m_num_events; // Number of Events in the file
  Long_t m_nMC_events; // Number of generated Monte Carlo events
  Long_t m_nMC_neg_events; // Number of negative Monte Carlo events
  Long_t m_ngen; // Total events generated (nMCgen - 2 * nMCgen_neg)
  Float_t m_xsec; // Cross section
  Float_t m_sample_weight; // xsec/ngen
  Float_t m_luminosity;
  std::string m_file_name;

  Sample(json::object_t sample_json, std::string analysis_directory) 
        : m_sample{sample_json.at("name")},
          m_sample_directory{analysis_directory},
          m_year{sample_json.at("year")},
          m_nMC_events{sample_json.at("nMC")},
          m_nMC_neg_events{sample_json.at("nMCneg")},
          m_xsec{sample_json.at("xs")},
          m_luminosity{sample_json.at("lumi")} {
    m_ngen = m_nMC_events - 2*m_nMC_neg_events;
    m_sample_weight = m_xsec / m_ngen; 
    m_file_name = analysis_directory + m_year + "/haddedFiles/" + m_sample + ".root";
  }
};

class Process {
public:
  std::string m_process;
  Int_t m_process_id;
  EAL::ClassID m_classification;

  Process(std::string process_name, json::object_t process_init)
          : m_process{process_name},
            m_process_id{process_init.at("process_id")},
            m_classification{SetProcessClassification(process_init.at("class"))} 
            {}


  EAL::ClassID SetProcessClassification(std::string process_class) {
    if (process_class == "data") return EAL::ClassID::kData;
    else if (process_class == "signal") return EAL::ClassID::kSignal;
    else if (process_class == "background") return EAL::ClassID::kBackground;
    else { 
      std::cout << "--- Error setting Class for a: " << process_class << '\n';  
      return EAL::ClassID::kData;
    }
  }

};

class Analysis {
private:
  std::unordered_map<std::string, Float_t> m_luminosities;
  std::unordered_map<std::string, EAL::Process> m_processes;
  std::vector<EAL::Sample> m_samples;
  std::unordered_map<std::string, Int_t> m_process_IDs;
  std::vector<std::string> m_analysis_sample_files;

public:
  Analysis(json dataset_json) {
    for (const auto& lumi : dataset_json.at("analysis").at("luminosities").items()) {
      m_luminosities.insert({lumi.key(), lumi.value()});
    }
    for (const auto& proc : dataset_json.at("processes").items()) {
      EAL::Process temp_proc{proc.key(), proc.value()};
      m_processes.insert({proc.key(), temp_proc});
      for (const auto& smpl : proc.value().at("samples").items()) {
        m_samples.emplace_back(smpl.value(), dataset_json.at("analysis").at("directory"));
        m_process_IDs[m_samples.back().m_file_name] = proc.value().at("process_id");
        m_analysis_sample_files.emplace_back(m_samples.back().m_file_name);
      }
    }
  }

  auto GetAnalysisSamples() { return m_samples; }
  auto GetSampleFileNames() {return m_analysis_sample_files; }
  auto GetProcessIDs()  {return m_process_IDs; }
};

json GetJSONContent(std::string file_name) {
  std::ifstream input_file(file_name);
  json file_content;
  input_file >> file_content;
  return file_content;
}

}