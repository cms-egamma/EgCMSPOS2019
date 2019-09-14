#include "EgCMSPOS2019/EgammaAnalysis/interface/HistFuncs.h"

#include "EgCMSPOS2019/EgammaAnalysis/interface/EGUtilFuncs.h"

#include "TH1D.h"
#include "TLegend.h"
#include "TColor.h"

#include<algorithm>

TH1* HistFuncs::makeEffHist(TTree* tree,int nrBins,float xMin,float xMax,const std::string& var,const std::string& sampleCuts,const std::string& cuts)
{
  TH1* pass = new TH1D("passTemp","pass",nrBins,xMin,xMax);
  TH1* all = new TH1D("allTemp","all",nrBins,xMin,xMax);
  pass->Sumw2();
  all->Sumw2();
  
  float nrPass = tree->Draw((var+">>passTemp").c_str(),(sampleCuts+" && "+cuts).c_str(),"goff");
  float nrAll = tree->Draw((var+">>allTemp").c_str(),(sampleCuts).c_str(),"goff");
  
  all->SetDirectory(0);
  pass->SetDirectory(0);

  pass->Divide(pass,all,1,1,"B");
  std::cout <<"nrPass "<<nrPass<< " / "<<nrAll<<std::endl;
  delete all;
  pass->SetTitle(";;Efficiency");
  return pass;
 
}

TH1* HistFuncs::makeHist(TTree* tree,int nrBins,float xmin,float xmax,const std::string& var,const std::string& cuts)
{
  TH1* hist = new TH1D("var1Hist","temp",nrBins,xmin,xmax);
  hist->Sumw2();
  tree->Draw((var+">>var1Hist").c_str(),cuts.c_str(),"goff");
  hist->SetDirectory(0);
  hist->SetTitle("");
  return hist;
}

TH1* HistFuncs::compSigBkg(TTree* sigTree, TTree* bkgTree,int nrBins,float xmin,float xmax,const std::string& var,const std::string& commonCuts,const std::string& sigCuts,const std::string& bkgCuts,bool norm)
{
  auto combineCuts =[](const std::string& cuts1,const std::string& cuts2){
    if(cuts2.empty()) return cuts1;
    else if(cuts1.empty()) return cuts2;
    else return "("+cuts1+") && ("+cuts2+")";
  };
    

  TH1* sigHist = makeHist(sigTree,nrBins,xmin,xmax,var,combineCuts(commonCuts,sigCuts));
  TH1* bkgHist = makeHist(bkgTree,nrBins,xmin,xmax,var,combineCuts(commonCuts,bkgCuts));
  
  sigHist->SetLineWidth(2);
  sigHist->SetLineColor(kBlue+2);
  bkgHist->SetLineWidth(2);
  bkgHist->SetLineColor(kOrange+7);

  if(norm) normHists({sigHist,bkgHist});
  
  sigHist->Draw();
  bkgHist->Draw("SAME");
  TLegend* leg = new TLegend(0.143,0.774,0.443,0.876);
  leg->SetBorderSize(0);
  leg->AddEntry(sigHist,"signal","LP");
  leg->AddEntry(bkgHist,"background","LP");
  leg->Draw(); 
  return sigHist;
}

std::vector<std::vector<float> > HistFuncs::readTree(TTree* tree,const std::string& vars,const std::string& cuts)
{
  std::vector<std::vector<float> > output;
  //format of vars is "var1:var2:var3:...:varN" so number of variables  is number of : +1
  const size_t nrVars = std::count(vars.begin(),vars.end(),':')+1;
  tree->SetEstimate(tree->GetEntries()+2);
  tree->Draw(vars.c_str(),cuts.c_str(),"goff");
  const size_t nrEntries = tree->GetSelectedRows() % tree->GetEstimate();
  output.reserve(nrEntries);
  for(size_t entryNr=0;entryNr<nrEntries;entryNr++){
    std::vector<float> varsVec(nrVars,0.);
    for(size_t varNr=0;varNr<nrVars;varNr++){
      varsVec[varNr] = tree->GetVal(varNr)[entryNr];
    }
    output.emplace_back(varsVec);			
  }
  return output;
}

