/*
 * multipleconsensustrees.h
 *
 *  Created on: 6-dec-2018
 *      Author: N. Aguse
 */

#ifndef MULTIPLECONSENSUSTREES_H
#define MULTIPLECONSENSUSTREES_H
#include "clonetree.h"
#include "inputgraph.h"
#include <stdlib.h> /* srand, rand */
#include <time.h>

class MultipleConsensusTrees{
public:
  MultipleConsensusTrees(const CloneTreeVector& ctv,
                         int k, int r, int seed);
  
  void setId(std::string filename);
  void updateClustering();
  void generateConsensusTrees();
  void displayConsensusTrees();
  void writeSummarytoFile();
  void updateSummary();
  void writeClusteringtoFile();
  int getClusteringCost();
  std::vector<int> getClustering() ;
  std::vector<int> generateInitialClustering();
  void setClustering(const std::vector<int>& clustering);
  void setConsensusTrees(const std::vector<int>& chosenTrees);
  
private:
  int _k;
  int _r;
  int _seed;
  int _clusteringCost; // distance between each tree in ctv and each of its corresponding consensus tree
  const CloneTreeVector& _ctv;
  std::map<int, CloneTreeVector> _cluster2trees;
  std::map<int, InputGraph*> _cluster2consensus;
  std::map<int, int> _cluster2cost;
  std::map<int, int> _cluster2totaltrees;;
  std::vector<int> _clustering;
  std::string _id;
  
  void init();
  void clearConsensusTrees();
  
  
};

void multipleConsensusTrees(const CloneTreeVector& ctv,
                            int k, int r, int seed, std::string filename);
int obtainClustering(const std::vector<int>& chosenTrees,
                      const CloneTreeVector& ctv, int k);
IntPairIntVector Combination2(const CloneTreeVector& ctv, int k, int totalLen,
                  int currTreeIdx, int numChosen, std::vector<int> chosenTrees);
void bruteForceMCT(const CloneTreeVector& ctv, int k, std::string filename);

#endif // MULTIPLECONSENSUSTREES_H
