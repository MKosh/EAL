/** @cond */
#include <fstream>
#include "nlohmann/json.hpp"
#include "Analysis.h"
/** @endcond */
//using json = nlohmann::
namespace EAL{

json GetJSONContent(std::string file_name) {
  std::ifstream input_file(file_name);
  json file_content;
  input_file >> file_content;
  return file_content;
}

}