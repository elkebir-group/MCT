/*
 * mctsolverca.cpp
 *
 *  Created on: 12-dec-2018
 *      Author: N. Aguse
 */

#include "mctsolverca.h"

MCTSolverCA::MCTSolverCA(const CloneTreeVector& ctv, int k, int r, int seed)
: MCTSolver(ctv, k)
  , _r(r)
  , _seed(seed){
  init();
}

void MCTSolverCA::init(){
  srand(_seed);
  assert(_k >= 1);
  generateInitialClustering();
}

void MCTSolverCA::updateClustering(){
  assert(_cluster2trees.size() != 0);
  resetClustering();
  for(int i = 0; i < _ctv.size(); i++){
    int bestCluster = 0;
    int bestDist = INT_MAX;
    for (auto it = _cluster2consensus.begin(); it != _cluster2consensus.end(); it++ ){
      int dist = (*it)->parentChildDistance(_ctv[i]);
      if (dist < bestDist){
        bestDist = dist;
        bestCluster =it-_cluster2consensus.begin();
      }
    }
    _cluster2trees[bestCluster].insert(i);
    _clustering.push_back(bestCluster);
  }
}

void pp(std::vector<int>& curr){
  std::cout << "Initial clustering is: " ;
  for (std::vector<int>::iterator it = curr.begin(); it != curr.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';
}

std::vector<int> MCTSolverCA::generateInitialClustering(){
  resetClustering();
  std::vector<int> clustering;
  for (int i = 0; i < getNumTrees(); i++){
    // assign each tree to a random cluster
    int cluster = rand() % _k;
    clustering.push_back(cluster);
  }
//  pp(clustering);
  setClustering(clustering);

  return clustering;
}

void MCTSolverCA::solve(){
  int minCost = INT_MAX;
  std::vector<int> bestClustering;
  for (int i = 0; i < _r; i++){
    IntVector currClustering = generateInitialClustering();

    int currCost = -1;
    while(true){
      generateParentChildGraphs();
      generateConsensusTrees();
      updateClustering();
      int newCost = getClusteringCost();
      const IntVector& newClustering = getClustering();
      if (currClustering == newClustering){
        currCost = newCost;
        break;
      }
      currCost = newCost;
      currClustering = newClustering;
    }
    if (currCost < minCost){
      minCost = currCost;
      bestClustering = currClustering;
    }
  }
  //  std::cout << "min cost achieved: " << minCost << std::endl;
  
  setClustering(bestClustering);
  generateParentChildGraphs();
  generateConsensusTrees();
  updateClusteringCost();
}

