/*
 * mctsolverbf.h
 *
 *  Created on: 12-dec-2018
 *      Author: N. Aguse
 */

#ifndef MCTSOLVERBF_H
#define MCTSOLVERBF_H

#include "mctsolver.h"

class MCTSolverBF: public MCTSolver{
public:
  /// Constructor
  ///
  /// @param ctv Input clone trees
  /// @param k Number of clusters
  MCTSolverBF(const CloneTreeVector& ctv, int k);

  virtual void writeSummarytoFile() const;
  virtual void solve();

private:
  bool next(IntVector& clustering);
};

void pp(std::vector<int>& curr);

#endif // MCTSOLVERBF_H
