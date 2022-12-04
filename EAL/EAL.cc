#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <initializer_list>

/* #include "ROOT/RDataFrame.hxx"
#include "../EAL/Analysis.h" */
#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;

std::vector<json> LoadDatasetConfig(std::initializer_list<std::string> config_files) 
{
  std::vector<json> dataset{};
  for (const auto& file : config_files)
  {
    dataset.emplace_back(file); 
  }
  return dataset;
} 

int main()
{
  auto dataset = LoadDatasetConfig({"data/config/dataset_2016.json", "data/config/dataset_2017.json", "data/config/dataset_2018.json"});

  for (const auto& file : dataset)
  {
    std::cout << file << '\n';
  }
  return 0;
}
