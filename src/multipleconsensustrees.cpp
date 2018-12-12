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
, _clustering(){
  init();
}
void MultipleConsensusTrees::setId(std::string filename){
  _id = filename;
}

void MultipleConsensusTrees::init(){
  //  std::cout << _ctv.size() << std::endl;
  srand(_seed);
  assert(_k >= 1);
  generateInitialClustering();
}

void MultipleConsensusTrees::clearConsensusTrees(){
  for (auto it = _cluster2consensus.begin(); it != _cluster2consensus.end(); it++ ){
    delete _cluster2consensus[it->first];
    _cluster2consensus[it->first] = NULL;
  }
}

void MultipleConsensusTrees::generateConsensusTrees(){
  clearConsensusTrees();
  for (auto it = _cluster2trees.begin(); it != _cluster2trees.end(); it++ ){
    _cluster2consensus[it->first] = new InputGraph(it->second);
    _cluster2consensus[it->first]->SL_graphyc();
  }
}

int MultipleConsensusTrees::getClusteringCost(){
  _clusteringCost = 0;
  
  assert(_cluster2trees.size() != 0);
  assert(_cluster2consensus.size() != 0);
  
  for (auto it = _cluster2trees.begin(); it != _cluster2trees.end(); it++ ){
    _cluster2cost[it->first] =_cluster2consensus[it->first] ->
                                clusteringCost(it->second);
    _cluster2totaltrees[it->first] = it->second.size();
    _clusteringCost += _cluster2consensus[it->first] ->
                            clusteringCost(it->second);
  }
  return _clusteringCost;
}

std::vector<int>  MultipleConsensusTrees::generateInitialClustering(){
  _clustering.clear();
  for (auto it = _cluster2trees.begin(); it != _cluster2trees.end(); it++ ){
    it->second.clear();
  }
  for (const CloneTree& T : _ctv){
    // assign each tree to a random cluster
    int cluster = rand() % _k;
    _clustering.push_back(cluster);
    _cluster2trees[cluster].push_back(T);
  }
  return _clustering;
}

void MultipleConsensusTrees::updateClustering(){
  assert(_cluster2trees.size() != 0);
  for (auto it = _cluster2trees.begin(); it != _cluster2trees.end(); it++ ){
    it->second.clear();
  }
  for(const CloneTree& T: _ctv){
    int bestCluster = 0;
    int bestDist = INT_MAX;
    for (auto it = _cluster2consensus.begin(); it != _cluster2consensus.end(); it++ ){
      int dist = it->second->parentChildDistance(T);
      if (dist < bestDist){
        bestDist = dist;
        bestCluster = it->first;
      }
    }
    _cluster2trees[bestCluster].push_back(T);
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
  for (int i = 0; i < _k; i++){
    _cluster2trees[i].push_back(_ctv[chosenTrees[i]]);
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
    outFile << it->second;
    it++;
    if (it != _cluster2cost.end()){
      outFile << ",";
    }
  }
  outFile << " ";
  for (auto it = _cluster2totaltrees.begin(); it != _cluster2totaltrees.end();){
    outFile << it->second;
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
  //  mct.updateSummary();
  mct.writeClusteringtoFile();
  //  mct.displayConsensusTrees();
  mct.writeSummarytoFile();
  
  std::cout << "the best cost is: " << best.first << " and the items in it are " ;
  for (auto it = best.second.begin(); it != best.second.end(); it++){
    std::cout << *it << ",";
  }
  std::cout << std::endl;
}
