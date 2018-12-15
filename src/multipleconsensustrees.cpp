/*
 * multipleconsensustrees.cpp
 *
 *  Created on: 6-dec-2018
 *      Author: N. Aguse
 */

#include "multipleconsensustrees.h"


MultipleConsensusTrees::MultipleConsensusTrees(const CloneTreeVector& ctv,
                                               int k, int r, int seed)
:  _k(k)
, _r(r)
, _seed(seed)
, _clusteringCost(-1)
, _ctv(ctv)
, _clustering()
, _cluster2trees(k)
, _cluster2consensus(k)
, _cluster2cost(k)
, _cluster2totaltrees(k){
  init();
}
void MultipleConsensusTrees::setId(std::string filename){
  _id = filename;
}

void MultipleConsensusTrees::init(){
  //  std::cout << _ctv.size() << std::endl;
  srand(_seed);
  assert(_k >= 1);
//  for (int i = 0; i < _k; i++){
//    _cluster2trees[i] = s
//  }
  generateInitialClustering();
}

void MultipleConsensusTrees::resetClustering(){
  _clustering.clear();
  for (auto it = _cluster2trees.begin(); it != _cluster2trees.end(); it++ ){
    (*it).clear();
  }
}

void MultipleConsensusTrees::clearConsensusTrees(){
  for (auto it = _cluster2consensus.begin(); it != _cluster2consensus.end(); it++ ){
    delete *it;
    *it = NULL;
  }
}

void MultipleConsensusTrees::generateConsensusTrees(){
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

int MultipleConsensusTrees::getClusteringCost(){
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

std::vector<int>  MultipleConsensusTrees::generateInitialClustering(){
  resetClustering();
  
  for (int i = 0; i < _ctv.size(); i++){
    // assign each tree to a random cluster
    int cluster = rand() % _k;
    _clustering.push_back(cluster);
    _cluster2trees[cluster].insert(i);
  }
  return _clustering;
}

void MultipleConsensusTrees::updateClustering(){
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

std::vector<int> MultipleConsensusTrees::getClustering(){
  assert(_clustering.size() == _ctv.size());
  return _clustering;
}

void MultipleConsensusTrees::setClustering(const std::vector<int>& clustering){
  assert(clustering.size() == _ctv.size());
  _clustering = clustering;
}

void MultipleConsensusTrees::writeClusteringtoFile(){
  assert(!_id.empty());
  std::ofstream outFile(_id + "k" + std::to_string(_k)+ "clustering.txt");
  for (const auto &e : _clustering) outFile << e << "\n";
}

void MultipleConsensusTrees::setConsensusTrees(const std::vector<int>& chosenTrees){
  resetClustering();
  for (int i = 0; i < _k; i++){
    _cluster2trees[i].insert(chosenTrees[i]);
  }
  generateConsensusTrees();
  updateClustering();
}

void MultipleConsensusTrees::writeSummarytoFile(){
  assert(!_id.empty());
  std::ofstream outFile;
  std::cout << _id + "summary.csv" << std::endl;
  outFile.open(_id + "summary.csv", std::ios_base::app);
  
//  std::ofstream outFile("summary.csv");
//  outfile.open("test.txt", std::ios_base::app);
  outFile << _r << " " << _k << " " << _clusteringCost << " ";
  for (auto it = _cluster2cost.begin(); it != _cluster2cost.end();){
//    std::cout << "cost of cluster " << it->first << " is: "<< it->second << std::endl;
    outFile << *it;
    it++;
    if (it != _cluster2cost.end()){
      outFile << ",";
    }
  }
  outFile << " ";
  for (auto it = _cluster2totaltrees.begin(); it != _cluster2totaltrees.end();){
    outFile << *it;
    it++;
    if (it != _cluster2totaltrees.end()){
      outFile << ",";
    }
  }
  outFile << std::endl;
}

void multipleConsensusTrees(const CloneTreeVector& ctv,
                            int k, int r, int seed, std::string filename){
  
  MultipleConsensusTrees mct(ctv, std::min<int>(k, ctv.size()), r, seed);
  mct.setId(filename);
  int minCost = INT_MAX;
  std::vector<int> bestClustering;
  for (int i = 0; i < r; i++){
    std::vector<int> currClustering = mct.generateInitialClustering();
    int currCost;
    while(true){
      mct.generateConsensusTrees();
      mct.updateClustering();
      int newCost = mct.getClusteringCost();
      const std::vector<int>& newClustering = mct.getClustering();
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
  
  mct.setClustering(bestClustering);
  mct.generateConsensusTrees();
  mct.getClusteringCost();
  mct.writeClusteringtoFile();
  mct.writeSummarytoFile();
  mct.clearConsensusTrees();

}

int obtainClustering(const std::vector<int>& chosenTrees,
                      const CloneTreeVector& ctv, int k){
//  std::cout << "the chosen trees are: ";
//  for (auto it = chosenTrees.begin(); it != chosenTrees.end(); it++){
//    std::cout << *it << " , ";
//  }
//  std::cout << std::endl;
//  int ret = 0;
//  for (auto it = chosenTrees.begin(); it != chosenTrees.end(); it++){
//    ret -= *it;
//  }
//  return ret;
  MultipleConsensusTrees mct(ctv, k, 0, 0);
  mct.setConsensusTrees(chosenTrees);
  int cost = mct.getClusteringCost();
  mct.clearConsensusTrees();
  return cost;
}

IntPairIntVector Combination2(const CloneTreeVector& ctv, int k, int totalLen,
                  int currTreeIdx, int numChosen, std::vector<int> chosenTrees){
  // we have filled in the required
  if (numChosen == k){
    int cost = obtainClustering(chosenTrees, ctv, k);
    IntPairIntVector ret = std::make_pair(cost, chosenTrees);
    return ret;
  }
  else if (currTreeIdx >= totalLen){
    return std::make_pair(INT_MAX, chosenTrees);
  }
  else{
    // two cases, either the current tree is chosen as consensus or not
    IntPairIntVector cost1 = Combination2(ctv, k, totalLen, currTreeIdx+1,
                                          numChosen, chosenTrees);
    chosenTrees.push_back(currTreeIdx);
    IntPairIntVector cost2 = Combination2(ctv, k, totalLen, currTreeIdx+1,
                                          numChosen+1, chosenTrees);
    if (cost1.first < cost2.first){
      return cost1;
    }
    return cost2;
  }
  
}

void bruteForceMCT(const CloneTreeVector& ctv, int k, std::string filename){
  std::vector<int> chosenTrees;
  IntPairIntVector best = Combination2(ctv, std::min<int>(k, ctv.size()), ctv.size(), 0, 0, chosenTrees);
  
  MultipleConsensusTrees mct(ctv, std::min<int>(k, ctv.size()), 0, 0);
  mct.setId(filename);
  mct.setConsensusTrees(best.second);
  mct.generateConsensusTrees();
  mct.getClusteringCost();
  assert(mct.getClusteringCost() == best.first);
  mct.writeClusteringtoFile();
  mct.writeSummarytoFile();
  mct.clearConsensusTrees();
  std::cout << "the best cost is: " << best.first << " and the items in it are " ;
  for (auto it = best.second.begin(); it != best.second.end(); it++){
    std::cout << *it << ",";
  }
  std::cout << std::endl;
}
