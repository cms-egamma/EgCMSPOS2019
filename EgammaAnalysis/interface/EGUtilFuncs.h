
#include <string>
#include <vector>

//a series of utility functions
//most were written over a decade ago and thus could do with robustness and modernisation so are not good examples

class EGUtilFuncs { 

private:
  EGUtilFuncs(){}

public:
  static void readFilelistFromPattern(const std::string& filelistPattern,std::vector<std::string> &filenames);
  static void readFilelistFromFile(const std::string& fileListName,std::vector<std::string> &filenames);
  static void readFilelist(std::string fileListName,std::vector<std::string> &filenames,int nrJobs=1,int jobNr=1,int verbose=2); 
  static void readFilelist(std::vector<std::string> fileListName,std::vector<std::string> &filenames,int nrJobs=1,int jobNr=1,int verbose=2);
  static void splitFilelist(int nrJobs,int jobNr,std::vector<std::string>& filenames);
  static void splitFilelistMixed(int nrJobs,int jobNr,std::vector<std::string>& filenames);//files are maxmally mixed between jobs
  static void splitFilelistConsecutive(int nrJobs,int jobNr,std::vector<std::string>& filenames);//each job has files that were consecutive to each other
  static int nrFilesInJob(int nrFiles,int nrJobs,int jobNr);
  static int nrFilesInPreviousJobs(int nrFiles,int nrJobs,int jobNr);
  static void addJobNrToFilename(char* filename,int jobNr);
  static void addJobNrToFilename(std::string& filename,int jobNr);
};
