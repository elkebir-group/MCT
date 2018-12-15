/*
 * mctsolver.cpp
 *
 *  Created on: 12-dec-2018
 *      Author: N. Aguse
 */

#include "mctsolver.h"
//
//MCTSolver::MCTSolver()
//:  _k()
//, _clusteringCost(-1)
//, _ctv()
//, _clustering()
//, _cluster2trees()
//, _cluster2consensus()
//, _cluster2cost()
//, _cluster2totaltrees(){
//
//}

MCTSolver::MCTSolver(const CloneTreeVector& ctv, int k)
:  _k(k)
, _clusteringCost(-1)
, _ctv(ctv)
, _clustering()
, _cluster2trees(k)
, _cluster2consensus(k)
, _cluster2cost(k)
, _cluster2totaltrees(k)
, _numTrees(ctv.size()){

}


void MCTSolver::resetClustering(){
  _clustering.clear();
  for (auto it = _cluster2trees.begin(); it != _cluster2trees.end(); it++ ){
    (*it).clear();
  }
}

void MCTSolver::clearConsensusTrees(){
  for (auto it = _cluster2consensus.begin(); it != _cluster2consensus.end(); it++ ){
    delete *it;
    *it = NULL;
  }
}

void MCTSolver::generateConsensusTrees(){
  clearConsensusTrees();
  assert(_cluster2trees.size() != 0);
  for (auto it = _cluster2trees.begin(); it != _cluster2trees.end(); it++ ){
    std::vector<CloneTree> ctvcopy;
    for (int idx: *it){
      ctvcopy.push_back(_ctv[idx]);
    }
    _cluster2consensus[it - _cluster2trees.begin()] = new ParentChildGraph(ctvcopy);
    _cluster2consensus[it - _cluster2trees.begin()]->SL_graphyc();
  }
}

int MCTSolver::getClusteringCost(){
  _clusteringCost = 0;
  
  assert(_cluster2trees.size() != 0);
  assert(_cluster2consensus.size() != 0);
  
  for (auto it = _cluster2trees.begin(); it != _cluster2trees.end(); it++ ){
    std::vector<CloneTree> ctvcopy;
    for (int idx: *it){
      ctvcopy.push_back(_ctv[idx]);
    }
    int idx = it - _cluster2trees.begin();
    _cluster2cost[idx] = _cluster2consensus[idx] ->
    clusteringCost(ctvcopy);
    _cluster2totaltrees[idx] = ctvcopy.size();
    _clusteringCost += _cluster2cost[idx];
  }
  return _clusteringCost;
}

void MCTSolver::updateClustering(){
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

std::vector<int>& MCTSolver::getClustering(){
  assert(_clustering.size() == _ctv.size());
  return _clustering;
}

void MCTSolver::setClustering(const std::vector<int>& clustering){
  assert(clustering.size() == _ctv.size());
  _clustering = clustering;
}

void MCTSolver::setId(std::string filename){
  _id = filename;
}

std::string MCTSolver::getId(){
  return _id;
}

void MCTSolver::writeClusteringtoFile(){
  assert(!_id.empty());
  std::ofstream outFile(_id + "k" + std::to_string(_k)+ "clustering.txt");
  for (const auto &e : _clustering) outFile << e << "\n";
}

void MCTSolver::setConsensusTrees(const std::vector<int>& chosenTrees){
  resetClustering();
  for (int i = 0; i < _k; i++){
    _cluster2trees[i].insert(chosenTrees[i]);
  }
  generateConsensusTrees();
  updateClustering();
}

void MCTSolver::setCluster2Trees(std::vector<std::set<int>> cluster2trees){
  _cluster2trees = cluster2trees;
}

std::vector<int> MCTSolver::getCluster2Cost(){
  return _cluster2cost;
}
std::vector<int> MCTSolver::getCluster2TotalTrees(){
  return _cluster2totaltrees;
}

int MCTSolver::getNumTrees(){
  return _numTrees;
}
