#include "DSelector_pippimkpkm.h"

void DSelector_pippimkpkm::Init(TTree *locTree)
{
  // USERS: IN THIS FUNCTION, ONLY MODIFY SECTIONS WITH A "USER" OR "EXAMPLE" LABEL. LEAVE THE REST ALONE.
  
  // The Init() function is called when the selector needs to initialize a new tree or chain.
  // Typically here the branch addresses and branch pointers of the tree will be set.
  // Init() will be called many times when running on PROOF (once per file to be processed).
  
  //USERS: SET OUTPUT FILE NAME //can be overriden by user in PROOF
  dOutputFileName = "pippimkpkm.root"; //"" for none
  dOutputTreeFileName = ""; //"" for none
  dFlatTreeFileName = ""; //output flat tree (one combo per tree entry), "" for none
  dFlatTreeName = ""; //if blank, default name will be chosen
  
  //Because this function gets called for each TTree in the TChain, we must be careful:
  //We need to re-initialize the tree interface & branch wrappers, but don't want to recreate histograms
  bool locInitializedPriorFlag = dInitializedFlag; //save whether have been initialized previously
  DSelector::Init(locTree); //This must be called to initialize wrappers for each new TTree
  //gDirectory now points to the output file with name dOutputFileName (if any)
  if(locInitializedPriorFlag)
    return; //have already created histograms, etc. below: exit
  
  Get_ComboWrappers();
  dPreviousRunNumber = 0;
  
  /*********************************** EXAMPLE USER INITIALIZATION: ANALYSIS ACTIONS **********************************/
  
  //ANALYSIS ACTIONS: //Executed in order if added to dAnalysisActions
  //false/true below: use measured/kinfit data
  
  //PID
  // dAnalysisActions.push_back(new DHistogramAction_ParticleID(dComboWrapper, false, "sig_precut"));
  dAnalysisActions.push_back(new DHistogramAction_ParticleID(dComboWrapper, dChargedHypoWrapper, false, "bkg_hypotheses_precut"));
  //below: value: +/- N ns, Unknown: All PIDs, SYS_NULL: all timing systems
  // dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper, false, 0.5, KPlus, SYS_TOF));
  // dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper, false, 0.5, PiPlus, SYS_TOF));
  // dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper, false, 1, PiPlus, SYS_BCAL));
  // dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper, false, 0.5, PiMinus, SYS_TOF));
  // dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper, false, 1, PiMinus, SYS_BCAL));
  // dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper, false, 0.5, Proton, SYS_TOF));
  // dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper, false, 1, Proton, SYS_BCAL));
  
  // 2nd cut is designed to provide as much purity as possible for a particle PID, e.g: the 3 bands in the TOF Delta T for pi+ Bkg hypotheses histogram matches those in k+ signal. If you can afford to lose some statistics, cut away ALL      of the possible pi+ background tracks. Depending on your analysis, you may or may not want to do this. 1st This cut is designed to provide as much signal as possible for a particle PID, same histogram as before, except this time        you add an additional curved cut to destroy the proton background that also exists in the plot.
  
  // PiPlus BCAL
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,PiPlus,SYS_BCAL,"PiPlus_KPlusBG_BCAL_Curved",KPlus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,PiPlus,SYS_BCAL,"PiPlus_KPlusBG_BCAL",KPlus));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,PiPlus,SYS_BCAL,"PiPlus_ProtonBG_BCAL_Curved",Proton,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,PiPlus,SYS_BCAL,"PiPlus_ProtonBG_BCAL",Proton));

  //PiPlus FCAL
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,PiPlus,SYS_FCAL,"PiPlus_KPlusBG_FCAL_Curved",KPlus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,PiPlus,SYS_FCAL,"PiPlus_KPlusBG_FCAL",KPlus));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,PiPlus,SYS_FCAL,"PiPlus_ProtonBG_FCAL_Curved",Proton,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,PiPlus,SYS_FCAL,"PiPlus_ProtonBG_FCAL",Proton));

  //PiPlus TOF
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,PiPlus,SYS_TOF,"PiPlus_KPlusBG_TOF_Curved",KPlus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,PiPlus,SYS_TOF,"PiPlus_KPlusBG_TOF",KPlus));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,PiPlus,SYS_TOF,"PiPlus_ProtonBG_TOF_Curved",Proton,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,PiPlus,SYS_TOF,"PiPlus_ProtonBG_TOF",Proton));

  //PiMinus BCAL
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,PiMinus,SYS_BCAL,"PiMinus_KMinusBG_BCAL_Curved",KMinus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,PiMinus,SYS_BCAL,"PiMinus_KMinusBG_BCAL",KMinus));

  //PiMinus FCAL
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,PiMinus,SYS_FCAL,"PiMinus_KMinusBG_FCAL_Curved",KMinus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,PiMinus,SYS_FCAL,"PiMinus_KMinusBG_FCAL",KMinus));

  //PiMinus TOF
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,PiMinus,SYS_TOF,"PiMinus_KMinusBG_TOF_Curved",KMinus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,PiMinus,SYS_TOF,"PiMinus_KMinusBG_TOF",KMinus));

  //KPlus BCAL
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,KPlus,SYS_BCAL,"KPlus_PiPlusBG_BCAL_Curved",PiPlus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,KPlus,SYS_BCAL,"KPlus_PiPlusBG_BCAL",PiPlus));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,KPlus,SYS_BCAL,"KPlus_ProtonBG_BCAL_Curved",Proton,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,KPlus,SYS_BCAL,"KPlus_ProtonBG_BCAL",Proton));
  
  //KPlus FCAL
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,KPlus,SYS_FCAL,"KPlus_PiPlusBG_FCAL_Curved",PiPlus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,KPlus,SYS_FCAL,"KPlus_PiPlusBG_FCAL",PiPlus));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,KPlus,SYS_FCAL,"KPlus_ProtonBG_FCAL_Curved",Proton,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,KPlus,SYS_FCAL,"KPlus_ProtonBG_FCAL",Proton));

  //KPlus TOF
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,KPlus,SYS_TOF,"KPlus_PiPlusBG_TOF_Curved",PiPlus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,KPlus,SYS_TOF,"KPlus_PiPlusBG_TOF",PiPlus));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,KPlus,SYS_TOF,"KPlus_ProtonBG_TOF_Curved",Proton,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,KPlus,SYS_TOF,"KPlus_ProtonBG_TOF",Proton));

  //KMinus BCAL
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,KMinus,SYS_BCAL,"KMinus_PiMinusBG_BCAL_Curved",PiMinus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,KMinus,SYS_BCAL,"KMinus_PiMinusBG_BCAL",PiMinus));

  //KMinus FCAL
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,KMinus,SYS_FCAL,"KMinus_PiMinusBG_FCAL_Curved",PiMinus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,KMinus,SYS_FCAL,"KMinus_PiMinusBG_FCAL",PiMinus));

  //KMinus TOF
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,KMinus,SYS_TOF,"KMinus_PiMinusBG_TOF_Curved",PiMinus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,KMinus,SYS_TOF,"KMinus_PiMinusBG_TOF",PiMinus));

  //Proton BCAL
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,Proton,SYS_BCAL,"Proton_KPlusBG_BCAL_Curved",KPlus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,Proton,SYS_BCAL,"Proton_KPlusBG_BCAL",KPlus));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,Proton,SYS_BCAL,"Proton_PiPlusBG_BCAL_Curved",PiPlus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,Proton,SYS_BCAL,"Proton_PiPlusBG_BCAL",PiPlus));

  //Proton FCAL
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,Proton,SYS_FCAL,"Proton_KPlusBG_FCAL_Curved",KPlus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,Proton,SYS_FCAL,"Proton_KPlusBG_FCAL",KPlus));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,Proton,SYS_FCAL,"Proton_PiPlusBG_FCAL_Curved",PiPlus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.7,Proton,SYS_FCAL,"Proton_PiPlusBG_FCAL",PiPlus));

  //Proton TOF
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,Proton,SYS_TOF,"Proton_KPlusBG_TOF_Curved",KPlus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,Proton,SYS_TOF,"Proton_KPlusBG_TOF",KPlus));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,Proton,SYS_TOF,"Proton_PiPlusBG_TOF_Curved",PiPlus,true));
  dAnalysisActions.push_back(new DCutAction_PIDDeltaT(dComboWrapper,dChargedHypoWrapper,false,0.3,Proton,SYS_TOF,"Proton_PiPlusBG_TOF",PiPlus));
  
  dAnalysisActions.push_back(new DHistogramAction_ParticleID(dComboWrapper,dChargedHypoWrapper,false,"bkg_hypotheses_postcut"));
  
  // dAnalysisActions.push_back(new DHistogramAction_ParticleID(dComboWrapper, false, "sig_postcut"));
  
  //MASSES
  dAnalysisActions.push_back(new DHistogramAction_MissingMassSquared(dComboWrapper, false, 600, -0.1, 0.1, "mm2_meas"));
  // dAnalysisActions.push_back(new DHistogramAction_MissingMassSquared(dComboWrapper, true, 600, -0.1, 0.1, "mm2_kinfit"));
  dAnalysisActions.push_back(new DCutAction_MissingMassSquared(dComboWrapper, false, -0.01, 0.01));
  
  //KINFIT RESULTS
  dAnalysisActions.push_back(new DHistogramAction_KinFitResults(dComboWrapper));
  
  //CUT MISSING MASS
  //dAnalysisActions.push_back(new DCutAction_MissingMassSquared(dComboWrapper, false, -0.03, 0.02));
  
  //MASSES: Phi(1020), f0(980), Y(2175)
  // std::deque<Particle_t> kmkp_m;
  // kmkp_m.push_back(KPlus); kmkp_m.push_back(KMinus);
  // dAnalysisActions.push_back(new DHistogramAction_InvariantMass(dComboWrapper, false, 0, kmkp_m, 1000, 0.98, 1.5, "kmkp_m_Measured"));
  // dAnalysisActions.push_back(new DHistogramAction_InvariantMass(dComboWrapper, true, 0, kmkp_m, 1000, 0.98, 1.5,"kmkp_m_KinFit"));
  
  // std::deque<Particle_t> pimpip_m;
  // pimpip_m.push_back(PiPlus); pimpip_m.push_back(PiMinus);
  // dAnalysisActions.push_back(new DHistogramAction_InvariantMass(dComboWrapper, false, 0, pimpip_m, 1000, 0.28, 1.2, "pimpip_m_Measured"));
  // dAnalysisActions.push_back(new DHistogramAction_InvariantMass(dComboWrapper, true, 0, pimpip_m, 1000, 0.28, 1.2, "pimpip_m_KinFit"));
  
  // std::deque<Particle_t> kmkppimpip_m;
  // kmkppimpip_m.push_back(KPlus); kmkppimpip_m.push_back(KMinus); kmkppimpip_m.push_back(PiPlus); kmkppimpip_m.push_back(PiMinus);
  // dAnalysisActions.push_back(new DHistogramAction_InvariantMass(dComboWrapper, false, 0, kmkppimpip_m, 1000, 1.3, 3.8, "kmkppimpip_m_Measured"));
  // dAnalysisActions.push_back(new DHistogramAction_InvariantMass(dComboWrapper, true, 0, kmkppimpip_m, 1000, 1.3, 3.8, "kmkppimpip_m_KinFit"));
  
  // dAnalysisActions.push_back(new DHistogramAction_2DInvariantMass(dComboWrapper,true,0,kmkp_m,pimpip_m,1000,0.98,1.5,1000,0.28,1.2, "m_2Kvs2Pi"));
  
  //BEAM ENERGY
  dAnalysisActions.push_back(new DHistogramAction_BeamEnergy(dComboWrapper, false));
  //dAnalysisActions.push_back(new DCutAction_BeamEnergy(dComboWrapper, false, 8.4, 9.05));
  
  //KINEMATICS
  dAnalysisActions.push_back(new DHistogramAction_ParticleComboKinematics(dComboWrapper, false));
  
  //INITIALIZE ACTIONS
  //If you create any actions that you want to run manually (i.e. don't add to dAnalysisActions), be sure to initialize them here as well
  Initialize_Actions();
  
  /******************************** EXAMPLE USER INITIALIZATION: STAND-ALONE HISTOGRAMS *******************************/
  
  //EXAMPLE MANUAL HISTOGRAMS:
  dHist_MissingMassSquared = new TH1I("MissingMassSquared", ";Missing Mass Squared (GeV/c^{2})^{2}", 600, -0.06, 0.06);
  dHist_BeamEnergy = new TH1I("BeamEnergy", ";Beam Energy (GeV)", 600, 0.0, 12.0);
  
  // P4 & Vertex kinematic fit masses: Phi(1020), fo(980), Y(2175))
  h_PhiMass_KinFit = new TH1F("PhiMass_KinFit", ";m_{K^{+}K^{-}} [GeV/c^{2}];Counts", 600, 0.99, 1.5);
  h_foMass_KinFit = new TH1F("foMass_KinFit", ";m_{#pi^{+}#pi^{-}} [GeV/c^{2}];Counts", 600, 0.28, 1.5);
  h_YMass_KinFit = new TH1F("YMass_KinFit", ";m_{K^{+}K^{-}#pi^{+}#pi^{-}} [GeV/c^{2}];Counts", 600, 1.5, 3.2);
  
  // Chi2/NDF KinFit positive cut
  h_PhiMass_chi2ndfcut = new TH1F("PhiMass_chi2ndfcut", ";m_{K^{+}K^{-}} [GeV/c^{2}];Counts", 600, 0.99, 1.5);
  h_foMass_chi2ndfcut = new TH1F("foMass_chi2ndfcut", ";m_{#pi^{+}#pi^{-}} [GeV/c^{2}];Counts", 600, 0.28, 1.5);
  h_YMass_chi2ndfcut = new TH1F("YMass_chi2ndfcut", ";m_{K^{+}K^{-}#pi^{+}#pi^{-}} [GeV/c^{2}];Counts", 600, 1.5, 3.2);

  // Chi2/NDF KinFit negative cut
  h_PhiMass_chi2ndfcut2 = new TH1F("PhiMass_chi2ndfcut2", ";m_{K^{+}K^{-}} [GeV/c^{2}];Counts", 600, 0.99, 1.5);
  h_foMass_chi2ndfcut2 = new TH1F("foMass_chi2ndfcut2", ";m_{#pi^{+}#pi^{-}} [GeV/c^{2}];Counts", 600, 0.28, 1.5);
  h_YMass_chi2ndfcut2 = new TH1F("YMass_chi2ndfcut2", ";m_{K^{+}K^{-}#pi^{+}#pi^{-}} [GeV/c^{2}];Counts", 600, 1.5, 3.2);
  
  // Cut on Phi(1020) mass < 1.06GeV/c
  h_PhiMass_kmkpcut = new TH1F("PhiMass_kmkpcut", ";m_{K^{+}K^{-}} [GeV/c^{2}];Counts", 600, 0.99, 1.06);
  h_foMass_kmkpcut = new TH1F("foMass_kmkpcut", ";m_{#pi^{+}#pi^{-}} [GeV/c^{2}];Counts", 600, 0.28, 1.5);
  h_YMass_kmkpcut = new TH1F("YMass_kmkpcut", ";m_{K^{+}K^{-}#pi^{+}#pi^{-}} [GeV/c^{2}];Counts", 600, 1.5, 3.2);	
  
  // Cut on Phi(1020) mass > 1.06GeV/c
  h_PhiMass_kmkpcut2 = new TH1F("PhiMass_kmkpcut2", ";m_{K^{+}K^{-}} [GeV/c^{2}];Counts", 600, 1.06, 1.5);
  h_foMass_kmkpcut2 = new TH1F("foMass_kmkpcut2", ";m_{#pi^{+}#pi^{-}} [GeV/c^{2}];Counts", 600, 0.28, 1.5);
  h_YMass_kmkpcut2 = new TH1F("YMass_kmkpcut2", ";m_{K^{+}K^{-}#pi^{+}#pi^{-}} [GeV/c^{2}];Counts", 600, 1.5, 3.2);	
  
  // kmkp vs pimpip
  h2_kmkpvspimpip_KinFit = new TH2F("kmkpvspimpip_KinFit",";m_{K^{+}K^{-}} [GeV/c^{2}];m_{#pi^{+}#pi^{-}} [GeV/c^{2}]",600,0.99,1.5,600,0.28,1.5);
  h2_kmkpvspimpip_chi2ndfcut = new TH2F("kmkpvspimpip_chi2ndfcut",";m_{K^{+}K^{-}} [GeV/c^{2}];m_{#pi^{+}#pi^{-}} [GeV/c^{2}]",600,0.99,1.5,600,0.28,1.5);
  h2_kmkpvspimpip_kmkpcut = new TH2F("kmkpvspimpip_kmkpcut",";m_{K^{+}K^{-}} [GeV/c^{2}];m_{#pi^{+}#pi^{-}} [GeV/c^{2}]",600,0.99,1.06,600,0.28,1.5);
  
  // kmkp vs kmkppimpip
  h2_kmkpvskmkppimpip_KinFit = new TH2F("kmkpvskmkppimpip_KinFit",";m_{K^{+}K^{-}} [GeV/c^{2}];m_{K^{+}K^{-}#pi^{+}#pi^{-}} [GeV/c^{2}]",600,0.99,1.5,600,1.5,3.2);
  h2_kmkpvskmkppimpip_chi2ndfcut = new TH2F("kmkpvskmkppimpip_chi2ndfcut",";m_{K^{+}K^{-}} [GeV/c^{2}];m_{K^{+}K^{-}#pi^{+}#pi^{-}} [GeV/c^{2}]",600,0.99,1.5,600,1.5,3.2);
  h2_kmkpvskmkppimpip_kmkpcut = new TH2F("kmkpvskmkppimpip_kmkpcut",";m_{K^{+}K^{-}} [GeV/c^{2}];m_{K^{+}K^{-}#pi^{+}#pi^{-}} [GeV/c^{2}]",600,0.99,1.06,600,1.5,3.2);
  
  // pimpip vs kmkppimpip
  h2_pimpipvskmkppimpip_KinFit = new TH2F("pimpipvskmkppimpip_KinFit",";m_{#pi^{+}#pi^{-}} [GeV/c^{2}];m_{K^{+}K^{-}#pi^{+}#pi^{-}} [GeV/c^{2}]",600,0.28,1.5,600,1.5,3.2);
  h2_pimpipvskmkppimpip_chi2ndfcut = new TH2F("pimpipvskmkppimpip_chi2ndfcut",";m_{#pi^{+}#pi^{-}} [GeV/c^{2}];m_{K^{+}K^{-}#pi^{+}#pi^{-}} [GeV/c^{2}]",600,0.28,1.5,600,1.5,3.2);
  h2_pimpipvskmkppimpip_kmkpcut = new TH2F("pimpipvskmkppimpip_kmkpcut",";m_{#pi^{+}#pi^{-}} [GeV/c^{2}];m_{K^{+}K^{-}#pi^{+}#pi^{-}} [GeV/c^{2}]",600,0.28,1.5,600,1.5,3.2);

  // Chi2/ndf
  hchi2ndf = new TH1F("hchi2ndf", "#chi2/ndf of the Kinematic Fit (P4 + Vertex) ;#chi2/ndf ;Counts", 100, 0, 1000);
  /************************** EXAMPLE USER INITIALIZATION: CUSTOM OUTPUT BRANCHES - MAIN TREE *************************/

  //EXAMPLE MAIN TREE CUSTOM BRANCHES (OUTPUT ROOT FILE NAME MUST FIRST BE GIVEN!!!! (ABOVE: TOP)):
  //The type for the branch must be included in the brackets
  //1st function argument is the name of the branch
  //2nd function argument is the name of the branch that contains the size of the array (for fundamentals only)
  /*
    dTreeInterface->Create_Branch_Fundamental<Int_t>("my_int"); //fundamental = char, int, float, double, etc.
    dTreeInterface->Create_Branch_FundamentalArray<Int_t>("my_int_array", "my_int");
    dTreeInterface->Create_Branch_FundamentalArray<Float_t>("my_combo_array", "NumCombos");
    dTreeInterface->Create_Branch_NoSplitTObject<TLorentzVector>("my_p4");
    dTreeInterface->Create_Branch_ClonesArray<TLorentzVector>("my_p4_array");
  */

  /************************** EXAMPLE USER INITIALIZATION: CUSTOM OUTPUT BRANCHES - FLAT TREE *************************/

  //EXAMPLE FLAT TREE CUSTOM BRANCHES (OUTPUT ROOT FILE NAME MUST FIRST BE GIVEN!!!! (ABOVE: TOP)):
  //The type for the branch must be included in the brackets
  //1st function argument is the name of the branch
  //2nd function argument is the name of the branch that contains the size of the array (for fundamentals only)
  /*
    dFlatTreeInterface->Create_Branch_Fundamental<Int_t>("flat_my_int"); //fundamental = char, int, float, double, etc.
    dFlatTreeInterface->Create_Branch_FundamentalArray<Int_t>("flat_my_int_array", "flat_my_int");
    dFlatTreeInterface->Create_Branch_NoSplitTObject<TLorentzVector>("flat_my_p4");
    dFlatTreeInterface->Create_Branch_ClonesArray<TLorentzVector>("flat_my_p4_array");
  */

  /************************************* ADVANCED EXAMPLE: CHOOSE BRANCHES TO READ ************************************/

  //TO SAVE PROCESSING TIME
  //If you know you don't need all of the branches/data, but just a subset of it, you can speed things up
  //By default, for each event, the data is retrieved for all branches
  //If you know you only need data for some branches, you can skip grabbing data from the branches you don't need
  //Do this by doing something similar to the commented code below

  //dTreeInterface->Clear_GetEntryBranches(); //now get none
  //dTreeInterface->Register_GetEntryBranch("Proton__P4"); //manually set the branches you want
}

