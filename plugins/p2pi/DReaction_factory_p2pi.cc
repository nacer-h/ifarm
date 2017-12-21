// $Id$
//
//    File: DReaction_factory_p2pi.cc
// Created: Mon May 22 07:47:13 EDT 2017
// Creator: nacer (on Linux ifarm1402.jlab.org 3.10.0-327.el7.x86_64 x86_64)
//

#include "DCustomAction_pid.h"
#include "DReaction_factory_p2pi.h"

//------------------
// brun
//------------------
jerror_t DReaction_factory_p2pi::brun(JEventLoop* locEventLoop, int32_t locRunNumber)
{
	vector<double> locBeamPeriodVector;
	locEventLoop->GetCalib("PHOTON_BEAM/RF/beam_period", locBeamPeriodVector);
	dBeamBunchPeriod = locBeamPeriodVector[0];

	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t DReaction_factory_p2pi::evnt(JEventLoop* locEventLoop, uint64_t locEventNumber)
{
	// Make as many DReaction objects as desired
	DReactionStep* locReactionStep = NULL;
	DReaction* locReaction = NULL; //create with a unique name for each DReaction object. CANNOT (!) be "Thrown"

	// DOCUMENTATION:
	// ANALYSIS library: https://halldweb1.jlab.org/wiki/index.php/GlueX_Analysis_Software
	// DReaction factory: https://halldweb1.jlab.org/wiki/index.php/Analysis_DReaction

	/************************************************** p2pi Reaction Definition *************************************************/

	locReaction = new DReaction("p2pi");

	 
	//Required: DReactionSteps to specify the channel and decay chain you want to study
		//Particles are of type Particle_t, an enum defined in sim-recon/src/libraries/include/particleType.h

	//Example: g, p -> pi+, pi-, p
	locReactionStep = new DReactionStep();
	locReactionStep->Set_InitialParticleID(Gamma);
	locReactionStep->Set_TargetParticleID(Proton);
	locReactionStep->Add_FinalParticleID(PiPlus);
	locReactionStep->Add_FinalParticleID(PiMinus);
	locReactionStep->Add_FinalParticleID(Proton);
	locReaction->Add_ReactionStep(locReactionStep);
	dReactionStepPool.push_back(locReactionStep); //register so will be deleted later: prevent memory leak

	/**************************************************** p2pi Control Settings ****************************************************/

	// Highly Recommended: Set EventStore skim query (use with "eventstore" source)
		// This will skip creating particle combos for events that aren't in the skims you list
		// Query should be comma-separated list of skims to boolean-AND together
	//locReaction->Set_EventStoreSkims("myskim1,myskim2,myskim3"); //boolean-AND of skims

	// Recommended: Type of kinematic fit to perform (default is d_NoFit)
		//fit types are of type DKinFitType, an enum defined in sim-recon/src/libraries/ANALYSIS/DReaction.h
		//Options: d_NoFit (default), d_P4Fit, d_VertexFit, d_P4AndVertexFit
		//P4 fits automatically constrain decaying particle masses, unless they are manually disabled
	locReaction->Set_KinFitType(d_P4AndVertexFit);

	// Highly Recommended: When generating particle combinations, reject all beam photons that match to a different RF bunch
	locReaction->Set_MaxPhotonRFDeltaT(0.5*dBeamBunchPeriod); //should be minimum cut value

	// Optional: When generating particle combinations, reject all photon candidates with a PID confidence level < 5.73303E-7 (+/- 5-sigma)
	// Make sure PID errors are calculated correctly before using. 
	//locReaction->Set_MinPhotonPIDFOM(5.73303E-7);

	// Optional: When generating particle combinations, reject all charged track candidates with a PID confidence level < 5.73303E-7 (+/- 5-sigma)
	// Make sure PID errors are calculated correctly before using. 
	//locReaction->Set_MinChargedPIDFOM(5.73303E-7);

	// Highly Recommended: Cut on number of extra "good" tracks. "Good" tracks are ones that survive the "PreSelect" (or user custom) factory.
		// Important: Keep cut large: Can have many ghost and accidental tracks that look "good"
	locReaction->Set_MaxExtraGoodTracks(3);

	// Highly Recommended: Enable ROOT TTree output for this DReaction
	// string is file name (must end in ".root"!!): doen't need to be unique, feel free to change
	locReaction->Enable_TTreeOutput("tree_p2pi.root", false); //true/false: do/don't save unused hypotheses

	/************************************************** p2pi Pre-Combo Custom Cuts *************************************************/

	// Highly Recommended: Very loose invariant mass cuts, applied during DParticleComboBlueprint construction
	// Example: Rho -> pi+, pi- cut
	//locReaction->Set_InvariantMassCut(rho, 0.6, 0.9);

	// Highly Recommended: Very loose DAnalysisAction cuts, applied just after creating the combination (before saving it)
	// Example: Missing mass of proton
	// locReaction->Add_ComboPreSelectionAction(new DCutAction_MissingMassSquared(locReaction, false, -0.005, 0.005));

	/**************************************************** p2pi Analysis Actions ****************************************************/

	
	// Recommended: Analysis actions automatically performed by the DAnalysisResults factories to histogram useful quantities.
		//These actions are executed sequentially, and are executed on each surviving (non-cut) particle combination 
		//Pre-defined actions can be found in ANALYSIS/DHistogramActions_*.h and ANALYSIS/DCutActions.h
		//If a histogram action is repeated, it should be created with a unique name (string) to distinguish them

	//MASSES: Rho , Deltapp, Deltao 
	deque<Particle_t> locRhoPIDs;  locRhoPIDs.push_back(PiPlus);  locRhoPIDs.push_back(PiMinus);
	locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locRhoPIDs, false, 900, 0.3, 1.2, "Rho_PreCut"));

	// deque<Particle_t> locDeltappPIDs;  locDeltappPIDs.push_back(Proton);  locDeltappPIDs.push_back(PiPlus);
	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltappPIDs, false, 900, 1.05, 1.6, "Deltapp_PreCut"));
	// deque<Particle_t> locDeltaoPIDs;  locDeltaoPIDs.push_back(Proton);  locDeltaoPIDs.push_back(PiMinus);
	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltaoPIDs, false, 900, 1.05, 1.6, "Deltao_PreCut"));

	// // 2D masses
	// locReaction->Add_AnalysisAction(new DHistogramAction_2DInvariantMass(locReaction, 0, locRhoPIDs, locDeltappPIDs, false, 600,0.3,1.2,600,1.05,1.6,"RhovsDeltapp_PreCut"));
	// locReaction->Add_AnalysisAction(new DHistogramAction_2DInvariantMass(locReaction, 0, locRhoPIDs, locDeltaoPIDs, false, 600,0.3,1.2, 600,1.05,1.6, "RhovsDeltao_PreCut"));
	
	// HISTOGRAM PID
	locReaction->Add_AnalysisAction(new DHistogramAction_PID(locReaction));

	locReaction->Add_AnalysisAction(new DCutAction_PIDDeltaT(locReaction, false, 0.5, Proton, SYS_TOF));
	locReaction->Add_AnalysisAction(new DCutAction_PIDDeltaT(locReaction, false, 1.0, Proton, SYS_BCAL));
	locReaction->Add_AnalysisAction(new DCutAction_PIDDeltaT(locReaction, false, 3.0, Proton, SYS_FCAL));
	locReaction->Add_AnalysisAction(new DCutAction_PIDDeltaT(locReaction, false, 0.5, PiPlus, SYS_TOF));
	locReaction->Add_AnalysisAction(new DCutAction_PIDDeltaT(locReaction, false, 0.8, PiPlus, SYS_BCAL));
	locReaction->Add_AnalysisAction(new DCutAction_PIDDeltaT(locReaction, false, 3.0, PiPlus, SYS_FCAL));
	locReaction->Add_AnalysisAction(new DCutAction_PIDDeltaT(locReaction, false, 0.5, PiMinus, SYS_TOF));
	locReaction->Add_AnalysisAction(new DCutAction_PIDDeltaT(locReaction, false, 0.8, PiMinus, SYS_BCAL));
	locReaction->Add_AnalysisAction(new DCutAction_PIDDeltaT(locReaction, false, 3.0, PiMinus, SYS_FCAL));
	// locReaction->Add_AnalysisAction(new DCustomAction_dEdxCut_p2pi(locReaction, false)); //false: focus on keeping signal
	locReaction->Add_AnalysisAction(new DHistogramAction_PID(locReaction, "PostPIDCuts"));

	//Kinematics Pre-Phi cut
	locReaction->Add_AnalysisAction(new DHistogramAction_ParticleComboKinematics(locReaction, false));

	//MASSES post time cuts
	locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locRhoPIDs, false, 900, 0.3, 1.2, "Rho_TCuts"));
	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltappPIDs, false, 900, 1.05, 1.6, "Deltapp_TCuts"));
	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltaoPIDs, false, 900, 1.05, 1.6, "Deltao_TCuts"));

	//Missing Mass Squared
	locReaction->Add_AnalysisAction(new DHistogramAction_MissingMassSquared(locReaction, false, 600, -1, 1,"meas"));
	locReaction->Add_AnalysisAction(new DCutAction_MissingMassSquared(locReaction, false, -0.005, 0.005));

	locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locRhoPIDs, false, 900, 0.3, 1.2, "Rho_MM2Cut_meas"));
	locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locRhoPIDs, true, 900, 0.3, 1.2, "Rho_MM2Cut"));

	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltappPIDs, false, 900, 1.05, 1.6, "Deltapp_MM2Cut_meas"));
	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltappPIDs, true, 900, 1.05, 1.6, "Deltapp_MM2Cut"));
	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltaoPIDs, false, 900, 1.05, 1.6, "Deltao_MM2Cut_meas"));
	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltaoPIDs, true, 900, 1.05, 1.6, "Deltao_MM2Cut"));

	// Kinematic Fit Results
	locReaction->Add_AnalysisAction(new DHistogramAction_KinFitResults(locReaction, 0.05, true)); //5% confidence level cut on pull histograms only
	locReaction->Add_AnalysisAction(new DCutAction_KinFitFOM(locReaction, 5.73303E-7)); // confidence level cut //+/- 5 sigma

	// MASSES, POST-KINFIT
	locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locRhoPIDs, false, 900, 0.3, 1.2, "Rho_PostKinFitCut_meas"));
	locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locRhoPIDs, true, 900, 0.3, 1.2, "Rho_PostKinFitCut"));

	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltappPIDs, false, 900, 1.05, 1.6, "Deltapp_PostKinFitCut_meas"));
	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltappPIDs, true, 900, 1.05, 1.6, "Deltapp_PostKinFitCut"));
	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltaoPIDs, false, 900, 1.05, 1.6, "Deltao_PostKinFitCut_meas"));
	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltaoPIDs, true, 900, 1.05, 1.6, "Deltao_PostKinFitCut"));


   // locReaction->Add_AnalysisAction(new DHistogramAction_2DInvariantMass(locReaction, 0, locRhoPIDs, locDeltappPIDs, true,600,0.3,1.2,600,1.05,1.6, "RhovsDeltapp_PostKinFitCut"));
   // locReaction->Add_AnalysisAction(new DHistogramAction_2DInvariantMass(locReaction, 0, locRhoPIDs, locDeltaoPIDs, true, 600,0.3,1.2,600,1.05,1.6, "RhovsDeltao_PostKinFitCut"));

	// Rho Invariant mass cut
	locReaction->Add_AnalysisAction(new DCutAction_InvariantMass(locReaction, 0, locRhoPIDs, false, 0.6, 0.9, "RhoMassCut"));
	locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locRhoPIDs, false, 900, 0.6, 0.9, "Rho_PostRhoMassCut_meas"));
	locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locRhoPIDs, true, 900, 0.6, 0.9, "Rho_PostRhoMassCut"));

	// Delta's Invariant mass cut
	// locReaction->Add_AnalysisAction(new DCutAction_InvariantMass(locReaction, 0, locDeltappPIDs, false, 1.1, 1.3, "DeltappMassCut"));
	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltappPIDs, false, 900, 1.1, 1.3, "Deltapp_PostDeltappCut_meas"));
	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltappPIDs, true, 900, 1.1, 1.3, "Deltapp_PostDeltappCut"));
	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltaoPIDs, false, 900, 1.1, 1.3, "Deltao_PostDeltappCut_meas"));
	// locReaction->Add_AnalysisAction(new DHistogramAction_InvariantMass(locReaction, 0, locDeltaoPIDs, true, 900, 1.1, 1.3, "Deltao_PostDeltappCut"));

	// Kinematics
	locReaction->Add_AnalysisAction(new DHistogramAction_ParticleComboKinematics(locReaction, false, "final_meas"));
	// locReaction->Add_AnalysisAction(new DHistogramAction_ParticleComboKinematics(locReaction, true, "final_KinFit"));
	locReaction->Add_AnalysisAction(new DHistogramAction_TrackVertexComparison(locReaction));
	
	// dE/dx PID hit-level information
	locReaction->Add_AnalysisAction(new DCustomAction_pid(locReaction, true));
	
	_data.push_back(locReaction); //Register the DReaction with the factory

	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t DReaction_factory_p2pi::fini(void)
{
	for(size_t loc_i = 0; loc_i < dReactionStepPool.size(); ++loc_i)
		delete dReactionStepPool[loc_i]; //cleanup memory
	return NOERROR;
}
