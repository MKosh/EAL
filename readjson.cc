#include <fstream>
#include <iostream>
#include "./include/nlohmann/json.hpp"

using json = nlohmann::json;

void readjson() {
  std::ifstream input_file("data/config/dataset.json");
  json dataset_list;
  input_file >> dataset_list;

  std::cout << dataset_list.at("analysis").at("directory") << '\n';
  for (const auto& thing : dataset_list.at("analysis").items()) {
    std::cout << thing.key() << '\n';
  }
}

int main() {
  readjson();
}