#pragma once
#include "Analysis.h"
#include "Training.h"

namespace EAL {
namespace Plot {

class NormalizeAndScale {
public:
  auto operator()(Float_t lumi, Float_t mcWeight, Float_t genWeight, Float_t L1PFWeight,
                  Float_t puWeight, Float_t lep1_idEffWeight, Float_t lep1_trigEffWeight,
                  Float_t btagWeight_loose, Float_t process_id, Float_t year) {
    return (((((year==2016)&&(process_id==13))*(1.378))
            +(((year==2016)&&(process_id==15))*(0.895))
            +(((year==2016)&&(process_id!=13)&&(process_id!=15))*1.))
            +((((year==2017)&&(process_id==13))*(1.213))
            +(((year==2017)&&(process_id==15))*(1.036))
            +(((year==2017)&&(process_id!=13)&&(process_id!=15))*1.))
            +((((year==2018)&&(process_id==13))*(0.631))
            +(((year==2018)&&(process_id==15))*(0.697))
            +(((year==2018)&&(process_id!=13)&&(process_id!=15))*1.)))
            *(((year==2016)*(lumi*mcWeight*genWeight*L1PFWeight*puWeight*lep1_idEffWeight*lep1_trigEffWeight))
            +((year==2017)*(lumi*mcWeight*genWeight*L1PFWeight*puWeight*lep1_idEffWeight*lep1_trigEffWeight))
            +((year==2018)*(lumi*mcWeight*genWeight*puWeight*L1PFWeight*lep1_idEffWeight*lep1_trigEffWeight)));
  }
};

class NormalizeData {
public:
  auto operator()(Float_t lumi, Float_t mcWeight, Float_t genWeight, Float_t L1PFWeight,
                  Float_t puWeight, Float_t lep1_idEffWeight, Float_t lep1_trigEffWeight,
                  Float_t btagWeight_loose, int process_id, int year) {
    return (((((year==2016)&&(process_id==13))*(1.378))
            +(((year==2016)&&(process_id==15))*(0.895))
            +(((year==2016)&&(process_id!=13)&&(process_id!=15))*1.))
            +((((year==2017)&&(process_id==13))*(1.213))
            +(((year==2017)&&(process_id==15))*(1.036))
            +(((year==2017)&&(process_id!=13)&&(process_id!=15))*1.))
            +((((year==2018)&&(process_id==13))*(0.631))
            +(((year==2018)&&(process_id==15))*(0.697))
            +(((year==2018)&&(process_id!=13)&&(process_id!=15))*1.)))
            *(((year==2016)*(lumi*mcWeight*genWeight*L1PFWeight*puWeight*lep1_idEffWeight*lep1_trigEffWeight))
            +((year==2017)*(lumi*mcWeight*genWeight*L1PFWeight*puWeight*lep1_idEffWeight*lep1_trigEffWeight))
            +((year==2018)*(lumi*mcWeight*genWeight*puWeight*L1PFWeight*lep1_idEffWeight*lep1_trigEffWeight)));
  }
};

}
}