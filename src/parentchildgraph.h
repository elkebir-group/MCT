/*
 * parentchildgraph.h
 *
 *  Created on: 6-dec-2018
 *      Author: N. Aguse
 */

#ifndef PARENTCHILDGRAPH_H
#define PARENTCHILDGRAPH_H

#include "clonetree.h"
#include <lemon/min_cost_arborescence.h>
#include <algorithm>

class ParentChildGraph{

public:
  ParentChildGraph(const CloneTreeVector& ctv);

  void writeDOT(std::ostream & out) const;
  void writeDOT(std::ostream & out,
                int numTrees,
                int cost) const;
  void SL_graphyc();
  int parentChildDistance(const CloneTree & T);
  int clusteringCost(const CloneTreeVector & cluster);
  
  /// Return the selected edge set in _mst
  StringPairSet getSelectedEdgeList() const{
    StringPairSet res;
      for (ArcIt a(_G); a != lemon::INVALID; ++a) {
        if (_mst[a]){
          Node u = _G.source(a);
          Node v = _G.target(a);
          res.insert(StringPair(_nodeToLabel[u], _nodeToLabel[v]));
        }
      }
    return res;
  }

private:

  const CloneTreeVector& _ctv;
  Digraph _G;
  StringToNodeMap _labelToNode;
  StringNodeMap _nodeToLabel;
  IntArcMap _arcCost;
  Digraph::ArcMap<bool> _mst;
  
  void init();
  
};

#endif // PARENTCHILDGRAPH_H
