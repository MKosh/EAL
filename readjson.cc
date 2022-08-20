#include <fstream>
#include <iostream>
#include "./include/nlohmann/json.hpp"

using json = nlohmann::json;

void readjson() {
  std::ifstream input_file("data/config/dataset.json");
  json dataset_list;
  input_file >> dataset_list;

  for (const auto& lumi : dataset_list.at("analysis").at("luminosities").items()) {
    std::cout << "year = " << lumi.key() << ", lumi = " << lumi.value() << '\n';
  }
  for (const auto& things : dataset_list.at("processes").items()) {
    std::cout << "\nProcess " << things.key() << " : \n";
    for (const auto& thing : things.value().at("samples").items()) {
      std::cout << "ngen " << static_cast<long>(thing.value().at("nMC")) + static_cast<long>(thing.value().at("nMCneg")) << '\n';
    }
  }

}

int main() {
  readjson();
}