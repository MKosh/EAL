#include "Process.h"

void EAL::Process::addSamples(json::object_t samples_list) {
  for(const auto& smpl : samples_list["samples"]) {
    if (smpl.contains("sample_directory")) {
      m_samples.emplace_back(smpl, samples_list["sample_directory"]);
    } else {
      m_samples.emplace_back(smpl, m_process_directory);
    }
  }
}
