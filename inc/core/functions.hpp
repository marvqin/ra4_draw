#ifndef H_FUNCTIONS
#define H_FUNCTIONS

#include <cstddef>

#include "core/named_func.hpp"
 
namespace Functions{
  extern const NamedFunc n_isr_match;
  extern const NamedFunc njets_weights_ttisr;
  extern const NamedFunc njets_weights_visr;
  extern const NamedFunc min_dphi_lep_met;
  extern const NamedFunc max_dphi_lep_met;
  extern const NamedFunc min_dphi_lep_jet;
  extern const NamedFunc max_dphi_lep_jet;
  extern const NamedFunc min_dphi_met_jet;
  extern const NamedFunc max_dphi_met_jet;
  extern const NamedFunc min_dr_lep_jet;
  extern const NamedFunc max_dr_lep_jet;
  extern const NamedFunc mct;
  extern const NamedFunc b_pt;
  extern const NamedFunc m_jj;
  extern const NamedFunc m_bb;

  bool IsGoodJet(const Baby &b, std::size_t ijet);
  bool IsGoodElectron(const Baby &b, std::size_t iel);
  bool IsGoodMuon(const Baby &b, std::size_t imu);
  bool IsGoodTrack(const Baby &b, std::size_t itk);

  NamedFunc::ScalarType NJetsWeights_ttISR(const Baby &b, bool use_baby_nisr);
  NamedFunc::ScalarType NJetsWeights_vISR(const Baby &b);
  int NISRMatch(const Baby &b);

  void DileptonAngles(const Baby &b,
		      NamedFunc::ScalarType &eta1, NamedFunc::ScalarType &phi1,
		      NamedFunc::ScalarType &eta2, NamedFunc::ScalarType &phi2);
}

#endif
