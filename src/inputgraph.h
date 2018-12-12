/*
 * inputgraph.h
 *
 *  Created on: 6-dec-2018
 *      Author: N. Aguse
 */

#ifndef INPUTGRAPH_H
#define INPUTGRAPH_H
#include "clonetree.h"
#include <lemon/min_cost_arborescence.h>
#include <algorithm>

class InputGraph{

public:
  InputGraph(const CloneTreeVector& ctv);

  void writeDOT(std::ostream & out) const;
  void SL_graphyc();
  int parentChildDistance(const CloneTree & T);
  int clusteringCost(const CloneTreeVector & cluster);
  
  /// Return the selecteed edge set in _mst
  StringPairList getSelectedEdgeList() const{
    StringPairList res;
      for (ArcIt a(_G); a != lemon::INVALID; ++a) {
        if (_mst[a]){
          Node u = _G.source(a);
          Node v = _G.target(a);
          res.push_back(StringPair(_nodeToLabel[u], _nodeToLabel[v]));
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

#endif // INPUTGRAPH_H
