// $Id$
//
//    File: DCustomAction_pid.h
// Created: Mon May 22 08:33:35 EDT 2017
// Creator: nacer (on Linux ifarm1402.jlab.org 3.10.0-327.el7.x86_64 x86_64)
//

#ifndef _DCustomAction_pid_
#define _DCustomAction_pid_

#include <string>
#include <iostream>

#include "JANA/JEventLoop.h"
#include "JANA/JApplication.h"

#include "ANALYSIS/DAnalysisAction.h"
#include "ANALYSIS/DReaction.h"
#include "ANALYSIS/DParticleCombo.h"
#include "ANALYSIS/DAnalysisUtilities.h"

#include <JANA/JEventProcessor.h>
#include <JANA/jerror.h>
#include <deque>
#include <map>
#include <limits>
#include <cmath>
#include <set>
#include <thread>
#include <string>

#include <JANA/JObject.h>
#include <JANA/JFactory.h>

#include <DVector3.h>
#include <TRACKING/DTrackTimeBased_factory.h>
#include <CDC/DCDCTrackHit.h>
#include <FDC/DFDCPseudo.h>
#include <BCAL/DBCALShower.h>
#include <BCAL/DBCALCluster.h>
#include <FCAL/DFCALShower.h>
#include <FCAL/DFCALGeometry_factory.h>
#include <TOF/DTOFPoint.h>
#include <TOF/DTOFPaddleHit.h>
#include <TOF/DTOFGeometry_factory.h>
#include <TOF/DTOFPoint_factory.h>
#include <START_COUNTER/DSCHit.h>
#include <TRACKING/DTrackFitter.h>
#include <TRACKING/DTrackFinder.h>
#include <TRACKING/DReferenceTrajectory.h>
#include <particleType.h>
#include <PID/DChargedTrack.h>
#include <PID/DChargedTrackHypothesis.h>
#include <PID/DNeutralParticleHypothesis.h>
#include <PID/DEventRFBunch.h>
#include <PID/DDetectorMatches.h>
#include <PID/DParticleID.h>
#include <TRACKING/DMagneticFieldStepper.h>
#include <TRACKING/DTrackWireBased.h>
#include <TRACKING/DTrackCandidate.h>
#include "ANALYSIS/DTreeInterface.h"

#include <TMath.h>
#include <TTree.h>
#include <TH1.h>
#include <iostream>
#include <vector>


using namespace std;
using namespace jana;

#define NHITMAX 1000

class DCustomAction_pid : public DAnalysisAction
{
	public:

		DCustomAction_pid(const DReaction* locReaction, bool locUseKinFitResultsFlag, string locActionUniqueString = "") : 
		DAnalysisAction(locReaction, "Custom_pid", locUseKinFitResultsFlag, locActionUniqueString)
		  {
		    dTreeInterface_proton = NULL;
		    dTreeInterface_pip = NULL;
		    dTreeInterface_pim = NULL;
		  }

		void Initialize(JEventLoop* locEventLoop);
		void Reset_NewEvent(void){}; //RESET HISTOGRAM DUPLICATE-CHECK TRACKING HERE!!

		~DCustomAction_pid(void)
		  {
		    if(dTreeInterface_proton != NULL)
		      delete dTreeInterface_proton; //SAVES FILES, TREES
		    if(dTreeInterface_pip != NULL)
		      delete dTreeInterface_pip; //SAVES FILES, TREES
		    if(dTreeInterface_pim != NULL)
		      delete dTreeInterface_pim; //SAVES FILES, TREES
		  }
		
	private:

		bool Perform_Action(JEventLoop* locEventLoop, const DParticleCombo* locParticleCombo);
		//Store any histograms as member variables here

		const DAnalysisUtilities* dAnalysisUtilities;
		const DParticleID* dParticleID;

		DTreeFillData dTreeFillData;
		
		DTreeInterface* dTreeInterface_proton;
		DTreeInterface* dTreeInterface_pip;
		DTreeInterface* dTreeInterface_pim;		
};

#endif // _DCustomAction_pid_

