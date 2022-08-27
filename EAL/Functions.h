#include "Rtypes.h"
#include "TMath.h"
#include <cstdint>

namespace EAL {
namespace Cut {

class WVBoosted {
public:
  auto operator()(Float_t lep2_pt, Float_t bos_PuppiAK8_pt) {
    return (lep2_pt<0.0 && bos_PuppiAK8_pt>0.0);
  }
};

class Tau21 {
public:
  auto operator()(Float_t tau21) {
    return (tau21>=0.0 && tau21<=1.0);
  }
};

class QGid {
public:
  auto operator()(Float_t qgid1, Float_t qgid2) {
    return ((qgid1 >= 0.0 && qgid1 <= 1.0)&&(qgid2 >= 0.0 && qgid2 <= 1.0));
  }
};

class LepPt {
public:
  auto operator()(Float_t lep_pt) {
    return lep_pt>25;
  }
};

class LepEta {
public:
  auto operator()(Float_t lep_m, Float_t lep_eta) {
    return ((lep_m > 0.105 && TMath::Abs(lep_eta) < 2.4) || 
      (lep_m < 0.105 && TMath::Abs(lep_eta < 2.5) && 
      !(TMath::Abs(lep_eta) > 1.4442 && TMath::Abs(lep_eta) < 1.566)));
  }
};

class LepElectron {
public:
  auto operator()(float lep_m, float lep_eta) {
    return (lep_m < 0.105 && TMath::Abs(lep_eta < 2.5) && 
      !(TMath::Abs(lep_eta) > 1.4442 && TMath::Abs(lep_eta) < 1.566));
  }
};

class LepMuon {
public:
  auto operator()(float lep_m, float lep_eta) {
    return (lep_m > 0.105 && TMath::Abs(lep_eta) < 2.4);
  }
};

class FatjetPt {
public:
  auto operator()(float bos_PuppiAK8_pt) {
    return bos_PuppiAK8_pt > 200;
  }
};

class FatjetEta {
public:
  auto operator()(float bos_PuppiAK8_eta) {
    return TMath::Abs(bos_PuppiAK8_eta) < 2.4;
  }
};

class FatjetTau21 {
public:
  auto operator()(float bos_PuppiAK8_tau21) {
    return TMath::Abs(bos_PuppiAK8_tau21) < 0.55;
  }
};

class VBSJetsMjj {
public:
  auto operator()(float vbs_m) {
    return vbs_m > 500;
  }
};

class VBSJetsPt {
public:
  auto operator()(float vbs1_AK4_pt, float vbs2_AK4_pt) {
    return (vbs1_AK4_pt > 50.0 && vbs2_AK4_pt > 50.0);
  }
};

class VBSDeltaEta {
public:
  auto operator()(float vbs_deta) {
    return vbs_deta > 2.5;
  }
};

class METpt {
public:
  auto operator()(float met) {
    return met > 30;
  }
};

class bTagVeto {
public:
  auto operator()(int32_t nBtag_loose) {
    return nBtag_loose == 0;
  }
};

class Isolation {
public:
  auto operator()(int32_t AntiIso, float bosCent) {
    return ((AntiIso == 0) && (bosCent > 0.0));
  }
};

class ZeppLep {
public:
  auto operator()(float zepp_lep, float vbs_deta) {
    return (TMath::Abs(zepp_lep)/vbs_deta) < 1.0;
  }
};

class ZeppHad {
public:
  auto operator()(float zepp_had, float vbs_deta) {
    return (TMath::Abs(zepp_had)/vbs_deta) < 1.0;
  }
};

class NoData {
public:
  auto operator()(int32_t class_id) {
    return !(class_id==1);
  }
};

class WVSignalRegion {
public:
  auto operator()(float bos_PuppiAK8_m) {
    return (bos_PuppiAK8_m > 65 && bos_PuppiAK8_m < 105);
  }
};

class TopControlRegion {
public:
  auto operator()(int32_t nBtag_loose) {
    return nBtag_loose > 0;
  }
};

class WjetsControlRegion {
public:
  auto operator()(float bos_PuppiAK8_m) {
    return ((bos_PuppiAK8_m > 50 && bos_PuppiAK8_m < 65) ||
      (bos_PuppiAK8_m > 105 && bos_PuppiAK8_m < 150));
  }
};

class BDTResponse {
public:
  auto operator()(float BDT_score) {
    return BDT_score > 0.118;
  }
};

class DNNResponse {
public:
  auto operator()(float DNN_score) {
    return DNN_score > 0.092;
  }
};

class DataOnly {
public:
  auto operator()(int32_t class_id) {
    return class_id == static_cast<int32_t>(EAL::ClassID::kData);
  }
};

class SignalOnly {
public:
  auto operator()(int32_t class_id) {
    return class_id == static_cast<int32_t>(EAL::ClassID::kSignal);
  }
};

class BackgroundOnly {
public:
  auto operator()(int32_t class_id) {
    return class_id == static_cast<int32_t>(EAL::ClassID::kBackground);
  }
};
/*
class FullWVSignalRegion {
public:
  EAL::Cut::WVSignalRegion wv_sr;
  EAL::Cut::WVBoosted wv_boosted;
  auto operator()(float bos_PuppiAK8_m, float lep2_pt, float bos_PuppiAK8_pt) {
    return wv_sr(bos_PuppiAK8_m) && wv_boosted(lep2_pt, bos_PuppiAK8_pt);
  }
};
*/
/*
  auto tau21_cut = [](Float_t tau21){ return (tau21 >= 0.0 && tau21 <= 1.0); };
  auto qgid_cut = [](Float_t qgid1, Float_t qgid2){ return ((qgid1 >= 0.0 && qgid1 <= 1.0)&&(qgid2 >= 0.0 && qgid2 <= 1.0)); };
  auto wv_boosted = [](Float_t lep2_pt, Float_t bos_PuppiAK8_pt) { return (lep2_pt<0 && bos_PuppiAK8_pt>0); };
  auto year_2016 = [](int year){ return year==2016; };
  auto year_2017 = [](int year){ return year==2017; };
  auto year_2018 = [](int year){ return year==2018; };
  auto lepton_pt = [](Float_t lep_pt){ return lep_pt>25; };
*/
}
}
