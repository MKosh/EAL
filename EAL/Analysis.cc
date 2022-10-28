/** @cond */
#include <fstream>
#include <string>
#include "nlohmann/json.hpp"
#include "Analysis.h"
/** @endcond */
//using json = nlohmann::
namespace EAL{

json GetJSONContent(std::string file_name) {
  //std::cout << "debug 1\n";
  std::ifstream input_file(file_name);
  //std::cout << "debug 2\n";
  json file_content;
  //std::cout << "debug 3\n";
  std::string text;
  //while (!input_file.fail() && !input_file.eof()) {
  //  input_file >> text;
  //  std::cout << text << '\n';
  //}
  //std::cout << input_file << "\n";
  input_file >> file_content;
  //std::cout << "debug 4\n";
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