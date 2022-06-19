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
//#include "XMLInterface.h"

namespace std {
  template<>
  struct hash<TString> {
    size_t operator()(const TString& key) {
      return hash<std::string>()(std::string(key.Data()));
    }
  };
} // namespace st 


namespace EAL {

////////////////////////////////////////////////////////////////////////////////
/// 
class Sample {
private:

  TString m_sample; // Sample (file) name
  TString m_directory; // File location // Optional
  TString m_process; // Physical process that the sample simulates
  TString m_classification;
  Long_t m_nevents; // Number of Events in the file
  Long_t m_nMCgen; // Number of generated Monte Carlo events
  Long_t m_nMCgen_neg; // Number of negative Monte Carlo events
  Long_t m_ngen; // Total events generated (nMCgen - 2 * nMCgen_neg)
  Float_t m_xsec; // Cross section
  Float_t m_luminosity;
  Float_t m_sample_weight; // xsec/ngen
  std::unique_ptr<TFile> m_file;
  std::unique_ptr<TTree> m_tree;

public:

  Sample(std::unordered_map<TString, TString> initializer) {
    m_sample = initializer.at("sample");
    m_directory = initializer.at("directory");
    m_process = initializer.at("process");
    m_classification = initializer.at("class");
    m_luminosity = std::atof(initializer.at("lumi").Data());
    m_nMCgen = std::atol(initializer.at("nMC").Data());
    m_nMCgen_neg = std::atol(initializer.at("nMCneg"));
    //m_file = std::unique_ptr<TFile>(new TFile(m_directory+m_sample+".root", "UPDATE"));
    //m_tree = std::make_unique<TTree>(m_file->Get<TTree*>("Events"));
    m_sample_weight = m_xsec/m_ngen;
    m_ngen = m_nMCgen - 2*m_nMCgen_neg;
  }
  
  Int_t OpenFile(TString file);
  Int_t SetTree(TString tree);
  

};

//class BackgroundSample : public Sample {
//
//};
//
//class SignalSample : public Sample {
//
//};
//
//class DataSample : public Sample {
//
//};

}
