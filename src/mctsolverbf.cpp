/*
 * mctsolverbf.cpp
 *
 *  Created on: 12-dec-2018
 *      Author: N. Aguse
 */

#include "mctsolverbf.h"


MCTSolverBF::MCTSolverBF(const CloneTreeVector& ctv, int k)
: MCTSolver(ctv, k){
}

void MCTSolverBF::writeSummary(std::ostream& out) const{
  out << "bruteforce" << " " << _k << " " << getClusteringCost() << " ";
  for (auto it = getCluster2Cost().begin(); it != getCluster2Cost().end();){
    out << *it;
    it++;
    if (it != getCluster2Cost().end()){
      out << ",";
    }
  }
  out << " ";
  for (auto it = getCluster2TotalTrees().begin(); it != getCluster2TotalTrees().end();){
    out << *it;
    it++;
    if (it != getCluster2TotalTrees().end()){
      out << ",";
    }
  }
  out << std::endl;
}

void MCTSolverBF::solve(){
  const int n = _ctv.size();
  
  IntVector clustering(n, 0);
  IntVector bestClustering;
  int bestClusteringCost = INT_MAX;
  
  do
  {
    setClustering(clustering);
    
    if (getClusteringCost() < bestClusteringCost)
    {
      bestClusteringCost = getClusteringCost();
      bestClustering = clustering;
    }
  } while (next(clustering));
  
  setClustering(bestClustering);
}

bool MCTSolverBF::next(IntVector& clustering){
  const int n = _ctv.size();

  // 1. identify first position 'idx' with value smaller than _k - 1 (to increment)
  int idx = 0;
  for (; idx < n; ++idx){
    if (clustering[idx] < _k - 1)
    {
      break;
    }
  }

  // 2. reset positions smaller than idx to 0
  for (int i = 0; i < idx; ++i)
  {
    clustering[i] = 0;
  }
  
  // 3. update clustering if possible (idx < n)
  if (idx == n)
  {
    return false;
  }
  else
  {
    ++clustering[idx];
    return true;
  }
}

void pp(std::vector<int>& curr){
  std::cout << "Numbers in Chosen trees are: " ;
  for (std::vector<int>::iterator it = curr.begin(); it != curr.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';
}
