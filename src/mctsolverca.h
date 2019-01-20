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

  virtual void solve();
  
  virtual std::string getMethodName() const
  {
    char buf[1024];
    snprintf(buf, 1024, "CA-%d",_r);
    return buf;
  }

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
