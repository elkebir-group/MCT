/*
 * mctsolverca.h
 *
 *  Created on: 12-dec-2018
 *      Author: N. Aguse
 */

#ifndef MCTSOLVERCA_H
#define MCTSOLVERCA_H
#include "mctsolver.h"

class MCTSolverCA: public MCTSolver{
public:
  MCTSolverCA(const CloneTreeVector& ctv, int k, int r, int seed);
  
  virtual void writeSummarytoFile() const;
  
  virtual void solve();

private:
  std::vector<int> generateInitialClustering();
  
  /// Update clustering when consensus trees are fixed
  void updateClustering();
  
private:
  int _r;
  int _seed;
  void init();
};

#endif // MCTSOLVERCA_H
