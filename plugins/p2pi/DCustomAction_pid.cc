// $Id$
//
//    File: DCustomAction_pid.cc
// Created: Mon May 22 08:33:35 EDT 2017
// Creator: nacer (on Linux ifarm1402.jlab.org 3.10.0-327.el7.x86_64 x86_64)
//

#include "DCustomAction_pid.h"

void DCustomAction_pid::Initialize(JEventLoop* locEventLoop)
{
  locEventLoop->GetSingle(dAnalysisUtilities);
  locEventLoop->GetSingle(dParticleID);
  
  CreateAndChangeTo_ActionDirectory();
  // const DReaction* locReaction = Get_Reaction();
  
  //CREATE TTREE, TFILE
  dTreeInterface_proton = DTreeInterface::Create_DTreeInterface("pid", "tree_p2pi_proton.root");
  dTreeInterface_pip = DTreeInterface::Create_DTreeInterface("pid", "tree_p2pi_pip.root");
  dTreeInterface_pim = DTreeInterface::Create_DTreeInterface("pid", "tree_p2pi_pim.root");

  //TTREE BRANCHES
  DTreeBranchRegister locTreeBranchRegister;
    
  //CHANNEL INFO
  locTreeBranchRegister.Register_Single<Int_t>("evnum");  // event number
  locTreeBranchRegister.Register_Single<Int_t>("runnum"); // run number
  locTreeBranchRegister.Register_Single<Int_t>("tracknum"); // track number
  locTreeBranchRegister.Register_Single<Int_t>("nhits"); // number of hits in the CDC
  locTreeBranchRegister.Register_Single<Int_t>("ntracks"); // number of tracks in the CDC
  locTreeBranchRegister.Register_FundamentalArray<Double_t>("de","nhits",NHITMAX); // energy for every hit
  locTreeBranchRegister.Register_FundamentalArray<Double_t>("dx","nhits",NHITMAX); // position for every hit
  locTreeBranchRegister.Register_FundamentalArray<Double_t>("dedx","nhits",NHITMAX); // energy loss for every hit
  locTreeBranchRegister.Register_Single<Double_t>("dedxav"); // the average dE/dx of hits in CDC
  locTreeBranchRegister.Register_Single<Double_t>("p"); // the hit momentum in CDC
  locTreeBranchRegister.Register_Single<Double_t>("theta"); // the hit polar angle in degrees in CDC
  locTreeBranchRegister.Register_Single<Double_t>("costheta"); // the hit polar angle in rads in CDC
  locTreeBranchRegister.Register_Single<Double_t>("phi"); // the hit azimuthal angle in degrees in CDC

  //REGISTER BRANCHES
  dTreeInterface_proton->Create_Branches(locTreeBranchRegister);
  dTreeInterface_pip->Create_Branches(locTreeBranchRegister);
  dTreeInterface_pim->Create_Branches(locTreeBranchRegister);
  
}

