/*
 * mctsolverbf.cpp
 *
 *  Created on: 12-dec-2018
 *      Author: N. Aguse
 */

#include "mctsolverbf.h"


MCTSolverBF::MCTSolverBF(const CloneTreeVector& ctv, int k, int timelimit)
: MCTSolver(ctv, k, timelimit)
{
}

void MCTSolverBF::solve(){
  const int n = _ctv.size();
  
  IntVector clustering(n, 0);
  IntVector bestClustering;
  int bestClusteringCost = INT_MAX;
  
  seconds_type t(_timelimit);
  auto start = std::chrono::high_resolution_clock::now();
  
  do
  {
    setClustering(clustering);
    
    if (getClusteringCost() < bestClusteringCost)
    {
      bestClusteringCost = getClusteringCost();
      bestClustering = clustering;
    }
    auto finish = std::chrono::high_resolution_clock::now();
    
    if (finish - start >= t){
      break;
    }
    
  } while (next(clustering));
  
  setClustering(bestClustering);
}

bool MCTSolverBF::next(IntVector& clustering){
  const int n = _ctv.size();
  while(true){
    IntVector check(_k,0);
    // 1. identify first position 'idx' with value smaller than _k - 1 (to increment)
    int idx = 0;
    for (; idx < n; ++idx){
      if (clustering[idx] < _k - 1){
        break;
      }
    }
    // 2. reset positions smaller than idx to 0
    for (int i = 0; i < idx; ++i){
      clustering[i] = 0;
    }
    // 3. update clustering if possible (idx < n)
    if (idx == n) {
      return false;
    }
    else {
      ++clustering[idx];
    }
    for (int i: clustering){
      check[i]++;
    }
    bool valid = true;
    for(int i: check){
      if (i == 0){
        valid = false;
      }
    }
    if(valid) break;
  }
  return true;
}

void pp(std::vector<int>& curr){
  std::cout << "Numbers in Chosen trees are: " ;
  for (std::vector<int>::iterator it = curr.begin(); it != curr.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';
}
