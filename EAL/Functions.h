#include "Rtypes.h"
#include "TMath.h"
#include <cstdint>

namespace EAL {

void Log(std::string_view message) {
  std::cout << "EAL\t\t:\t" << message << "\n\t\t:\n";
}

void LogFromFunction(std::string_view func, std::string_view message) {
  std::cout << "EAL " << "- " << func << " -\t: " << message << "\n\t\t:\n";
}

void LogLine(std::string_view message) {
  std::cout << "EAL\t\t:\t" << message << "\n";
}

void LogLineFromFunction(std::string_view func, std::string_view message) {
  std::cout << "EAL " << "- " << func << " -\t: " << message << "\n";
}

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

struct CutList {
  EAL::Cut::Tau21 tau21_cut;
  EAL::Cut::QGid qgid_cut;
  EAL::Cut::WVBoosted wv_boosted_cut;
  EAL::Cut::LepPt lep_pt_cut;
  EAL::Cut::LepEta lep_eta_cut;
  EAL::Cut::FatjetPt fatjet_pt_cut;
  EAL::Cut::FatjetEta fatjet_eta_cut;
  EAL::Cut::FatjetTau21 fatjet_tau21_cut;
  EAL::Cut::VBSJetsMjj vbs_jets_mjj_cut;
  EAL::Cut::VBSJetsPt vbs_jets_pt_cut;
  EAL::Cut::VBSDeltaEta vbs_deta_cut;
  EAL::Cut::METpt met_pt_cut;
  EAL::Cut::bTagVeto btag_veto_cut;
  EAL::Cut::Isolation iso_cut;
  EAL::Cut::ZeppLep zepp_lep_cut;
  EAL::Cut::ZeppHad zepp_had_cut;
  EAL::Cut::WVSignalRegion wv_sr_cut;
};
}

namespace Define {
class SetProcessID {
private:
  const std::unordered_map<std::string, Int_t> m_process_IDs;
public:
  SetProcessID(std::unordered_map<std::string, Int_t> IDs) :
    m_process_IDs{IDs} {}

  auto operator()(unsigned int slot, const ROOT::RDF::RSampleInfo& id) {
    int32_t pid = -999;
    for (const auto& process : m_process_IDs) {
      if (id.Contains(process.first)) { pid = process.second; }
    }
    return pid;
  }
};

class SetSampleYear {
private:
  //std::string m_dataframe_class;
  //bool debugging;
public:
  auto operator()(unsigned int slot, const ROOT::RDF::RSampleInfo& id) {
    //if (debugging) std::cout << "Processing " << m_dataframe_class << " dataframe\n--- " << id.AsString() << "\n\n";
    if (id.Contains("2016")) { return 2016; }
    if (id.Contains("2017")) { return 2017; }
    if (id.Contains("2018")) { return 2018; }
    return 0;
  }
};

class SetSampleLumi {
private:
  std::vector<EAL::Sample> m_samples;
  std::unordered_map<std::string, int32_t> m_class_IDs;

public:
  SetSampleLumi(std::vector<EAL::Sample> samples, std::unordered_map<std::string, int32_t> IDs) :
                m_samples{samples}, m_class_IDs{IDs} {}

  auto operator()(unsigned int slot, const ROOT::RDF::RSampleInfo& id) {
    for (const auto& smpl : m_samples) {
      if (id.Contains(smpl.m_file_name) && m_class_IDs.at(smpl.m_file_name) 
        != static_cast<int32_t>(EAL::ClassID::kData)) { 
        return smpl.m_luminosity;
      }
      else if (id.Contains(smpl.m_file_name) && m_class_IDs.at(smpl.m_file_name) 
        == static_cast<int32_t>(EAL::ClassID::kData)) { 
        return 1.0f;
      }
    }
    return -999.0f;
  }
};

class SetSampleWeight {
private:
  std::vector<EAL::Sample> m_samples;
public:
  SetSampleWeight(std::vector<EAL::Sample> samples) : m_samples{samples} {}

  auto operator()(unsigned int slot, const ROOT::RDF::RSampleInfo& id) {
    for (const auto& smpl : m_samples) {
      if (id.Contains(smpl.m_file_name)) { return smpl.m_sample_weight; }
    }
    return -999.0f;
  }
};

class SetSampleClass {
private:
  std::unordered_map<std::string, int32_t> m_class_IDs;
public:
  SetSampleClass(std::unordered_map<std::string, int32_t> class_IDs) : 
    m_class_IDs{class_IDs} {}

  auto operator()(unsigned int slot, const ROOT::RDF::RSampleInfo& id) {
    for (const auto& classes : m_class_IDs) {
      if (id.Contains(classes.first)) { return classes.second; }
    }
    return -999;
  }
};

struct DefinesList {
  EAL::Define::SetProcessID set_process_ids;
  EAL::Define::SetSampleClass set_sample_class;
  EAL::Define::SetSampleYear set_sample_year;
  EAL::Define::SetSampleLumi set_sample_lumi;
  EAL::Define::SetSampleWeight set_sample_weight;

  DefinesList(const std::unordered_map<std::string, Int_t>& process_IDs,
              const std::unordered_map<std::string, int32_t>& class_IDs,
              const std::vector<EAL::Sample>& samples) :
    set_process_ids{process_IDs},
    set_sample_class{class_IDs},
    set_sample_year{},
    set_sample_lumi{samples, class_IDs},
    set_sample_weight{samples} {}
};
}
}
