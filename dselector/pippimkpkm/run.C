void run(TString runnum) 
{
  TStopwatch timer;
  timer.Start();
  gROOT->ProcessLine(".x $ROOT_ANALYSIS_HOME/scripts/Load_DSelector.C");

  // gROOT->ProcessLine(Form("DPROOFLiteManager::Process_Tree(\"/cache/halld/RunPeriod-2016-02/analysis/ver05/tree_pimpipkmkp/%s/*.root\", \"pimpipkmkp_Tree\", \"DSelector_pimpipkmkp.C+\", 8, \"pimpipkmkp_%s.root\")",runnum.Data(),runnum.Data()));

  gROOT->ProcessLine(Form("DPROOFLiteManager::Process_Tree(\"/cache/halld/RunPeriod-2017-01/analysis/ver06/tree_pippimkpkm__U1/merged/tree_pippimkpkm__U1_%s.root\", \"pippimkpkm__U1_Tree\", \"DSelector_pippimkpkm.C+\", 8, \"/volatile/halld/home/nacer/ana/pippimkpkm/ROOT/17v6h/pippimkpkm__U1_17v6l_%s.root\")",runnum.Data(),runnum.Data()));

  // gROOT->ProcessLine("DPROOFLiteManager::Process_Tree(\"/cache/halld/RunPeriod-2017-01/analysis/ver06/tree_pippimkpkm__U1/merged/tree_pippimkpkm__U1_0303*.root\", \"pippimkpkm__U1_Tree\", \"DSelector_pippimkpkm.C+\", 8, \"pippimkpkm__U1_17v6l_test.root\")");

  // DPROOFLiteManager::Process_Tree("/cache/halld/RunPeriod-2017-01/analysis/ver06/tree_pippimkpkm__U1/merged/tree_pippimkpkm__U1_0303*.root", "pippimkpkm__U1_Tree", "DSelector_pippimkpkm.C+", 8, "pippimkpkm__U1_17v6l_test.root")

  timer.Stop();
  cout << "RealTime: " << timer.RealTime() << " seconds" << endl;
  cout << "CpuTime: " << timer.CpuTime() << " seconds" << endl;
  gROOT->ProcessLine(".q");
}
