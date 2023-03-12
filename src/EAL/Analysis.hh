#pragma once
#include <memory>

#include "nlohmann/json.hpp"
#include "yaml-cpp/yaml.h"

namespace EAL {

enum class EventType {
  kUknown = -999,
  kSignal = 0,
  kBackground,
  kData
};

class AnalysisConfig {

};

class Analysis {
private:
  static std::unique_ptr<Analysis> m_instance;
  EAL::AnalysisConfig m_config;
  Analysis();

public:
  auto GetInstance() -> std::unique_ptr<Analysis>;
  auto LoadConfig() -> void;  
  auto Run() -> void;
  
};
}
