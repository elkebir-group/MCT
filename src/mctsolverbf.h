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
  MCTSolverBF(const CloneTreeVector& ctv, int k);
  MCTSolverBF(const CloneTreeVector& ctv, const std::vector<int>& chosenTrees);
  void writeSummarytoFile();
  void solveBF();

private:
  int _k;
  bool next();
  void init();
  std::vector<int> _chosenTrees;

};

void pp(std::vector<int>& curr);
void solveMCTBruteForce(const CloneTreeVector& ctv, int k,
                          std::string filename);

#endif // MCTSOLVERBF_H


