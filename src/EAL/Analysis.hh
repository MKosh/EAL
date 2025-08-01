#pragma once
/// \cond
#include <memory>
// #include <gtest/gtest.h>
/// \endcond
#include "nlohmann/json.hpp"

namespace EAL {


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
