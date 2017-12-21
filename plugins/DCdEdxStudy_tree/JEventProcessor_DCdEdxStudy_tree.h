// $Id$
//
//    File: JEventProcessor_DCdEdxStudy_tree.h
// Created: Tue Dec 19 09:24:17 EST 2017
// Creator: nacer (on Linux ifarm1401.jlab.org 3.10.0-327.el7.x86_64 x86_64)
//

#ifndef _JEventProcessor_DCdEdxStudy_tree_
#define _JEventProcessor_DCdEdxStudy_tree_

#include <JANA/JEventProcessor.h>
using namespace jana;

#include <TFile.h>
#include <TTree.h>
#include <DVector3.h>
#include <particleType.h>

#include <DANA/DApplication.h>
#include <TRACKING/DMCThrown.h>
#include <TRACKING/DTrackTimeBased.h>
#include <PID/DParticleID.h>
#include <DCdEdxInformation.h>

class JEventProcessor_DCdEdxStudy_tree:public jana::JEventProcessor{
	public:
		JEventProcessor_DCdEdxStudy_tree();
		~JEventProcessor_DCdEdxStudy_tree();
		const char* className(void){return "JEventProcessor_DCdEdxStudy_tree";}

	private:
		jerror_t init(void);						///< Called once at program start.
		jerror_t brun(jana::JEventLoop *eventLoop, int32_t runnumber);	///< Called everytime a new run number is detected.
		jerror_t evnt(jana::JEventLoop *eventLoop, uint64_t eventnumber);	///< Called every event.
		jerror_t erun(void);						///< Called everytime run number changes, provided brun has been called.
		jerror_t fini(void);						///< Called after last event of last event source has been processed.

		DCdEdxInformation *dDCdEdxInformation;
		TTree* dPluginTree_DCdEdxInformation;

		DParticleID *dPIDAlgorithm;
};

#endif // _JEventProcessor_DCdEdxStudy_tree_

