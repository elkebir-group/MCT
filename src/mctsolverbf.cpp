/*
 * mctsolverbf.cpp
 *
 *  Created on: 12-dec-2018
 *      Author: N. Aguse
 */

#include "mctsolverbf.h"


MCTSolverBF::MCTSolverBF(const CloneTreeVector& ctv, int k)
: MCTSolver(ctv, k)
  , _k(k)
{
  init();
}

MCTSolverBF::MCTSolverBF(const CloneTreeVector& ctv, const std::vector<int>& chosenTrees)
: MCTSolver(ctv, chosenTrees.size()){
  setConsensusTrees(chosenTrees);
  getClusteringCost();
  init();
}

void MCTSolverBF::init(){
  for (int i = 0; i < _k; i++){
    _chosenTrees.push_back(i);
  }
}

void MCTSolverBF::writeSummarytoFile(){
  std::string id = getId();
  assert(!id.empty());
  std::ofstream outFile;
  std::cout << id +"_BFsummary.csv" << std::endl;
  outFile.open(id +"_BFsummary.csv", std::ios_base::app);
  outFile << "bruteforce" << " " << _k << " " << getClusteringCost() << " ";
  for (auto it = getCluster2Cost().begin(); it != getCluster2Cost().end();){
    outFile << *it;
    it++;
    if (it != getCluster2Cost().end()){
      outFile << ",";
    }
  }
  outFile << " ";
  for (auto it = getCluster2TotalTrees().begin(); it != getCluster2TotalTrees().end();){
    outFile << *it;
    it++;
    if (it != getCluster2TotalTrees().end()){
      outFile << ",";
    }
  }
  outFile << std::endl;
}

void MCTSolverBF::solveBF(){
  assert(_chosenTrees.size() == _k);
  setConsensusTrees(_chosenTrees);
  int mincost = getClusteringCost();
  std::vector<int> minChosenTrees = _chosenTrees;
//  pp(_chosenTrees);
  while(next()){
//    pp(_chosenTrees);
    setConsensusTrees(_chosenTrees);
    int cost = getClusteringCost();
    if (cost < mincost){
      mincost = cost;
      minChosenTrees = _chosenTrees;
    }
  }
//  pp(minChosenTrees);
  setConsensusTrees(minChosenTrees);
  getClusteringCost();
  writeSummarytoFile();
  writeClusteringtoFile();
}

bool MCTSolverBF::next(){
  int maxnum = getNumTrees()-1;
  if (*_chosenTrees.rbegin() != maxnum){
    (*_chosenTrees.rbegin())++;
    return true;
  }
  else{
    int endval = maxnum-1;
    for (auto rit = _chosenTrees.rbegin()+1; rit != _chosenTrees.rend(); rit++){
      if (*rit != endval){
        (*rit)++;
        int newval = *rit;
        int offset = _chosenTrees.rend()-rit;
        for (auto it = _chosenTrees.begin() +offset; it != _chosenTrees.end(); it++){
          *it = ++newval;
        }
        return true;
      }
      endval--;
    }
    return false;
  }
}

void pp(std::vector<int>& curr){
  std::cout << "Numbers in Chosen trees are: " ;
  for (std::vector<int>::iterator it = curr.begin(); it != curr.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';
}

void solveMCTBruteForce(const CloneTreeVector& ctv, int k, std::string filename){
  MCTSolverBF mct(ctv, k);
  mct.setId(filename);
  mct.solveBF();
  mct.clearConsensusTrees();
}