void HistFuncs::setHistAttributes(TH1* hist,int colour,int lineWidth,int markerStyle)
{
  hist->SetLineColor(colour);
  hist->SetLineWidth(lineWidth);
  hist->SetMarkerStyle(markerStyle);
  hist->SetMarkerColor(colour);
}

void HistFuncs::setHistAttributes(TH1* hist,int histNr)
{
  static const std::vector<int> colours = {kBlue+2,kOrange+7,kAzure+8};
  static const std::vector<int> markers = {8,22,23};
  
  int colour = colours[histNr%colours.size()];
  int marker = markers[histNr&markers.size()];
  
  setHistAttributes(hist,colour,1,marker);
}

std::vector<TH1*> HistFuncs::compVars(TTree* tree,int nrBins,float xmin,float xmax,const std::vector<std::string>& vars,const std::string& cuts)
{ 
  
  TLegend* leg = new TLegend(0.3,0.5,0.6,0.6);
  std::vector<TH1*> hists;

  std::string varStr;
  for(size_t varNr=0;varNr<vars.size();varNr++){
    if(varNr==0) varStr=vars[varNr];
    else varStr+=":"+vars[varNr];
    TH1* hist = new TH1D(("var"+std::to_string(varNr)+"Hist").c_str(),"temp",nrBins,xmin,xmax);
    hist->Sumw2();
    hist->SetDirectory(0);
    leg->AddEntry(hist,vars[varNr].c_str(),"LP");
    setHistAttributes(hist,varNr);
    hists.push_back(hist);
  }
  
  const std::vector<std::vector<float> > treeData = readTree(tree,varStr,cuts);
  for(const auto& treeEntry : treeData){
    for(size_t varNr=0;varNr<vars.size();varNr++){
      hists[varNr]->Fill(treeEntry[varNr]);
    }
  }
   
  for(size_t varNr=0;varNr<vars.size();varNr++){
    if(varNr==0) hists[varNr]->Draw();
    else hists[varNr]->Draw("SAME");
  }
  leg->Draw();
  return hists;
}


void HistFuncs::normHists(std::vector<TH1*> hists)
{
  float minIntegral = std::numeric_limits<float>::max();
  std::vector<float> integrals;
  for(TH1* hist: hists){
    integrals.push_back(hist->Integral(0,hist->GetNbinsX()+1));
    if(integrals.back()<minIntegral && integrals.back()!=0) minIntegral = integrals.back();
  }
  for(size_t histNr=0;histNr<hists.size();histNr++){
    if(integrals[histNr]!=0) hists[histNr]->Scale(minIntegral/integrals[histNr]);
  }
}

void HistFuncs::adjustYaxisMaximum(std::vector<TH1*> hists)
{
  double maximum = 0;
  for(TH1* hist : hists){
    maximum = std::max(maximum,hist->GetMaximum());
  }
  if(!hists.empty()) hists[0]->SetMaximum(maximum);

}

TChain* HistFuncs::makeChain(const std::string& chainName,std::string filelist,int nrJobs,int jobNr,int verbose)
{
  return makeChain(chainName,std::vector<std::string>{filelist},nrJobs,jobNr,verbose);
}

TChain* HistFuncs::makeChain(const std::string& chainName,std::vector<std::string> filelists,int nrJobs,int jobNr,int verbose)
{
  std::vector<std::string> filenames;
  EGUtilFuncs::readFilelist(filelists,filenames,nrJobs,jobNr,verbose); 
  TChain* chain = new TChain(chainName.c_str(),"chain");
  for(size_t i=0;i<filenames.size();i++){
    chain->Add(filenames[i].c_str());
  }
  return chain;
}

void HistFuncs::print(const std::string& fileName,const std::string& canvasName)
{
  TCanvas* canvas = static_cast<TCanvas*>(gROOT->FindObject(canvasName.c_str()));
  
  std::string outputName(fileName);
  
  std::string outputNameC = outputName + ".C";
  std::string outputNamePdf = outputName + ".pdf";
  std::string outputNamePng = outputName + ".png";
  canvas->Print(outputNamePdf.c_str());
  canvas->Print(outputNamePng.c_str());
  canvas->Print(outputNameC.c_str());
}
