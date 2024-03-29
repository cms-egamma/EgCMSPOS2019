#ifndef EgCMSPOS2019_EgammaAnalysis_HistFuncs_h
#define EgCMSPOS2019_EgammaAnalysis_HistFuncs_h

#include "TH1.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TROOT.h"

#include <vector>
#include <string>
#include <iostream>

//this is just a handly group of utility classes for making & plotting histograms
//nothing to do with CMSSW but its convient to use CMSSW as a build system
//note  these are meant to be used interactively in a root session and as such leak like crazy!

class HistFuncs {
private:
  HistFuncs(){} //no instantation possible
public:
  static std::vector<std::vector<float> > readTree(TTree* tree,const std::string& vars,const std::string& cuts);

  static TH1* makeEffHist(TTree* tree,int nrBins,float xMin,float xMax,const std::string& var,const std::string& sampleCuts,const std::string& cuts);
  
  static TH1* makeHist(TTree* tree,int nrBins,float xmin,float xmax,const std::string& var,const std::string& cuts);
  static TH1* compSigBkg(TTree* sigTree, TTree* bkgTree,int nrBins,float xmin,float xmax,const std::string& var,const std::string& commonCuts,const std::string& sigCuts,const std::string& bkgCuts,bool norm);
  static TH1* compVars(TTree* tree,int nrBins,float xmin,float xmax,const std::vector<std::string>& vars,const std::string& cuts);

  static void setHistAttributes(TH1* hist,int histNr);
  static void setHistAttributes(TH1* hist,int colour,int lineWidth,int markerStyle);
  static void normHists(std::vector<TH1*> hists);
  static void adjustYaxisMaximum(std::vector<TH1*> hists);

  static TChain* makeChain(const std::string& chainName,std::string filelist,int nrJobs=1,int jobNr=1,int verbose=2);
  static TChain* makeChain(const std::string& chainName,std::vector<std::string> filelist,int nrJobs=1,int jobNr=1,int verbose=2);
  static void print(const std::string& fileName,const std::string& canvasName);
  template<typename T,typename Container=TCanvas>
  static std::vector<T*> getFromCanvas(Container* c1,const std::string& className,int verbose=0);
  static void getHistIntegral(const TH1* theHist,double xMin,double xMax,double& nrEvents,double& nrEventsErr);
  static double getHistIntegral(const TH1* theHist,double xMin,double xMax);
  static int getBinNr(const TH1* theHist,double x);
  static TH1* compareDataMC(TH1* dataHist,TH1* mcHist,bool norm);
};

template<typename T,typename Container>
std::vector<T*> HistFuncs::getFromCanvas(Container* c1,const std::string& className,int verbose)
{
  std::vector<T*> result;
  TIter next(c1->GetListOfPrimitives());
  while (TObject* tObj=next()) {
    if(verbose>0) std::cout <<tObj->ClassName()<<std::endl;
    if(tObj->ClassName()==className){
      T* obj  = static_cast<T*>(tObj);
      result.push_back(obj);
    }
  }
  if(verbose>0) std::cout <<"nr objs "<<result.size()<<std::endl;
  return result;
}

#endif


