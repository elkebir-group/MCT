/*
 * mctsolver.h
 *
 *  Created on: 12-dec-2018
 *      Author: N. Aguse
 */

#ifndef MCTSOLVER_H
#define MCTSOLVER_H

#include "clonetree.h"
#include "parentchildgraph.h"

class MCTSolver{
public:
  /// Constructor
  ///
  /// @param ctv Input clone trees
  /// @param k Number of clusters
  MCTSolver(const CloneTreeVector& ctv, int k);
  
  /// Destructor
  virtual ~MCTSolver();
  
  void setId(const std::string& filename);
  const std::string& getId() const
  {
    return _id;
  }
  
  void generateConsensusTrees();
  
  void displayConsensusTrees() const;
  
  void writeClusteringtoFile() const;
  
  int getClusteringCost() const
  {
    return _clusteringCost;
  }
  
  const IntVector& getClustering() const
  {
    return _clustering;
  }
  
  void setClustering(const std::vector<int>& clustering);

  void clearConsensusTrees();

  const IntVector& getCluster2Cost() const
  {
    return _cluster2cost;
  }
  
  const IntVector& getCluster2TotalTrees() const
  {
    return _cluster2totaltrees;
  }

  int getNumTrees() const
  {
    return _ctv.size();
  }
  
//  virtual void writeSummary(const std::ostream& out) const = 0;
  virtual void writeSummarytoFile() const = 0;
  
  virtual void solve() = 0;
  
  static void run(MCTSolver& solver,
                  const std::string& outputPrefix);
  
protected:
  void updateClusteringCost();
  
  void resetClustering();
  
protected:
  /// Input trees
  const CloneTreeVector& _ctv;
  /// Number of clusters
  const int _k;
  /// Sum of distances between each input tree and its corresponding consensus tree
  int _clusteringCost;
  /// Set of input tree indices for each cluster
  IntSetVector _cluster2trees;
  /// Consensus tree of each cluster
  std::vector<ParentChildGraph*> _cluster2consensus;
  /// Total distance of the trees to the consensus tree in each cluster
  std::vector<int> _cluster2cost;
  /// Number of trees in each cluster
  std::vector<int> _cluster2totaltrees;
  /// Input tree index to cluster index
  std::vector<int> _clustering;
  /// Output prefix
  std::string _id;
};

#endif // MCTSOLVER_H
