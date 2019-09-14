#include "EgCMSPOS2019/EgammaAnalysis/interface/EGUtilFuncs.h"

#include <iostream>
#include <fstream>

#include <cstring>

void EGUtilFuncs::readFilelistFromPattern(const std::string& filelistPattern,std::vector<std::string> &filenames)
{
  char filename[512];
  auto file = popen(("ls "+filelistPattern).c_str(),"r");
  while(file && !std::feof(file)){
    if(std::fgets(filename,sizeof(filename),file)==NULL) break; //otherwise will read the last entry twice
    std::string filenameStr(filename);
    filenameStr.pop_back(); //removing end of line character
    filenames.push_back(filenameStr);
  }
  std::fclose(file);
}

void EGUtilFuncs::readFilelistFromFile(const std::string& fileListName,std::vector<std::string> &filenames)
{
  std::ifstream fileList(fileListName.c_str());
  char filename[512];
  while(!fileList.eof() && fileList.is_open()){
    fileList.getline(filename,511);
    if(strcmp(filename,"")!=0 && filename[0]!='#'){
      filenames.push_back(filename);
    }
  }
  fileList.close();
}

void EGUtilFuncs::readFilelist(std::string fileListName,std::vector<std::string> &filenames,int nrJobs,int jobNr,int verbose)
{
  readFilelist(std::vector<std::string>{fileListName},filenames,nrJobs,jobNr,verbose);
}

void EGUtilFuncs::readFilelist(std::vector<std::string> fileListNames,std::vector<std::string> &filenames,int nrJobs,int jobNr,int verbose)  
{
  for(const auto& fileListName : fileListNames){
    const char* extension = strstr(fileListName.c_str(),".root");
    if(fileListName.find("*")!=std::string::npos || fileListName.find("?")!=std::string::npos  ){ 
      readFilelistFromPattern(fileListName,filenames);
    }else if(extension!=NULL && strlen(extension)==5){ //file is a root file, process it
      filenames.push_back(fileListName);
    }else{
      readFilelistFromFile(fileListName,filenames);
    }
  }
  
  if(nrJobs!=1) splitFilelist(nrJobs,jobNr,filenames);
  if(verbose>=2) for(unsigned i=0;i<filenames.size();i++) std::cout <<"filename = "<<filenames[i].c_str()<<std::endl;
  if(verbose) std::cout <<"nr files: "<<filenames.size()<<std::endl; 
}

void EGUtilFuncs::splitFilelistConsecutive(int nrJobs,int jobNr,std::vector<std::string>& filenames)
{
  if(jobNr>nrJobs || nrJobs<1){
    std::vector<std::string>().swap(filenames);
    return;
  }
  if(jobNr==0) jobNr=nrJobs; //setting 0th job to be the same as last job

  int nrFiles = filenames.size();
  
std::vector<std::string>::iterator start,end;
  start = filenames.begin() + nrFilesInPreviousJobs(nrFiles,nrJobs,jobNr);
  end = start+nrFilesInJob(nrFiles,nrJobs,jobNr); // as it should be just past the indx we want
  if(jobNr==nrJobs && end!=filenames.end()) std::cout <<"EGUtilFuncs::splitFilelist : Warning : last job does not equal end of filenames end"<<std::endl;
  std::vector<std::string> jobFilenames(start,end);
  filenames.swap(jobFilenames);
}

void EGUtilFuncs::splitFilelist(int nrJobs,int jobNr,std::vector<std::string>& filenames)
{
  splitFilelistMixed(nrJobs,jobNr,filenames);
}

void EGUtilFuncs::splitFilelistMixed(int nrJobs,int jobNr,std::vector<std::string>& filenames)
{
  if(jobNr>nrJobs || nrJobs<1){
    std::vector<std::string>().swap(filenames);
    return;
  }
  
  std::vector<std::string> outFilenamesTmp;
  for(size_t fileNr=0;fileNr<filenames.size();fileNr++){
    if( (fileNr+jobNr)%nrJobs==0){
      outFilenamesTmp.push_back(filenames[fileNr]);
    }
  }
  filenames.swap(outFilenamesTmp);
}
   
 

int EGUtilFuncs::nrFilesInJob(int nrFiles,int nrJobs,int jobNr)
{
  int nrFilesLeftOver = nrFiles%nrJobs;
  int nrFilesInJob = nrFiles/nrJobs; //intentionally throwing away remainder
  if(jobNr<= nrFilesLeftOver) nrFilesInJob++;
  return nrFilesInJob;
}

int EGUtilFuncs::nrFilesInPreviousJobs(int nrFiles,int nrJobs,int jobNr)
{
  int totNrFiles =0;
  for(int job = jobNr-1;job>0;job--){
    totNrFiles+=nrFilesInJob(nrFiles,nrJobs,job);
  }
  return totNrFiles;
}
