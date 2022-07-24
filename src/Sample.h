/** \file */
// Copyright (c) 2022 MKosh

#pragma once

#include <map>
#include <unordered_map>
#include <memory>
#include "Rtypes.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;

namespace EAL {

////////////////////////////////////////////////////////////////////////////////
/// 
class Sample {
public:

  std::string m_sample; // Sample (file) name
  std::string m_sample_directory; // File location // Optional
  Long_t m_nevents; // Number of Events in the file
  Long_t m_nMCgen; // Number of generated Monte Carlo events
  Long_t m_nMCgen_neg; // Number of negative Monte Carlo events
  Long_t m_ngen; // Total events generated (nMCgen - 2 * nMCgen_neg)
  Float_t m_xsec; // Cross section
  Float_t m_sample_weight; // xsec/ngen
  std::string m_file_name;

public:

  Sample(json::object_t sample_init, std::string directory = "") {
    m_sample = sample_init["name"];
    m_xsec = sample_init["xs"];
    m_nMCgen = sample_init["nMC"];
    m_nMCgen_neg = sample_init["nMCneg"];
    m_sample_weight = m_xsec / m_ngen;
    m_ngen = m_nMCgen - 2 * m_nMCgen_neg;

    if (!sample_init["sample_directory"].is_null()) {
      m_sample_directory = sample_init["sample_directory"];
    } else {
      m_sample_directory = directory;
    }

    m_file_name = m_sample_directory + m_sample + ".root";
  }
  
  Int_t OpenFile(TString file);
  Int_t SetTree(TString tree);
  
};

}
