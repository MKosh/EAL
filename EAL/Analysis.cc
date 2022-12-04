/** @cond */
#include <fstream>
#include <string>
#include "nlohmann/json.hpp"
#include "Analysis.h"
/** @endcond */
//using json = nlohmann::
namespace EAL{

json GetJSONContent(std::string file_name) {
  std::ifstream input_file(file_name);
  json file_content;
  std::string text;
  input_file >> file_content;
  return file_content;
}


std::vector<std::string> Analysis::GetFileNames(EAL::ClassID class_id) {
  std::vector<std::string> smpls;
  for (const auto& sample : m_samples) {
    if (m_class_IDs.at(sample.m_file_name) == static_cast<int32_t>(class_id)) {
      smpls.emplace_back(sample.m_file_name);
    }
  }
  return smpls;
}


}