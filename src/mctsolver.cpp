/*
 * mctsolver.cpp
 *
 *  Created on: 12-dec-2018
 *      Author: N. Aguse
 */

#include "mctsolver.h"

MCTSolver::MCTSolver(const CloneTreeVector& ctv, int k)
: _ctv(ctv)
, _k(k)
, _clusteringCost(-1)
, _cluster2trees(k)
, _cluster2consensus(k)
, _cluster2cost(k)
, _cluster2totaltrees(k)
, _clustering()
, _id(){
}

MCTSolver::~MCTSolver(){
  clearConsensusTrees();
}

void MCTSolver::run(MCTSolver& solver,
                    const std::string& outputPrefix){
  solver.solve();
  solver.setId(outputPrefix);
  solver.writeClusteringtoFile();
  solver.writeSummarytoFile();
  // TODO: write consensus trees to separate files
  solver.displayConsensusTrees();
}

void MCTSolver::displayConsensusTrees() const{
  for (int i = 0; i < _k; ++i){
    auto edges = _cluster2consensus[i]->getSelectedEdgeList();
    for (const auto& edge : edges){
      std::cerr << edge.first << " " << edge.second << std::endl;
    }
    std::cerr << std::endl << std::endl;
  }
}

void MCTSolver::resetClustering(){
  _clustering.clear();
  _cluster2trees = IntSetVector(_k);
}

void MCTSolver::clearConsensusTrees(){
  for (auto it = _cluster2consensus.begin(); it != _cluster2consensus.end(); it++ ){
    delete *it;
    *it = NULL;
  }
}

void MCTSolver::generateConsensusTrees(){
  clearConsensusTrees();
  assert(_cluster2trees.size() == _k);
  
  int clusterIdx = 0;
  for (const IntSet& trees : _cluster2trees){
    std::vector<CloneTree> ctvcopy;
    
    for (int treeIdx : trees){
      ctvcopy.push_back(_ctv[treeIdx]);
    }
    _cluster2consensus[clusterIdx] = new ParentChildGraph(ctvcopy);
    _cluster2consensus[clusterIdx]->SL_graphyc();
    
    ++clusterIdx;
  }
}

void MCTSolver::updateClusteringCost(){
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
}

void MCTSolver::setClustering(const std::vector<int>& clustering){
  assert(clustering.size() == _ctv.size());
  _clustering = clustering;
  
  // update _cluster2trees
  const int n = _ctv.size();
  _cluster2trees = IntSetVector(_k);
  for (int i = 0; i < n; ++i)
  {
    _cluster2trees[clustering[i]].insert(i);
  }
  
  generateConsensusTrees();
  updateClusteringCost();
}

void MCTSolver::setId(const std::string& filename){
  _id = filename;
}

void MCTSolver::writeClusteringtoFile() const{
  assert(!_id.empty());
  std::ofstream outFile(_id + "k" + std::to_string(_k)+ "clustering.txt");
  for (const auto &e : _clustering) outFile << e << "\n";
}
