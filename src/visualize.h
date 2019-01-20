/*
 * visualize.h
 *
 *  Created on: 17-jan-2019
 *      Author: M. El-Kebir
 */

#ifndef VISUALIZE_H
#define VISUALIZE_H

#include "utils.h"
#include "clonetree.h"
#include "parentchildgraph.h"

class Visualize
{
public:
  Visualize(const CloneTreeVector& ctv,
            const IntVector& clustering,
            int k);
  
  ~Visualize();
  
  void writeDOT(std::ostream& out, bool allArcs) const;
  
private:
  void init();
  
private:
  const CloneTreeVector& _ctv;
  const IntVector& _clustering;
  const int _k;
  /// Set of input tree indices for each cluster
  IntSetVector _cluster2trees;
  /// Consensus tree of each cluster
  std::vector<ParentChildGraph*> _cluster2consensus;
};

#endif // VISUALIZE_H
