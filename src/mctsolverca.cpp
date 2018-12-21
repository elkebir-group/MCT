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
  , _k(k)
  , _seed(seed){
  init();
}

void MCTSolverCA::init(){
  srand(_seed);
  assert(_k >= 1);
  generateInitialClustering();
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
  std::vector<std::set<int>> cluster2trees(_k);
  for (int i = 0; i < getNumTrees(); i++){
    // assign each tree to a random cluster
    int cluster = rand() % _k;
    clustering.push_back(cluster);
    cluster2trees[cluster].insert(i);
  }
//  pp(clustering);
  setClustering(clustering);
  setCluster2Trees(cluster2trees);
  return clustering;
}

void MCTSolverCA::writeSummarytoFile(){
  std::string id = getId();
  assert(!id.empty());
  std::ofstream outFile;
  std::cout << id +"_k"+ std::to_string(_k)+ "_CAsummary.csv" << std::endl;
  outFile.open(id + "_CAsummary.csv", std::ios_base::app);
  
  //  std::ofstream outFile("summary.csv");
  //  outfile.open("test.txt", std::ios_base::app);
  outFile << _r << " " << _k << " " << getClusteringCost() << " ";
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

void solveMCTCoordinateAscend(const CloneTreeVector& ctv,
                            int k, int r, int seed, std::string filename){
  
  MCTSolverCA mct(ctv, std::min<int>(k, ctv.size()), r, seed);
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

