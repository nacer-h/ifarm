// $Id$
//
//    File: JEventProcessor_DCdEdxStudy_tree.cc
// Created: Tue Dec 19 09:24:17 EST 2017
// Creator: nacer (on Linux ifarm1401.jlab.org 3.10.0-327.el7.x86_64 x86_64)
//

#include "JEventProcessor_DCdEdxStudy_tree.h"
using namespace jana;

// The executable should define the ROOTfile global variable. It will
// be automatically linked when dlopen is called.
extern TFile *ROOTfile;

// Routine used to create our JEventProcessor
#include <JANA/JApplication.h>
#include <JANA/JFactory.h>
extern "C"{
void InitPlugin(JApplication *app){
	InitJANAPlugin(app);
	app->AddProcessor(new JEventProcessor_DCdEdxStudy_tree());
}
} // "C"


//------------------
// JEventProcessor_DCdEdxStudy_tree (Constructor)
//------------------
JEventProcessor_DCdEdxStudy_tree::JEventProcessor_DCdEdxStudy_tree()
{

}

//------------------
// ~JEventProcessor_DCdEdxStudy_tree (Destructor)
//------------------
JEventProcessor_DCdEdxStudy_tree::~JEventProcessor_DCdEdxStudy_tree()
{

}

//------------------
// init
//------------------
jerror_t JEventProcessor_DCdEdxStudy_tree::init(void)
{
	// This is called once at program startup. 
        dDCdEdxInformation = new DCdEdxInformation();
	dPluginTree_DCdEdxInformation = new TTree("dPluginTree_DCdEdxInformation", "DC dEdx Information");
	dPluginTree_DCdEdxInformation->Branch("dPluginBranch_DCdEdxInformation", "DCdEdxInformation", &dDCdEdxInformation);
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t JEventProcessor_DCdEdxStudy_tree::brun(JEventLoop *eventLoop, int32_t runnumber)
{
	// This is called whenever the run number changes

        // Get the particle ID algorithms
       vector<const DParticleID *> locPIDAlgorithms;
        eventLoop->Get(locPIDAlgorithms);
	if(locPIDAlgorithms.size() < 1){
	  _DBG_<<"Unable to get a DParticleID object! NO PID will be done!"<<endl;
	  return RESOURCE_UNAVAILABLE;
	}
	// Drop the const qualifier from the DParticleID pointer (I'm surely going to hell for this!)
	LockState();
	dPIDAlgorithm = const_cast<DParticleID*>(locPIDAlgorithms[0]);
	UnlockState();
	
	// Warn user if something happened that caused us NOT to get a dPIDAlgorithm object pointer
	if(!dPIDAlgorithm){
	  _DBG_<<"Unable to get a DParticleID object! NO PID will be done!"<<endl;
	  return RESOURCE_UNAVAILABLE;
	}
	
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t JEventProcessor_DCdEdxStudy_tree::evnt(JEventLoop *loop, uint64_t eventnumber)
{

        vector<const DMCThrown*> locDMCThrownVector;
	const DMCThrown *locDMCThrown;
	loop->Get(locDMCThrownVector);
	
	vector<const DChargedTrackHypothesis*> locChargedTrackHypothesisVector;
	const DChargedTrackHypothesis *locChargedTrackHypothesis;
	loop->Get(locChargedTrackHypothesisVector);

	const DTrackTimeBased *locTrackTimeBased;
	
	Particle_t locThrownPID, locReconstructedPID;
	float locThrownMass, locMomentum, locBeta;
	
	if(locDMCThrownVector.size() != 1)
	  return NOERROR; //routine assumes only one thrown track (matching not performed!)
	if(locChargedTrackHypothesisVector.size() == 0)
	  return NOERROR;
	locDMCThrown = locDMCThrownVector[0];
	locThrownPID = Particle_t(locDMCThrown->type);
	locThrownMass = ParticleMass(locThrownPID);
	
	bool locPIDMatchFlag = false;
	for(unsigned int loc_i = 0; loc_i < locChargedTrackHypothesisVector.size(); loc_i++){
	  locChargedTrackHypothesis = locChargedTrackHypothesisVector[loc_i];
	  locTrackTimeBased = locChargedTrackHypothesis->Get_TrackTimeBased();
	  locReconstructedPID = locChargedTrackHypothesis->PID();
	  if(locReconstructedPID == locThrownPID){
	    locPIDMatchFlag = true;
	    break;
	  }
	}
	if(locPIDMatchFlag == false)
	  locTrackTimeBased = locChargedTrackHypothesisVector[0]->Get_TrackTimeBased();
	
	locMomentum = locTrackTimeBased->momentum().Mag();
	locBeta = locMomentum/sqrt(locThrownMass*locThrownMass + locMomentum*locMomentum);
	
	// Although we are only filling objects local to this plugin, TTree::Fill() periodically writes to file: Global ROOT lock
	japp->RootWriteLock(); //ACQUIRE ROOT LOCK
	
	//low-momentum & beta protons are reconstructed as low-momentum but high-beta pions
	dDCdEdxInformation->dBeta = locBeta;
	dDCdEdxInformation->dMomentum = locMomentum;
	dDCdEdxInformation->dTheta = locTrackTimeBased->momentum().Theta();
	dDCdEdxInformation->dVertexZ = locTrackTimeBased->z();
	
	dDCdEdxInformation->ddEdx_FDC = locTrackTimeBased->ddEdx_FDC; //units of GeV/cm
	dDCdEdxInformation->ddx_FDC = locTrackTimeBased->ddx_FDC;
	dDCdEdxInformation->dNumHitsUsedFordEdx_FDC = locTrackTimeBased->dNumHitsUsedFordEdx_FDC;
	dDCdEdxInformation->ddEdx_CDC = locTrackTimeBased->ddEdx_CDC; //units of GeV/cm
	dDCdEdxInformation->ddx_CDC = locTrackTimeBased->ddx_CDC;
	dDCdEdxInformation->dNumHitsUsedFordEdx_CDC = locTrackTimeBased->dNumHitsUsedFordEdx_CDC;
	
	dDCdEdxInformation->dChiSq_DCdEdx = locChargedTrackHypothesis->Get_ChiSq_DCdEdx();
	dDCdEdxInformation->dNDF_DCdEdx = locChargedTrackHypothesis->Get_NDF_DCdEdx();
	dDCdEdxInformation->dFOM = TMath::Prob(dDCdEdxInformation->dChiSq_DCdEdx, dDCdEdxInformation->dNDF_DCdEdx);
	dPluginTree_DCdEdxInformation->Fill();
	
	japp->RootUnLock(); //RELEASE ROOT LOCK
	

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t JEventProcessor_DCdEdxStudy_tree::erun(void)
{
	// This is called whenever the run number changes, before it is
	// changed to give you a chance to clean up before processing
	// events from the next run number.
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t JEventProcessor_DCdEdxStudy_tree::fini(void)
{
	// Called before program exit after event processing is finished.
	return NOERROR;
}

