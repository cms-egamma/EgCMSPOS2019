
#include "TTree.h"
#include "TFile.h"
#include "TLorentzVector.h"

#include <vector>
#include <utility>
#include <string>
#include <iostream>
#include <unordered_map>

//So I was in a rush to make a TTree with mass for the school
//as a result, completely not thought out at all
//converts a tree of eleEn[eleInx] -> ele1En, ele2En, mass


enum class DataType {
  kInvalid=0,kFloat,kInt,kUShort,kULong,kVecInt,kVecFloat
};

struct EleVecBranches {
  

  //vector of elePt[eleIndex],eleEn[eleIndx]
  std::vector<std::vector<float>* > dataFloat;
  std::vector<std::vector<int>* >dataInt;
  std::vector<std::vector<UShort_t>* >dataUShort;
  std::vector<std::vector<ULong64_t>* >dataULong;
  std::vector<std::vector<std::vector<int>>*> dataVecInt;
  std::vector<std::vector<std::vector<float>>*> dataVecFloat;
  std::vector<std::pair<std::string,DataType> > names;
  std::vector<int> nrBranchesByType;
  //oh screw it, lets brute force it
  std::unordered_map<std::string,size_t> dataFloatNameMap;
  std::unordered_map<std::string,size_t> dataIntNameMap;
  std::unordered_map<std::string,size_t> dataUShortNameMap;
  std::unordered_map<std::string,size_t> dataULongNameMap;
  std::unordered_map<std::string,size_t> dataVecFloatNameMap;
  std::unordered_map<std::string,size_t> dataVecIntNameMap;


  
  EleVecBranches(): nrBranchesByType(static_cast<int>(DataType::kVecFloat)+1,0){}  

  DataType getDataType(const std::string& dataTypeStr){
    if(dataTypeStr=="vector<float>") return DataType::kFloat;
    else if(dataTypeStr=="vector<int>") return DataType::kInt;
    else if(dataTypeStr=="vector<unsigned short>") return DataType::kUShort;
    else if(dataTypeStr=="vector<ULong64_t>") return DataType::kULong;
    else if(dataTypeStr=="vector<vector<int> >") return DataType::kVecInt;
    else if(dataTypeStr=="vector<vector<float> >") return DataType::kVecFloat;
    else{
      std::cout <<"type "<<dataTypeStr<<" not recognised"<<std::endl;
      return DataType::kInvalid;
    }
  }
   

  void init(TTree* tree); 
};

void EleVecBranches::init(TTree* tree)
{

  for(int branchNr=0;branchNr<tree->GetListOfBranches()->GetEntries();branchNr++){
    std::string name = (*tree->GetListOfBranches())[branchNr]->GetName();
    if(name.find("ele")==0 && name.find("eleGSF")!=0){
      TBranch* branch = tree->GetBranch(name.c_str());
      DataType dataType = getDataType(branch->GetClassName());
      names.push_back({std::move(name),dataType});      
      nrBranchesByType[static_cast<int>(dataType)]++;
    }
  }

  //now we allocate our memories
  dataFloat.resize(nrBranchesByType[static_cast<int>(DataType::kFloat)],nullptr);
  dataInt.resize(nrBranchesByType[static_cast<int>(DataType::kInt)],nullptr);
  dataUShort.resize(nrBranchesByType[static_cast<int>(DataType::kUShort)],nullptr);
  dataULong.resize(nrBranchesByType[static_cast<int>(DataType::kULong)],nullptr);
  dataVecInt.resize(nrBranchesByType[static_cast<int>(DataType::kVecInt)],nullptr);
  dataVecFloat.resize(nrBranchesByType[static_cast<int>(DataType::kVecFloat)],nullptr);

  for(auto& entry: dataFloat) entry=new std::vector<float>();
  for(auto& entry: dataInt) entry=new std::vector<int>();
  for(auto& entry: dataUShort) entry=new std::vector<UShort_t>();
  for(auto& entry: dataULong) entry=new std::vector<ULong64_t>();
  for(auto& entry: dataVecInt) entry=new std::vector<std::vector<int> >();
  for(auto& entry: dataVecFloat) entry=new std::vector<std::vector<float> >();
  
  

  std::vector<int> counts(static_cast<int>(DataType::kVecFloat)+1,0);  
  for(auto& name : names){
    DataType dataType = name.second;
    auto setAddress = [&counts,&name,tree](auto& data,auto& nameMap,DataType& dataType){
      std::cout <<"setting address  "<<name.first<<" count "<<counts[static_cast<int>(dataType)]<<" data type "<<static_cast<int>(dataType)<<std::endl;
      tree->SetBranchAddress(name.first.c_str(),&data[counts[static_cast<int>(dataType)]]);
      nameMap[name.first]=counts[static_cast<int>(dataType)];
      counts[static_cast<int>(dataType)]++;
    };
    
    if(dataType ==DataType::kFloat) setAddress(dataFloat,dataFloatNameMap,dataType);
    else if(dataType ==DataType::kInt) setAddress(dataInt,dataIntNameMap,dataType);
    else if(dataType ==DataType::kUShort) setAddress(dataUShort,dataUShortNameMap,dataType);
    else if(dataType ==DataType::kULong) setAddress(dataULong,dataULongNameMap,dataType);
    else if(dataType ==DataType::kVecInt) setAddress(dataVecInt,dataVecIntNameMap,dataType);
    else if(dataType ==DataType::kVecFloat) setAddress(dataVecFloat,dataVecFloatNameMap,dataType);
  }
}
  
  
  

