#include <fstream>
#include <iostream>
#include "./include/nlohmann/json.hpp"

using json = nlohmann::json;

void readjson() {
  std::ifstream input_file("data/config/TMVA_settings.json");
  json dataset_list;
  input_file >> dataset_list;

  std::cout << "Analysis has " << dataset_list.at("variables").size() << " variables\n";
  std::cout << " and " << dataset_list.at("spectators").size() << " spectators\n";
}

int main() {
  readjson();
}