/** 
* MIT License
* Copyright (c) 2022 MKosh
*/

#include <iostream>
#include <vector>
#include "../include/nlohmann/json.hpp"
#include "Sample.h"
#include "TString.h"

////////////////////////////////////////////////////////////////////////////////
/// A longer description. Permission is hereby granted, free of charge, to any 
/// person obtaining a copy of this software and associated documentation files 
/// (the "Software"), to deal in the Software without restriction, including 
/// without limitation the rights
///
/// @param nothing There are no params for this function.
//void EAL::test() {
//  TString s = "Hello, world";
//  std::cout << s << "\n";
//};

////////////////////////////////////////////////////////////////////////////////
/// Open the sample's .root file 
///
/// @param file The name of the file given as a TString
Int_t EAL::Sample::OpenFile(TString file) {
//  m_file = std::make_unique<TFile>(new TFile(m_directory+m_sample+".root", "READ"));
//  if (!m_file)
//    return -1; // File did not open
//  if (m_file->IsZombie())
//    return -2; // File is a zombie
  return 1;
};

////////////////////////////////////////////////////////////////////////////////
/// Open the sample's TTree
///
/// @param tree The name of the tree given as a TString
Int_t EAL::Sample::SetTree(TString tree) {
//  if (!m_file)
//    return -1; // Sample file is not open
//  m_tree = std::make_unique<TTree>(m_file->Get<TTree>(tree));
//  if (!m_tree)
//    return -2; // Tree is not open
  return 1;
};