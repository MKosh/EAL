#pragma once
/** @cond */
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include "Rtypes.h"
#include "nlohmann/json.hpp"
/** @endcond */
using json = nlohmann::json;
////////////////////////////////////////////////////////////////////////////////
/// 
namespace EAL {

enum ClassID {
  kUnknown = -999,
  kData = 1,
  kSignal,
  kBackground
};

class Sample {
public:

  std::string m_sample; // Sample (file) name
  std::string m_sample_directory; // File location // Optional
  std::string m_year;
  int64_t m_num_events; // Number of Events in the file
  int64_t m_nMC_events; // Number of generated Monte Carlo events
  int64_t m_nMC_neg_events = 0; // Number of negative Monte Carlo events
  int64_t m_ngen; // Total events generated (nMCgen - 2 * nMCgen_neg)
  Float_t m_xsec; // Cross section
  Float_t m_sample_weight; // xsec/ngen
  Float_t m_luminosity;
  std::string m_file_name;

  Sample(json::object_t sample_json, json::object_t analysis) 
        : m_sample{sample_json.at("name")},
          m_sample_directory{analysis.at("directory")},
          m_year{analysis.at("year")},
          m_nMC_events{sample_json.at("nMC")},
          m_nMC_neg_events{sample_json.at("nMCneg")},
          m_xsec{sample_json.at("xs")},
          m_luminosity{analysis.at("luminosity")} {
    m_ngen = m_nMC_events - 2*m_nMC_neg_events;
    m_sample_weight = m_xsec / m_ngen; 
    m_file_name = m_sample_directory + m_year + "/haddedFiles/" + m_sample + ".root";
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
public:
  std::unordered_map<std::string, Float_t> m_luminosities;
  std::unordered_map<std::string, EAL::Process> m_processes;
  std::vector<EAL::Sample> m_samples;
  std::unordered_map<std::string, Int_t> m_process_IDs;
  std::vector<std::string> m_analysis_sample_files;
  std::unordered_map<std::string, int32_t> m_class_IDs;

public:
  Analysis(std::vector<json> datasets) {
    for (const auto& dataset : datasets) {
      //m_luminosities.insert({dataset.at("analysis").at("year"), dataset.at("analysis").at("luminosity")});
      //std::string temp_year = dataset.at("analysis").at("year");
      //Float_t temp_lumi = dataset.at("analysis").at("luminosity");
      //m_luminosities.insert({"2016", 35867.06});
      //std::cout << "debugging lumis for " << dataset.at("analysis").at("year") << '\n';
      m_luminosities.insert(std::make_pair(dataset.at("analysis").at("year"),dataset.at("analysis").at("luminosity")));
      for (const auto& proc : dataset.at("processes").items()) {
        //std::cout << "debugging process init for " << proc.key() << "\n";
        EAL::Process temp_proc{proc.key(), proc.value()};
        m_processes.insert({proc.key(), temp_proc});

        for (const auto& smpl : proc.value().at("samples").items()) {
          //std::cout << "debugging sample init for" << smpl.value() << '\n';
          m_samples.emplace_back(smpl.value(), dataset.at("analysis"));
          m_process_IDs[m_samples.back().m_file_name] = proc.value().at("process_id");
          m_class_IDs.insert({m_samples.back().m_file_name, static_cast<int32_t>(temp_proc.m_classification)});
          m_analysis_sample_files.emplace_back(m_samples.back().m_file_name);
          //std::cout << "debugging sample init end\n";
        }
      }
    }
  }

  auto GetAnalysisSamples() { return m_samples; }
  auto GetSampleFileNames() {return m_analysis_sample_files; }
  auto GetProcessIDs() { return m_process_IDs; }
  auto GetClassIDs() { return m_class_IDs; }
  auto GetFileNames(EAL::ClassID class_id) -> std::vector<std::string>;
};

json GetJSONContent(std::string file_name);

}