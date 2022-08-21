#include "Rtypes.h"


namespace EAL {
  
class wv_boosted {
public:
  auto operator()(Float_t lep2_pt, Float_t bos_PuppiAK8_pt) {
    return (lep2_pt<0.0 && bos_PuppiAK8_pt>0.0);
  }
};

class tau21_cut {
public:
  auto operator()(Float_t tau21) {
    return (tau21>=0.0 && tau21<=1.0);
  }
};

class qgid_cut {
public:
  auto operator()(Float_t qgid1, Float_t qgid2) {
    return ((qgid1 >= 0.0 && qgid1 <= 1.0)&&(qgid2 >= 0.0 && qgid2 <= 1.0));
  }
};

class lep_pt_cut {
public:
  auto operator()(Float_t lep_pt) {
    return lep_pt>25;
  }
};

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
