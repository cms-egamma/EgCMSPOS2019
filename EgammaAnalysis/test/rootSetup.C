{
  gStyle->SetOptStat(0);
  gSystem->Load("libEgCMSPOS2019EgammaAnalysis.so");
  //suppressing noisy fits
  RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL); 
  RooMsgService::instance().setSilentMode(true);
  gErrorIgnoreLevel = kError;
  
}
