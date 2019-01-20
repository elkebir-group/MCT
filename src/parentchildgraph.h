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
#include <lemon/maps.h>
#include <algorithm>

class ParentChildGraph
{
public:
  ParentChildGraph(const CloneTreeVector& ctv);
  
  void clearMST()
  {
    lemon::mapFill(_G, _mst, false);
  }
  
  const Digraph& getGraph() const
  {
    return _G;
  }
  
  bool isInConsensusTree(Arc a) const
  {
    return _mst[a];
  }
  
  void setConsensusTreeArc(Arc a, bool status)
  {
    _mst[a] = status;
  }
  
  const std::string& getLabel(Node u) const
  {
    return _nodeToLabel[u];
  }

  void writeDOT(std::ostream & out) const;
  
  void writeDOT(std::ostream & out,
                int numTrees,
                int cost) const;
  
  void SL_graphyc();
  
  int parentChildDistance(const CloneTree & T);
  
  int clusteringCost(const CloneTreeVector & cluster);
  
  int getMaxCount(bool allArcs) const
  {
    int res = 0;
    
    for (ArcIt a(_G); a != lemon::INVALID; ++a)
    {
      if (allArcs || _mst[a])
      {
        res = std::min(res, _arcCost[a]);
      }
    }

    return -res;
  }
  
  /// Return the selected edge set in _mst
  StringPairSet getSelectedEdgeList() const
  {
    StringPairSet res;
    for (ArcIt a(_G); a != lemon::INVALID; ++a)
    {
      if (_mst[a])
      {
        Node u = _G.source(a);
        Node v = _G.target(a);
        res.insert(StringPair(_nodeToLabel[u], _nodeToLabel[v]));
      }
    }
    return res;
  }
  
  typedef std::set<std::pair<StringPair, int> > StringPairIntSet;
  
  /// Return the selected edge set in _mst
  StringPairIntSet getSelectedEdgeListWithCounts(bool allArcs) const
  {
    StringPairIntSet res;
    for (ArcIt a(_G); a != lemon::INVALID; ++a)
    {
      Node u = _G.source(a);
      Node v = _G.target(a);
      
      if (_mst[a])
      {
        res.insert(std::make_pair(StringPair(_nodeToLabel[u], _nodeToLabel[v]),
                                  -1 * _arcCost[a]));
      }
      else if (allArcs)
      {
        res.insert(std::make_pair(StringPair(_nodeToLabel[u], _nodeToLabel[v]),
                                  _arcCost[a]));
      }
    }
    return res;
  }
  
  int getCost() const
  {
    return _mstCost;
  }
  
  int getArcCost(Arc a) const
  {
    return _arcCost[a];
  }
  
  int getMstCost() const
  {
    return _mstCost;
  }
  
  void setMstCost(int mstCost)
  {
    _mstCost = mstCost;
  }

private:
  const CloneTreeVector& _ctv;
  Digraph _G;
  StringToNodeMap _labelToNode;
  StringNodeMap _nodeToLabel;
  IntArcMap _arcCost;
  Digraph::ArcMap<bool> _mst;
  int _mstCost;
  
  void init();
};

#endif // PARENTCHILDGRAPH_H
