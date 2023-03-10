#include <iostream>
#include <string>
#include <vector>
#include "ROOT/RDataFrame.hxx"
#include "THStack.h"
#include "TH1.h"
#include "TCanvas.h"
#include "EAL/Plotting.h"

void SetHistProperties(TH1* hist, int x) {
  hist->SetLineWidth(0);
  hist->SetFillStyle(1001);
  hist->SetFillColor(x);
  hist->Scale(2);
}

void SetDataHistProperties(TH1* hist) {
  //hist->SetLineWidth(0);
  hist->SetFillStyle(0);
  hist->SetFillColor(0);
  hist->SetMarkerStyle(20);
  hist->SetMarkerColor(1);
  hist->SetMarkerSize(0.6);
}

void SetErrorHistProperties(TH1* hist) {
  hist->SetFillStyle(3145);
  hist->SetMarkerStyle(0);
  hist->SetFillColor(kGray+2);
  hist->SetLineColor(1);
  hist->Scale(2);
}

void Plot() {
  ROOT::RDataFrame background_df{"TMVAoutput/TestTree", "TMVAoutput.root"};
  //ROOT::RDataFrame signal_df{"TMVAoutput/TrainTree", "TMVAoutput.root"};
  ROOT::RDataFrame data_df{"TMVAoutput/data", "TMVAoutput.root"};
  std::vector<std::string> weights{"lumi", "mcWeight", "genWeight", "L1PFWeight",
                                    "puWeight", "lep1_idEffWeight", "lep1_trigEffWeight",
                                    "btagWeight_loose", "process_id", "year"};
  EAL::Plot::NormalizeAndScale norm_and_scale;
  EAL::Plot::NormalizeData norm_data;

  auto h12 = background_df.Define("TotalWeight", norm_and_scale, weights)
    .Filter([](float a){ return a == 12; }, {"process_id"})
    .Histo1D({"hist12", "hist12", 30,-2.4,2.4}, "lep1_eta", "TotalWeight");
  auto h13 = background_df.Define("TotalWeight", norm_and_scale, weights)
    .Filter([](float a){ return a == 13; }, {"process_id"})
    .Histo1D({"hist12", "hist12", 30,-2.4,2.4}, "lep1_eta", "TotalWeight");
  auto h14 = background_df.Define("TotalWeight", norm_and_scale, weights)
    .Filter([](float a){ return a == 14; }, {"process_id"})
    .Histo1D({"hist12", "hist12", 30,-2.4,2.4}, "lep1_eta", "TotalWeight");
  auto h15 = background_df.Define("TotalWeight", norm_and_scale, weights)
    .Filter([](float a){ return a == 15; }, {"process_id"})
    .Histo1D({"hist12", "hist12", 30,-2.4,2.4}, "lep1_eta", "TotalWeight");
  auto h11 = background_df.Define("TotalWeight", norm_and_scale, weights)
    .Filter([](float a){ return a == 11; }, {"process_id"})
    .Histo1D({"hist12", "hist12", 30,-2.4,2.4}, "lep1_eta", "TotalWeight");
  auto h1 = data_df.Define("TotalWeight", norm_data, weights)
    .Filter([](int a){ return a == 1; }, {"process_id"})
    .Histo1D({"hist12", "hist12", 30,-2.4,2.4}, "lep1_eta");
  auto hErrors = background_df.Define("TotalWeight", norm_and_scale, weights)
    .Histo1D({"hist12", "hist12", 30,-2.4,2.4}, "lep1_eta", "TotalWeight");
  
  Stat_t _stats[4];  
  Stat_t _statsZ[4];
  Stat_t _statsW[4];
h12->GetStats(_stats);
h14->GetStats(_statsZ);
h13->GetStats(_statsW);

  SetHistProperties(h12.GetPtr(), 833);
  SetHistProperties(h13.GetPtr(), 856);
  SetHistProperties(h14.GetPtr(), 796);
  SetHistProperties(h15.GetPtr(), 606);
  SetHistProperties(h11.GetPtr(), 626);
  SetDataHistProperties(h1.GetPtr());
  SetErrorHistProperties(hErrors.GetPtr());

  gStyle->SetLineScalePS(0.5);
  gStyle->SetHatchesSpacing(2.5);
  gStyle->SetHatchesLineWidth(1);

  THStack* hs = new THStack("hs", "");
  
  std::cout << "Diboson entries = " << h12.GetPtr()->GetEntries() << " " << _stats[0] << '\n';
  std::cout << "Wjets entries = " << h13.GetPtr()->GetEntries() << " " << _statsW[0] << '\n';
  std::cout << "Zjets entries = " << h14.GetPtr()->GetEntries() << " " << _statsZ[0] << '\n';
  std::cout << "Top entries = " << h15.GetPtr()->GetEntries() << '\n';
  std::cout << "Signal entries = " << h11.GetPtr()->GetEntries() << '\n';
  std::cout << "Data entries = " << h1.GetPtr()->GetEntries() << "\n\n";
  h12->ResetStats();
  //h12->Sumw2();
  h14->ResetStats();
  h13->ResetStats();
  h12->GetStats(_stats);
  h14->GetStats(_statsZ);
  h13->GetStats(_statsW);
  std::cout << "Diboson entries = " << h12.GetPtr()->GetEffectiveEntries() << " " << _stats[0] << '\n';
  std::cout << "Wjets entries = " << h13.GetPtr()->GetEffectiveEntries() << " " << _statsW[0] << '\n';
  std::cout << "Zjets entries = " << h14.GetPtr()->GetEffectiveEntries() << " " << _statsZ[0] << '\n';
  std::cout << "Top entries = " << h15.GetPtr()->GetEffectiveEntries() << '\n';
  std::cout << "Signal entries = " << h11.GetPtr()->GetEffectiveEntries() << '\n';
  std::cout << "Data entries = " << h1.GetPtr()->GetEffectiveEntries() << '\n';
  
  hs->Add(h14.GetPtr());
  hs->Add(h12.GetPtr());
  hs->Add(h13.GetPtr());
  hs->Add(h15.GetPtr());
  hs->Add(h11.GetPtr());
  auto hs_max = hs->GetMaximum();
  hs->SetMaximum(hs_max*1.3);

  TCanvas C;
  hs->Draw("HIST");
  hErrors->Draw("E2 SAME");
  h1->Draw("E1 SAME");
  //gPad->SetLogy(1);
  C.Update();

  std::cout << "Press enter to continue";
  std::string temp;
  std::getline(std::cin, temp);
}

int main() {
  Plot();

  return 0;
}
