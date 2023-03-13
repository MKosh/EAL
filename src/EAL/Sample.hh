#include <cstdint>
#include <string>
#include "Analysis.hh"

namespace EAL {

enum class SampleType {
  kUknown = -999,
  kSignal = 0,
  kBackground,
  kData
};

class Sample {
private:
  EAL::SampleType m_class; 
  std::string m_name;
  int64_t m_num_events;
  int64_t m_nMC_events;
  int64_t m_nMC_neg_events;
  int64_t m_ngen; // Total events generated (nMC_events - 2 * nMC_neg_events)
  float m_xsec;
  float m_sample_weight;
  float m_luminosity;
public:
  Sample();
  auto GetNumEvents() const -> int64_t;
  auto GetCrossSection() const -> float;
  auto GetSampleWeight() const -> float;
  auto GetLuminosity() const -> float;
};

class DataSample : Sample {
private:
  EAL::SampleType m_class = EAL::SampleType::kData;
  
};

}