struct EleBranches {
  std::vector<float> dataFloat;
  std::vector<int> dataInt;
  std::vector<UShort_t> dataUShort;
  std::vector<ULong64_t> dataULong;
  std::vector<std::vector<int>> dataVecInt;
  std::vector<std::vector<float>> dataVecFloat;
  size_t ptIndex,etaIndex,phiIndex,genEnIndex;

  void init(const EleVecBranches& inTreeBranchData,TTree *tree,int index); 
  void fill(const EleVecBranches& inTreeBranchData,size_t index);

  TLorentzVector p4()const{
    TLorentzVector eleP4;
    eleP4.SetPtEtaPhiM(dataFloat[ptIndex],dataFloat[etaIndex],dataFloat[phiIndex],0.);
    return eleP4;
  }
  float genEn()const{return dataFloat[genEnIndex];}

};

void EleBranches::init(const EleVecBranches& branchData,TTree *tree,int index)
{
  const auto& nrBranchesByType = branchData.nrBranchesByType;
  const auto& names = branchData.names;
  dataFloat.resize(nrBranchesByType[static_cast<int>(DataType::kFloat)]);
  dataInt.resize(nrBranchesByType[static_cast<int>(DataType::kInt)]);
  dataUShort.resize(nrBranchesByType[static_cast<int>(DataType::kUShort)]);
  dataULong.resize(nrBranchesByType[static_cast<int>(DataType::kULong)]);
  dataVecInt.resize(nrBranchesByType[static_cast<int>(DataType::kVecInt)]);
  dataVecFloat.resize(nrBranchesByType[static_cast<int>(DataType::kVecFloat)]);

  std::vector<int> counts(static_cast<int>(DataType::kVecFloat)+1,0);  
  for(auto& name : names){
    auto createBranch = [&counts,tree](auto& data,const DataType dataType,const std::string& name){
      tree->Branch(name.c_str(),&data[counts[static_cast<int>(dataType)]]);
      counts[static_cast<int>(dataType)]++;
    };

    std::string branchName = name.first;
    if(index>=0) branchName = "ele"+std::to_string(index+1)+name.first.substr(3); //starts with ele
    auto dataType = name.second;
    if(dataType ==DataType::kFloat) createBranch(dataFloat,dataType,branchName);
    else if(dataType ==DataType::kInt) createBranch(dataInt,dataType,branchName);
    else if(dataType ==DataType::kUShort) createBranch(dataUShort,dataType,branchName);
    else if(dataType ==DataType::kULong) createBranch(dataULong,dataType,branchName);
    else if(dataType ==DataType::kVecInt) createBranch(dataVecInt,dataType,branchName);
    else if(dataType ==DataType::kVecFloat) createBranch(dataVecFloat,dataType,branchName);
  }
  
}

