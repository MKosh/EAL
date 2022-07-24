#include <iostream>
#include <memory>
#include "TFile.h"
#include "TObjArray.h"
#include "./Process.h"
#include "./Sample.h"

namespace EAL {

TObjArray OpenFiles(std::vector<EAL::Sample> samples) {
  TObjArray files;
  for (const auto& smpl : samples) {
    files.Add(TFile::Open(smpl.m_file_name.c_str(), "READ"));
  }
  return files;
}
}