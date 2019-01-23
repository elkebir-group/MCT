/*
 * mctsolverca.h
 *
 *  Created on: 12-dec-2018
 *      Author: N. Aguse
 */

#ifndef MCTSOLVERCA_H
#define MCTSOLVERCA_H

#include "mctsolver.h"

class MCTSolverCA: public MCTSolver
{
public:
  /// Constructor
  ///
  /// @param ctv Input clone trees
  /// @param k Number of clusters
  /// @param timeLimit Time limit in seconds
  /// @param nrMaxRestarts Maximum number of restarts
  MCTSolverCA(const CloneTreeVector& ctv,
              int k,
              int timeLimit,
              int nrMaxRestarts);

  virtual void solve();
  
  virtual std::string getMethodName() const
  {
    char buf[1024];
    snprintf(buf, 1024, "CA-r%d_t%d", _nrMaxRestarts, _timeLimit);
    return buf;
  }
  
  virtual void writeSummaryHeader(std::ostream& out, bool newLine) const;
  
  virtual void writeSummary(std::ostream& out, bool newLine) const;

private:
  void init();
  
  void generateInitialClustering();
  
  /// Update clustering when consensus trees are fixed
  void updateClustering();
  
private:
  const int _nrMaxRestarts;
  int _nrRestarts;
};

#endif // MCTSOLVERCA_H