void EleBranches::fill(const EleVecBranches& inTreeBranchData,size_t index)
{
  auto convert = [index](auto& inData,auto& outData){
    for(size_t dataNr=0;dataNr<inData.size();dataNr++){
      //small fixed when I forgot to fill some branches
      //std::cout <<"dataNr "<<dataNr<<std::endl;
      //std::cout <<"index "<<index <<" size "<<inData[dataNr]->size()<<std::endl;
      if(index<inData[dataNr]->size()){
	outData[dataNr]=(*inData[dataNr])[index];
      }
    }
  };
  
  convert(inTreeBranchData.dataFloat,dataFloat);
  convert(inTreeBranchData.dataInt,dataInt);
  convert(inTreeBranchData.dataUShort,dataUShort);
  convert(inTreeBranchData.dataULong,dataULong);
  convert(inTreeBranchData.dataVecFloat,dataVecFloat);
  convert(inTreeBranchData.dataVecInt,dataVecInt);

  auto getIndex=[](const auto& map,const auto& name)->size_t{
    auto res = map.find(name);
    if(res!=map.end()) return res->second;
    else return 0.;
  };

  ptIndex = getIndex(inTreeBranchData.dataFloatNameMap,"elePt");
  etaIndex = getIndex(inTreeBranchData.dataFloatNameMap,"eleEta");
  phiIndex = getIndex(inTreeBranchData.dataFloatNameMap,"elePhi");
  genEnIndex = getIndex(inTreeBranchData.dataFloatNameMap,"eleGenEn");
  
}

struct ConvertToMassNtup {
  EleVecBranches inBranches;
  EleBranches ele1;
  EleBranches ele2;

  void convert(TTree* inTree,const std::string& outputFilename);
};

void ConvertToMassNtup::convert(TTree* inTree,const std::string& outputFilename)
{
  inBranches.init(inTree);

  TFile* outFile = new TFile(outputFilename.c_str(),"RECREATE");
  TTree* outTree = new TTree("EventMassTree","");

  float mass;
  outTree->Branch("mass",&mass,"mass/F");
  

  ele1.init(inBranches,outTree,0);
  ele2.init(inBranches,outTree,1);

  int nrEntries = inTree->GetEntries();
  for(int entryNr=0;entryNr<nrEntries;entryNr++){
    inTree->GetEntry(entryNr);
    if(entryNr%10000==0) std::cout <<"entryNr "<<entryNr<<" nr Entries "<<nrEntries<<std::endl;
    for(size_t ele1Nr=0;ele1Nr<inBranches.dataFloat.front()->size();ele1Nr++){
      for(size_t ele2Nr=ele1Nr+1;ele2Nr<inBranches.dataFloat.front()->size();ele2Nr++){
	ele1.fill(inBranches,ele1Nr);
	ele2.fill(inBranches,ele2Nr);
	
	auto ele1P4 = ele1.p4();
	auto ele2P4 = ele2.p4();

	mass = (ele1P4+ele2P4).Mag();
	if(ele1P4.Pt()>20 && ele2P4.Pt()>20){
	  outTree->Fill();
	}
      }

    }
  }
  outFile->Write();
  outFile->Close();
 
}
struct NtupSkimmer {
  EleVecBranches inBranches;
  EleBranches ele;
  void convert(TTree* inTree,const std::string& outputFilename);
};

void NtupSkimmer::convert(TTree* inTree,const std::string& outputFilename)
{
  inBranches.init(inTree);

  TFile* outFile = new TFile(outputFilename.c_str(),"RECREATE");
  TTree* outTree = new TTree("EventTree","");

  //float mass;
  float eleNrTree;
  outTree->Branch("eleNr",&eleNrTree,"eleNr/I");
  

  ele.init(inBranches,outTree,-1);

  int nrEntries = inTree->GetEntries();
  for(int entryNr=0;entryNr<nrEntries;entryNr++){
    inTree->GetEntry(entryNr);
    if(entryNr%10000==0) std::cout <<"entryNr "<<entryNr<<" nr Entries "<<nrEntries<<std::endl;
    for(size_t eleNr=0;eleNr<inBranches.dataFloat.front()->size();eleNr++){
      ele.fill(inBranches,eleNr);
      eleNrTree = eleNr;
      
      if(ele.genEn()>0) outTree->Fill();
    }
  }
  outFile->Write();
  outFile->Close();
 
}
