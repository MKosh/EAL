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
//#include "XMLInterface.h"

//namespace std {
//  template<>
//  struct hash<TString> {
//    std::size_t operator()(const TString& key) {
//      return hash<TString>()(key.Data());
//    }
//  };
//} // namespace st 


namespace EAL {

////////////////////////////////////////////////////////////////////////////////
/// 
class Sample {
private:

  std::string m_sample; // Sample (file) name
  std::string m_directory; // File location // Optional
  //std::string m_process; // Physical process that the sample simulates
  
  Long_t m_nevents; // Number of Events in the file
  Long_t m_nMCgen; // Number of generated Monte Carlo events
  Long_t m_nMCgen_neg; // Number of negative Monte Carlo events
  Long_t m_ngen; // Total events generated (nMCgen - 2 * nMCgen_neg)
  Float_t m_xsec; // Cross section
  //Float_t m_luminosity;
  Float_t m_sample_weight; // xsec/ngen
  std::unique_ptr<TFile> m_file;
  TTree* m_tree;

public:

  Sample(json::object_t sample_init, std::string directory = "") {
    m_sample = sample_init["name"];

    if (!sample_init["directory"].is_null()) {
      m_directory = sample_init["directory"];
    } else {
      m_directory = directory;
    }
    m_nMCgen = sample_init["nMC"];
    m_nMCgen_neg = sample_init["nMCneg"];
    m_file = std::unique_ptr<TFile>(new TFile(static_cast<TString>(m_directory)
                        + static_cast<TString>(m_sample) + ".root", "UPDATE"));

    m_tree = m_file->Get<TTree>("Events");
    m_sample_weight = m_xsec / m_ngen;
    m_ngen = m_nMCgen - 2 * m_nMCgen_neg;
  }
  
  Int_t OpenFile(TString file);
  Int_t SetTree(TString tree);
  
};

}
