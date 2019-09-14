
class VarBase {
  

};


class EleVecBranches {
  std::vector<std::vector<float> > dataFloat;
  std::vector<std::vector<int> >dataInt;
  std::vector<std::vector<std::vector<int>>> dataVecInt;
  std::vector<std::vector<std::vector<int>>> dataVecFloat;
  std::vector<std::string,std::string> names;

  void init(TTree* tree); 
  void setBranchAddreses(TTree *tree);
};

class EleBranches {
  std::vector<float> dataFloat;
  std::vector<int> dataInt;
  std::vector<std::vector<int>> dataVecInt;
  std::vector<std::vector<int>> dataVecFloat;
  std::vector<std::string,std::string> names;
};

class ConvertToMassNtup {



}