bool DCustomAction_pid::Perform_Action(JEventLoop* locEventLoop, const DParticleCombo* locParticleCombo)
{

  double locdEdx_CDC, locdx_CDC;
  unsigned int locNumHitsUsedFordEdx_CDC;
  int nhits;     
     
  vector<const DParticleID *> dParticleID_algos;
  locEventLoop->Get(dParticleID_algos);
  const DParticleID *dParticleID = NULL;
  if(dParticleID_algos.size() < 1)
    {
      _DBG_<<"Unable to get a DParticleID object! NO PID will be done!"<<endl;
      return RESOURCE_UNAVAILABLE;
    } 
  dParticleID = dParticleID_algos[0];
      
  vector<const DChargedTrack*> locChargedTrack;
  locEventLoop->Get(locChargedTrack);
  if (locChargedTrack.size()==0) return NOERROR;  

  // reject tracks that are poorly reconstructed (bad tracking FOM)
  const DTrackTimeBased* locTrackTimeBased = NULL;
  if(locTrackTimeBased == NULL) return NOERROR;
  if(locTrackTimeBased->FOM < 5.73303E-7) return NOERROR;   // 5 sigma cut
  
  for(unsigned int loc_k = 0; loc_k < locChargedTrack.size(); ++loc_k)
    {
     dTreeFillData.Fill_Single<Int_t>("tracknum", loc_k);
    }

  dTreeFillData.Fill_Single<Int_t>("ntracks", locChargedTrack.size());
  dTreeFillData.Fill_Single<Int_t>("runnum", locEventLoop->GetJEvent().GetRunNumber()); // get run number
  dTreeFillData.Fill_Single<Int_t>("evnum", locEventLoop->GetJEvent().GetEventNumber()); // get event number

  // one reaction step
  const DParticleComboStep* locParticleComboStep = locParticleCombo->Get_ParticleComboStep(0);
  deque<const DKinematicData*> locParticles;
  // locParticleComboStep->Get_FinalParticles_Measured(locParticles);
  
  for(unsigned int loc_j=0; loc_j < locParticles.size(); ++loc_j)
    {

      // const DChargedTrackHypothesis* locChargedTrackHypothesis = static_cast<const DChargedTrackHypothesis*>(locParticles[loc_j]);
      const DChargedTrack* locChargedTrack = static_cast<const DChargedTrack*>(locParticleComboStep->Get_FinalParticle_SourceObject(loc_j));
      const DChargedTrackHypothesis* locChargedTrackHypothesis = static_cast<const DChargedTrackHypothesis*>(locParticleComboStep->Get_FinalParticle_SourceObject(loc_j));
      locChargedTrackHypothesis->GetSingle(locTrackTimeBased); 
      locChargedTrackHypothesis = locChargedTrack->Get_BestFOM();             
    
      dTreeFillData.Fill_Single<Double_t>("p", locChargedTrackHypothesis->momentum().Mag());
      dTreeFillData.Fill_Single<Double_t>("theta", locChargedTrackHypothesis->momentum().Theta()*180./3.14159);
      dTreeFillData.Fill_Single<Double_t>("costheta", locChargedTrackHypothesis->momentum().CosTheta());
      dTreeFillData.Fill_Single<Double_t>("phi", locChargedTrackHypothesis->momentum().Phi()*180./3.14159);
      
      vector<DParticleID::dedx_t> locdEdxHits_CDC, locdEdxHits_FDC;
      
      jerror_t locReturnStatus = dParticleID->GetDCdEdxHits(locTrackTimeBased, locdEdxHits_CDC, locdEdxHits_FDC);
      
      if(locReturnStatus != NOERROR)
	{
	  locdEdx_CDC = numeric_limits<double>::quiet_NaN();
	  locdx_CDC = numeric_limits<double>::quiet_NaN();
	  locNumHitsUsedFordEdx_CDC = 0;
	  return locReturnStatus;
	}
      
      locdx_CDC = 0.0;
      locdEdx_CDC = 0.0;
      locNumHitsUsedFordEdx_CDC = locdEdxHits_CDC.size();
	  
      nhits = locNumHitsUsedFordEdx_CDC;
      dTreeFillData.Fill_Single<Int_t>("nhits", locNumHitsUsedFordEdx_CDC);
      
      if (nhits>NHITMAX) nhits = NHITMAX;
      
      if(locNumHitsUsedFordEdx_CDC > 0)
	{
	  for(unsigned int loc_i = 0; loc_i < locNumHitsUsedFordEdx_CDC; ++loc_i)
	    {
	      locdEdx_CDC += locdEdxHits_CDC[loc_i].dE; //weight is ~ #e- (scattering sites): dx!
	      locdx_CDC += locdEdxHits_CDC[loc_i].dx;	     
	      if (loc_i<NHITMAX)
		{
		  dTreeFillData.Fill_Array<Double_t>("de", locdEdxHits_CDC[loc_i].dE,loc_i);
		  dTreeFillData.Fill_Array<Double_t>("dx", locdEdxHits_CDC[loc_i].dx,loc_i);
		  dTreeFillData.Fill_Array<Double_t>("dedx", locdEdxHits_CDC[loc_i].dEdx,loc_i);
		}
	    }
	  locdEdx_CDC /= locdx_CDC;
	  dTreeFillData.Fill_Single<Double_t>("dedxav", locdEdx_CDC);
	}
      
      //FILL TTREE
      if(loc_j==0)
	dTreeInterface_pip->Fill(dTreeFillData);
      if(loc_j==1)
	dTreeInterface_pim->Fill(dTreeFillData);
      if(loc_j==2)
	dTreeInterface_proton->Fill(dTreeFillData);
      
    }
  return true; //return false if you want to use this action to apply a cut (and it fails the cut!)
}