Bool_t DSelector_pippimkpkm::Process(Long64_t locEntry)
{
  // The Process() function is called for each entry in the tree. The entry argument
  // specifies which entry in the currently loaded tree is to be processed.
  //
  // This function should contain the "body" of the analysis. It can contain
  // simple or elaborate selection criteria, run algorithms on the data
  // of the event and typically fill histograms.
  //
  // The processing can be stopped by calling Abort().
  // Use fStatus to set the return value of TTree::Process().
  // The return value is currently not used.

  //CALL THIS FIRST
  DSelector::Process(locEntry); //Gets the data from the tree for the entry
  //cout << "RUN " << Get_RunNumber() << ", EVENT " << Get_EventNumber() << endl;
  //TLorentzVector locProductionX4 = Get_X4_Production();

  /******************************************** GET POLARIZATION ORIENTATION ******************************************/

  //Only if the run number changes
  //RCDB environment must be setup in order for this to work! (Will return false otherwise)
  UInt_t locRunNumber = Get_RunNumber();
  if(locRunNumber != dPreviousRunNumber)
    {
      dIsPolarizedFlag = dAnalysisUtilities.Get_IsPolarizedBeam(locRunNumber, dIsPARAFlag);
      dPreviousRunNumber = locRunNumber;
    }

  /********************************************* SETUP UNIQUENESS TRACKING ********************************************/

  //ANALYSIS ACTIONS: Reset uniqueness tracking for each action
  //For any actions that you are executing manually, be sure to call Reset_NewEvent() on them here
  Reset_Actions_NewEvent();

  //PREVENT-DOUBLE COUNTING WHEN HISTOGRAMMING
  //Sometimes, some content is the exact same between one combo and the next
  //e.g. maybe two combos have different beam particles, but the same data for the final-state
  //When histogramming, you don't want to double-count when this happens: artificially inflates your signal (or background)
  //So, for each quantity you histogram, keep track of what particles you used (for a given combo)
  //Then for each combo, just compare to what you used before, and make sure it's unique

  //EXAMPLE 1: Particle-specific info:
  set<Int_t> locUsedSoFar_BeamEnergy; //Int_t: Unique ID for beam particles. set: easy to use, fast to search

  //EXAMPLE 2: Combo-specific info:
  //In general: Could have multiple particles with the same PID: Use a set of Int_t's
  //In general: Multiple PIDs, so multiple sets: Contain within a map
  //Multiple combos: Contain maps within a set (easier, faster to search)
  set<map<Particle_t, set<Int_t> > > locUsedSoFar_MissingMass;

  //INSERT USER ANALYSIS UNIQUENESS TRACKING HERE
  // masses: Phi(1020), fo(980), Y(2175).
  set<map<unsigned int, set<Int_t> > > locUsedSoFar_PhiMass;
  set<map<unsigned int, set<Int_t> > > locUsedSoFar_foMass;
  set<map<unsigned int, set<Int_t> > > locUsedSoFar_YMass;

  /**************************************** EXAMPLE: FILL CUSTOM OUTPUT BRANCHES **************************************/

  /*
    Int_t locMyInt = 7;
    dTreeInterface->Fill_Fundamental<Int_t>("my_int", locMyInt);

    TLorentzVector locMyP4(4.0, 3.0, 2.0, 1.0);
    dTreeInterface->Fill_TObject<TLorentzVector>("my_p4", locMyP4);

    for(int loc_i = 0; loc_i < locMyInt; ++loc_i)
    dTreeInterface->Fill_Fundamental<Int_t>("my_int_array", 3*loc_i, loc_i); //2nd argument = value, 3rd = array index
  */

  /************************************************* LOOP OVER COMBOS *************************************************/

  //Loop over combos
  for(UInt_t loc_i = 0; loc_i < Get_NumCombos(); ++loc_i)
    {
      //Set branch array indices for combo and all combo particles
      dComboWrapper->Set_ComboIndex(loc_i);

      // Is used to indicate when combos have been cut
      if(dComboWrapper->Get_IsComboCut()) // Is false when tree originally created
	continue; // Combo has been cut previously

      /********************************************** GET PARTICLE INDICES *********************************************/

      //Used for tracking uniqueness when filling histograms, and for determining unused particles

      //Step 0
      Int_t locBeamID = dComboBeamWrapper->Get_BeamID();
      Int_t locPiPlusTrackID = dPiPlusWrapper->Get_TrackID();
      Int_t locPiMinusTrackID = dPiMinusWrapper->Get_TrackID();
      Int_t locKPlusTrackID = dKPlusWrapper->Get_TrackID();
      Int_t locKMinusTrackID = dKMinusWrapper->Get_TrackID();
      Int_t locProtonTrackID = dProtonWrapper->Get_TrackID();

      /*********************************************** GET FOUR-MOMENTUM **********************************************/

      // Get P4's: //is kinfit if kinfit performed, else is measured
      //dTargetP4 is target p4
      //Step 0
      TLorentzVector locBeamP4 = dComboBeamWrapper->Get_P4();
      TLorentzVector locPiPlusP4 = dPiPlusWrapper->Get_P4();
      TLorentzVector locPiMinusP4 = dPiMinusWrapper->Get_P4();
      TLorentzVector locKPlusP4 = dKPlusWrapper->Get_P4();
      TLorentzVector locKMinusP4 = dKMinusWrapper->Get_P4();
      TLorentzVector locProtonP4 = dProtonWrapper->Get_P4();

      // Get Measured P4's:
      //Step 0
      TLorentzVector locBeamP4_Measured = dComboBeamWrapper->Get_P4_Measured();
      TLorentzVector locPiPlusP4_Measured = dPiPlusWrapper->Get_P4_Measured();
      TLorentzVector locPiMinusP4_Measured = dPiMinusWrapper->Get_P4_Measured();
      TLorentzVector locKPlusP4_Measured = dKPlusWrapper->Get_P4_Measured();
      TLorentzVector locKMinusP4_Measured = dKMinusWrapper->Get_P4_Measured();
      TLorentzVector locProtonP4_Measured = dProtonWrapper->Get_P4_Measured();

      /********************************************* COMBINE FOUR-MOMENTUM ********************************************/

      // DO YOUR STUFF HERE

      // Combine 4-vectors
      TLorentzVector locMissingP4_Measured = locBeamP4_Measured + dTargetP4;
      locMissingP4_Measured -= locPiPlusP4_Measured + locPiMinusP4_Measured + locKPlusP4_Measured + locKMinusP4_Measured + locProtonP4_Measured;

      /******************************************** EXECUTE ANALYSIS ACTIONS *******************************************/

      // Loop through the analysis actions, executing them in order for the active particle combo
      if(!Execute_Actions()) //if the active combo fails a cut, IsComboCutFlag automatically set
	continue;

      //if you manually execute any actions, and it fails a cut, be sure to call:
      //dComboWrapper->Set_IsComboCut(true);

      /**************************************** EXAMPLE: FILL CUSTOM OUTPUT BRANCHES **************************************/

      /*
	TLorentzVector locMyComboP4(8.0, 7.0, 6.0, 5.0);
	//for arrays below: 2nd argument is value, 3rd is array index
	//NOTE: By filling here, AFTER the cuts above, some indices won't be updated (and will be whatever they were from the last event)
	//So, when you draw the branch, be sure to cut on "IsComboCut" to avoid these.
	dTreeInterface->Fill_Fundamental<Float_t>("my_combo_array", -2*loc_i, loc_i);
	dTreeInterface->Fill_TObject<TLorentzVector>("my_p4_array", locMyComboP4, loc_i);
      */

      /**************************************** EXAMPLE: HISTOGRAM BEAM ENERGY *****************************************/

      //Histogram beam energy (if haven't already)
      if(locUsedSoFar_BeamEnergy.find(locBeamID) == locUsedSoFar_BeamEnergy.end())
	{
	  dHist_BeamEnergy->Fill(locBeamP4.E());
	  locUsedSoFar_BeamEnergy.insert(locBeamID);
	}

      /************************************ EXAMPLE: HISTOGRAM MISSING MASS SQUARED ************************************/

      //Missing Mass Squared
      double locMissingMassSquared = locMissingP4_Measured.M2();

      //Uniqueness tracking: Build the map of particles used for the missing mass
      //For beam: Don't want to group with final-state photons. Instead use "Unknown" PID (not ideal, but it's easy).
      map<Particle_t, set<Int_t> > locUsedThisCombo_MissingMass;
      locUsedThisCombo_MissingMass[Unknown].insert(locBeamID); //beam
      locUsedThisCombo_MissingMass[PiPlus].insert(locPiPlusTrackID);
      locUsedThisCombo_MissingMass[PiMinus].insert(locPiMinusTrackID);
      locUsedThisCombo_MissingMass[KPlus].insert(locKPlusTrackID);
      locUsedThisCombo_MissingMass[KMinus].insert(locKMinusTrackID);
      locUsedThisCombo_MissingMass[Proton].insert(locProtonTrackID);

      //compare to what's been used so far
      if(locUsedSoFar_MissingMass.find(locUsedThisCombo_MissingMass) == locUsedSoFar_MissingMass.end())
	{
	  //unique missing mass combo: histogram it, and register this combo of particles
	  dHist_MissingMassSquared->Fill(locMissingMassSquared);
	  locUsedSoFar_MissingMass.insert(locUsedThisCombo_MissingMass);
	}

      //E.g. Cut
      //if((locMissingMassSquared < -0.04) || (locMissingMassSquared > 0.04))
      //{
      //	dComboWrapper->Set_IsComboCut(true);
      //	continue;
      //}

      // **************************** Phi, fo and Y invariant masses *********************************************

      // P4 & Vertex kinematic fit
      double PhiMass_KinFit = (locKPlusP4 + locKMinusP4).M();
      double foMass_KinFit = (locPiPlusP4 + locPiMinusP4).M();
      double YMass_KinFit = (locKPlusP4 + locKMinusP4 + locPiPlusP4 + locPiMinusP4).M();
      double locKinFitChiSqPerNDF = dComboWrapper->Get_ChiSq_KinFit()/dComboWrapper->Get_NDF_KinFit();
      hchi2ndf->Fill(locKinFitChiSqPerNDF);
		
      map<unsigned int, set<Int_t> >  locUsedThisCombo_PhiMass;
      locUsedThisCombo_PhiMass[abs(PDGtype(KPlus))].insert(locKPlusTrackID);
      locUsedThisCombo_PhiMass[abs(PDGtype(KMinus))].insert(locKMinusTrackID);
      map<unsigned int, set<Int_t> > locUsedThisCombo_foMass;
      locUsedThisCombo_foMass[abs(PDGtype(PiPlus))].insert(locPiPlusTrackID);
      locUsedThisCombo_foMass[abs(PDGtype(PiMinus))].insert(locPiMinusTrackID);
      map<unsigned int, set<Int_t> > locUsedThisCombo_YMass;
      locUsedThisCombo_YMass[abs(PDGtype(KPlus))].insert(locKPlusTrackID);
      locUsedThisCombo_YMass[abs(PDGtype(KMinus))].insert(locKMinusTrackID);
      locUsedThisCombo_YMass[abs(PDGtype(PiPlus))].insert(locPiPlusTrackID);
      locUsedThisCombo_YMass[abs(PDGtype(PiMinus))].insert(locPiMinusTrackID);
		
      if(locUsedSoFar_PhiMass.find(locUsedThisCombo_PhiMass) == locUsedSoFar_PhiMass.end())
	{
	  h_PhiMass_KinFit->Fill(PhiMass_KinFit);
	  if(locKinFitChiSqPerNDF<50)
	    {
	      h_PhiMass_chi2ndfcut->Fill(PhiMass_KinFit);
	      if(PhiMass_KinFit<1.06)
		{
		  h_PhiMass_kmkpcut->Fill(PhiMass_KinFit);
		}
	    }
		    
	  if(locKinFitChiSqPerNDF>50)
	    {
	      h_PhiMass_chi2ndfcut2->Fill(PhiMass_KinFit);
	      if(PhiMass_KinFit>1.06)
		h_PhiMass_kmkpcut2->Fill(PhiMass_KinFit);
	    }

	  locUsedSoFar_PhiMass.insert(locUsedThisCombo_PhiMass);
	}
		
      if(locUsedSoFar_foMass.find(locUsedThisCombo_foMass) == locUsedSoFar_foMass.end())
	{
	  h_foMass_KinFit->Fill(foMass_KinFit);
	  h2_kmkpvspimpip_KinFit->Fill(PhiMass_KinFit, foMass_KinFit);
	  if(locKinFitChiSqPerNDF<50)
	    {
	      h_foMass_chi2ndfcut->Fill(foMass_KinFit);
	      h2_kmkpvspimpip_chi2ndfcut->Fill(PhiMass_KinFit, foMass_KinFit);
	      if(PhiMass_KinFit<1.06)
		{
		  h_foMass_kmkpcut->Fill(foMass_KinFit);
		  h2_kmkpvspimpip_kmkpcut->Fill(PhiMass_KinFit, foMass_KinFit);
		}
	    }

	  if(locKinFitChiSqPerNDF>50)
	    {
	      h_foMass_chi2ndfcut2->Fill(foMass_KinFit);
	      if(PhiMass_KinFit>1.06)
		h_foMass_kmkpcut2->Fill(foMass_KinFit);
	    }

	  locUsedSoFar_foMass.insert(locUsedThisCombo_foMass);
	}
		
      if(locUsedSoFar_YMass.find(locUsedThisCombo_YMass) == locUsedSoFar_YMass.end())
	{
	  h_YMass_KinFit->Fill(YMass_KinFit);
	  h2_kmkpvskmkppimpip_KinFit->Fill(PhiMass_KinFit,YMass_KinFit);
	  h2_pimpipvskmkppimpip_KinFit->Fill(foMass_KinFit,YMass_KinFit);
	  if(locKinFitChiSqPerNDF<50)
	    {
	      h_YMass_chi2ndfcut->Fill(YMass_KinFit);
	      h2_kmkpvskmkppimpip_chi2ndfcut->Fill(PhiMass_KinFit,YMass_KinFit);
	      h2_pimpipvskmkppimpip_chi2ndfcut->Fill(foMass_KinFit,YMass_KinFit);
	      if(PhiMass_KinFit<1.06)
		{
		  h_YMass_kmkpcut->Fill(YMass_KinFit);
		  h2_kmkpvskmkppimpip_kmkpcut->Fill(PhiMass_KinFit,YMass_KinFit);
		  h2_pimpipvskmkppimpip_kmkpcut->Fill(foMass_KinFit,YMass_KinFit);
		}
	    }

	  if(locKinFitChiSqPerNDF>50)
	    {
	      h_YMass_chi2ndfcut2->Fill(YMass_KinFit);
	      if(PhiMass_KinFit>1.06)
		h_YMass_kmkpcut2->Fill(YMass_KinFit);
	    }

	  locUsedSoFar_YMass.insert(locUsedThisCombo_YMass);
	}

      /****************************************** FILL FLAT TREE (IF DESIRED) ******************************************/

      /*
      //FILL ANY CUSTOM BRANCHES FIRST!!
      Int_t locMyInt_Flat = 7;
      dFlatTreeInterface->Fill_Fundamental<Int_t>("flat_my_int", locMyInt_Flat);

      TLorentzVector locMyP4_Flat(4.0, 3.0, 2.0, 1.0);
      dFlatTreeInterface->Fill_TObject<TLorentzVector>("flat_my_p4", locMyP4_Flat);

      for(int loc_j = 0; loc_j < locMyInt_Flat; ++loc_j)
      {
      dFlatTreeInterface->Fill_Fundamental<Int_t>("flat_my_int_array", 3*loc_j, loc_j); //2nd argument = value, 3rd = array index
      TLorentzVector locMyComboP4_Flat(8.0, 7.0, 6.0, 5.0);
      dFlatTreeInterface->Fill_TObject<TLorentzVector>("flat_my_p4_array", locMyComboP4_Flat, loc_j);
      }
      */

      //FILL FLAT TREE
      //Fill_FlatTree(); //for the active combo
    } // end of combo loop

  //FILL HISTOGRAMS: Num combos / events surviving actions
  Fill_NumCombosSurvivedHists();

  /******************************************* LOOP OVER THROWN DATA (OPTIONAL) ***************************************/
  /*
  //Thrown beam: just use directly
  if(dThrownBeam != NULL)
  double locEnergy = dThrownBeam->Get_P4().E();

  //Loop over throwns
  for(UInt_t loc_i = 0; loc_i < Get_NumThrown(); ++loc_i)
  {
  //Set branch array indices corresponding to this particle
  dThrownWrapper->Set_ArrayIndex(loc_i);

  //Do stuff with the wrapper here ...
  }
  */
  /****************************************** LOOP OVER OTHER ARRAYS (OPTIONAL) ***************************************/
  /*
  //Loop over beam particles (note, only those appearing in combos are present)
  for(UInt_t loc_i = 0; loc_i < Get_NumBeam(); ++loc_i)
  {
  //Set branch array indices corresponding to this particle
  dBeamWrapper->Set_ArrayIndex(loc_i);

  //Do stuff with the wrapper here ...
  }

  //Loop over charged track hypotheses
  for(UInt_t loc_i = 0; loc_i < Get_NumChargedHypos(); ++loc_i)
  {
  //Set branch array indices corresponding to this particle
  dChargedHypoWrapper->Set_ArrayIndex(loc_i);

  //Do stuff with the wrapper here ...
  }

  //Loop over neutral particle hypotheses
  for(UInt_t loc_i = 0; loc_i < Get_NumNeutralHypos(); ++loc_i)
  {
  //Set branch array indices corresponding to this particle
  dNeutralHypoWrapper->Set_ArrayIndex(loc_i);

  //Do stuff with the wrapper here ...
  }
  */

  /************************************ EXAMPLE: FILL CLONE OF TTREE HERE WITH CUTS APPLIED ************************************/
  /*
    Bool_t locIsEventCut = true;
    for(UInt_t loc_i = 0; loc_i < Get_NumCombos(); ++loc_i) {
    //Set branch array indices for combo and all combo particles
    dComboWrapper->Set_ComboIndex(loc_i);
    // Is used to indicate when combos have been cut
    if(dComboWrapper->Get_IsComboCut())
    continue;
    locIsEventCut = false; // At least one combo succeeded
    break;
    }
    if(!locIsEventCut && dOutputTreeFileName != "")
    Fill_OutputTree();
  */

  return kTRUE;
}

void DSelector_pippimkpkm::Finalize(void)
{
  //Save anything to output here that you do not want to be in the default DSelector output ROOT file.

  //Otherwise, don't do anything else (especially if you are using PROOF).
  //If you are using PROOF, this function is called on each thread,
  //so anything you do will not have the combined information from the various threads.
  //Besides, it is best-practice to do post-processing (e.g. fitting) separately, in case there is a problem.

  //DO YOUR STUFF HERE

  //CALL THIS LAST
  DSelector::Finalize(); //Saves results to the output file
}
