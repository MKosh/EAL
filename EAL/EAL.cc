#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <initializer_list>
/* #include "ROOT/RDataFrame.hxx"
#include "../EAL/Analysis.h" */
#include "../include/nlohmann/json.hpp"
#include "../EAL/Analysis.h"
#include "../EAL/Functions.h"

using json = nlohmann::json;

////////////////////////////////////////////////////////////////////////////////
///
EAL::Analysis CreateAnalysis(std::vector<json> datasets)
{
  if (datasets.empty())
  {
    EAL::ErrorLog("Cannot load an empty dataset");
    exit(1);
  }
  return EAL::Analysis(datasets);
}

////////////////////////////////////////////////////////////////////////////////
/// Load the datasets from a list of json files.
std::vector<json> GatherDatasetFiles(std::initializer_list<std::string> config_files) 
{
  if (std::empty(config_files))
  {
    // EAL::Log("No files provided to load");
    EAL::ErrorLog("No files provided to load from\n");
    exit(1);
  }
  
  std::vector<json> dataset{};
  for (const auto& file : config_files)
  {
    dataset.emplace_back(file); 
  }
  
  return dataset;
} 

int main()
{
    auto dataset = GatherDatasetFiles({"data/config/dataset_2016.json", "data/config/dataset_2017.json", "data/config/dataset_2018.json"});
  for (const auto& file : dataset)
  {
    std::cout << file << '\n';
  }
  return 0;
}
