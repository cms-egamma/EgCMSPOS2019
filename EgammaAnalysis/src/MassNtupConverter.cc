
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
  std::vector<std::vector<std::vector<int>>*> dataVecFloat;
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
  tree->GetEntry(0);
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

  std::vector<int> counts(static_cast<int>(DataType::kVecFloat)+1,0);  
  for(auto& name : names){
    TBranch* branch = tree->GetBranch(name.first.c_str());
    DataType dataType = name.second;
    auto setAddress = [&counts,branch](auto& data,auto& nameMap,DataType& dataType){
      std::cout <<"setting address  "<<branch->GetName()<<" count "<<counts[static_cast<int>(dataType)]<<" data tpye "<<static_cast<int>(dataType)<<std::endl;
      branch->SetAddress(&data[counts[static_cast<int>(dataType)]]);
      nameMap[branch->GetName()]=counts[static_cast<int>(dataType)];
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
  std::vector<std::vector<int>> dataVecFloat;
  size_t ptIndex,etaIndex,phiIndex;

  void init(const EleVecBranches& inTreeBranchData,TTree *tree,int index); 
  void fill(const EleVecBranches& inTreeBranchData,size_t index);

  TLorentzVector p4(){
    TLorentzVector eleP4;
    eleP4.SetPtEtaPhiM(dataFloat[ptIndex],dataFloat[etaIndex],dataFloat[phiIndex],0.);
    return eleP4;
  }

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

    std::string branchName = "ele"+std::to_string(index+1)+name.first.substr(3); //starts with ele
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
      outData[dataNr]=(*inData[dataNr])[index];
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
  
}

struct ConvertToMassNtup {
  EleVecBranches inBranches;
  EleBranches ele1;
  EleBranches ele2;

  void convert(TTree* inTree,const std::string& outputFilename);
};

void ConvertToMassNtup::convert(TTree* inTree,const std::string& outputFilename)
{
  //so if this is chain, we need to read it first otherwise it'll wipe all our branch addresses
  //yes, I know, this is non obvious
  int nrEntries = inTree->GetEntries();
  std::cout <<"init read tree"<<std::endl;
  inBranches.init(inTree);
  std::cout <<"out read tree"<<std::endl;
  TFile* outFile = new TFile(outputFilename.c_str(),"RECREATE");
  TTree* outTree = new TTree("EventMassTree","");

  float mass;
  outTree->Branch("mass",&mass,"mass/F");
  

  ele1.init(inBranches,outTree,0);
  ele2.init(inBranches,outTree,1);
  std::cout <<"inited "<<std::endl;
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
  outTree->Write();
  outFile->Write();
  outFile->Close();
 
}
