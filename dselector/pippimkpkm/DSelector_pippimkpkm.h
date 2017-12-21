#ifndef DSelector_pippimkpkm_h
#define DSelector_pippimkpkm_h

#include <iostream>

#include "DSelector/DSelector.h"
#include "DSelector/DHistogramActions.h"
#include "DSelector/DCutActions.h"

#include "TH1I.h"
#include "TH2I.h"

class DSelector_pippimkpkm : public DSelector
{
	public:

		DSelector_pippimkpkm(TTree* locTree = NULL) : DSelector(locTree){}
		virtual ~DSelector_pippimkpkm(){}

		void Init(TTree *tree);
		Bool_t Process(Long64_t entry);

	private:

		void Get_ComboWrappers(void);
		void Finalize(void);

		// BEAM POLARIZATION INFORMATION
		UInt_t dPreviousRunNumber;
		bool dIsPolarizedFlag; //else is AMO
		bool dIsPARAFlag; //else is PERP or AMO

		//CREATE REACTION-SPECIFIC PARTICLE ARRAYS

		//Step 0
		DParticleComboStep* dStep0Wrapper;
		DBeamParticle* dComboBeamWrapper;
		DChargedTrackHypothesis* dPiPlusWrapper;
		DChargedTrackHypothesis* dPiMinusWrapper;
		DChargedTrackHypothesis* dKPlusWrapper;
		DChargedTrackHypothesis* dKMinusWrapper;
		DChargedTrackHypothesis* dProtonWrapper;

		// DEFINE YOUR HISTOGRAMS HERE
		// EXAMPLES:
		TH1I* dHist_MissingMassSquared;
		TH1I* dHist_BeamEnergy;

		// Monte Carlo Truth masses: Phi(1020), fo(980), Y(2175)
		TH1F* h_PhiMass_Thrown;
		TH1F* h_foMass_Thrown;
		TH1F* h_YMass_Thrown;		

		// P4 & Vertex kinematic fit masses: Phi(1020), fo(980), Y(2175)
		TH1F* h_PhiMass_KinFit;
		TH1F* h_foMass_KinFit;
		TH1F* h_YMass_KinFit;		
		
		// Chi2/NDF KinFit +cut
		TH1F* h_PhiMass_chi2ndfcut;
		TH1F* h_foMass_chi2ndfcut;
		TH1F* h_YMass_chi2ndfcut;

		// Chi2/NDF KinFit -cut
		TH1F* h_PhiMass_chi2ndfcut2;
		TH1F* h_foMass_chi2ndfcut2;
		TH1F* h_YMass_chi2ndfcut2;

		// Phi(1020) mass +cut (<1.06)
		TH1F* h_PhiMass_kmkpcut;
		TH1F* h_foMass_kmkpcut;
		TH1F* h_YMass_kmkpcut;

		// Phi(1020) mass +cut (>1.06)
		TH1F* h_PhiMass_kmkpcut2;
		TH1F* h_foMass_kmkpcut2;
		TH1F* h_YMass_kmkpcut2;

		// kmkp vs pimpip
		TH2F* h2_kmkpvspimpip_KinFit;
		TH2F* h2_kmkpvspimpip_chi2ndfcut;
		TH2F* h2_kmkpvspimpip_kmkpcut;

                // kmkp vs kmkppimpip
                TH2F* h2_kmkpvskmkppimpip_KinFit;
		TH2F* h2_kmkpvskmkppimpip_chi2ndfcut; 
                TH2F* h2_kmkpvskmkppimpip_kmkpcut;

                // pimpip vs kmkppimpip
                TH2F* h2_pimpipvskmkppimpip_KinFit;
		TH2F* h2_pimpipvskmkppimpip_chi2ndfcut; 
                TH2F* h2_pimpipvskmkppimpip_kmkpcut;

		TH1F* hchi2ndf;

	ClassDef(DSelector_pippimkpkm, 0);
};

void DSelector_pippimkpkm::Get_ComboWrappers(void)
{
	//Step 0
	dStep0Wrapper = dComboWrapper->Get_ParticleComboStep(0);
	dComboBeamWrapper = static_cast<DBeamParticle*>(dStep0Wrapper->Get_InitialParticle());
	dPiPlusWrapper = static_cast<DChargedTrackHypothesis*>(dStep0Wrapper->Get_FinalParticle(0));
	dPiMinusWrapper = static_cast<DChargedTrackHypothesis*>(dStep0Wrapper->Get_FinalParticle(1));
	dKPlusWrapper = static_cast<DChargedTrackHypothesis*>(dStep0Wrapper->Get_FinalParticle(2));
	dKMinusWrapper = static_cast<DChargedTrackHypothesis*>(dStep0Wrapper->Get_FinalParticle(3));
	dProtonWrapper = static_cast<DChargedTrackHypothesis*>(dStep0Wrapper->Get_FinalParticle(4));
}

#endif // DSelector_pippimkpkm_h
