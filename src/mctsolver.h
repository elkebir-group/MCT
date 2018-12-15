/*
 * mctsolver.h
 *
 *  Created on: 12-dec-2018
 *      Author: N. Aguse
 */

#ifndef MCTSOLVER_H
#define MCTSOLVER_H
#include "clonetree.h"
#include "inputgraph.h"
#include <stdlib.h> /* srand, rand */
#include <time.h>

class MCTSolver{
public:
  MCTSolver(const CloneTreeVector& ctv, int k);
  
  void setId(std::string filename);
  void updateClustering();
  void generateConsensusTrees();
  void displayConsensusTrees();
  void writeClusteringtoFile();
  int getClusteringCost();
  std::vector<int>& getClustering();
  void setClustering(const std::vector<int>& clustering);
  void setConsensusTrees(const std::vector<int>& chosenTrees);
  void clearConsensusTrees();
  std::vector<int> getCluster2Cost();
  std::vector<int> getCluster2TotalTrees();
  int getNumTrees();
  void setCluster2Trees(std::vector<std::set<int>> cluster2trees);
  void resetClustering();
  std::string getId();
  
private:
  int _k;
  int _clusteringCost; // distance between each tree in ctv and each of its corresponding consensus tree
  const CloneTreeVector& _ctv;
  std::vector<std::set<int>> _cluster2trees;
  std::vector<ParentChildGraph*> _cluster2consensus;
  std::vector<int> _cluster2cost;
  std::vector<int> _cluster2totaltrees;
  std::vector<int> _clustering;
  std::string _id;
  int _numTrees;
  

  
};

#endif // MCTSOLVER_H


