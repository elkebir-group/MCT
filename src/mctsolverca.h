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
  std::vector<int> generateInitialClustering();
  void writeSummarytoFile();
  
private:
  int _k;
  int _r;
  int _seed;
  void init();

  
};

void solveMCTCoordinateAscend(const CloneTreeVector& ctv, int k,
                              int r, int seed, std::string filename);

#endif // MCTSOLVERCA_H


