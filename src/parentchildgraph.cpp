/*
 * inputgraph.cpp
 *
 *  Created on: 6-dec-2018
 *      Author: N. Aguse
 */
#include "parentchildgraph.h"

ParentChildGraph::ParentChildGraph(const CloneTreeVector& ctv)
  : _ctv(ctv)
  , _G()
  , _labelToNode()
  , _nodeToLabel(_G)
  , _arcCost(_G)
  , _mst(_G){
  init();
}

void ParentChildGraph::init(){
//  std::cout << _ctv.size() << std::endl;

  DynArcLookUp ae(_G);
  for (const CloneTree& T : _ctv)
  {
    for(NodeIt v(T.tree()); v != lemon::INVALID; ++v){
      
      if (_labelToNode.count(T.label(v)) == 0){
        Node n = _G.addNode();
        _nodeToLabel[n] = T.label(v);
        _labelToNode[T.label(v)] = n;
      }
    }
    for(ArcIt a(T.tree()); a != lemon::INVALID; ++a){
      
      Node src = T.tree().source(a);
      Node tgt = T.tree().target(a);
      const std::string& label_src = T.label(src);
      const std::string& label_tgt = T.label(tgt);
      
      assert(_labelToNode.count(label_src) == 1);
      assert(_labelToNode.count(label_tgt) == 1);
      
      Arc a_exist = ae(_labelToNode[label_src],_labelToNode[label_tgt]);
      if (a_exist != lemon::INVALID){
        _arcCost[a_exist]--;
      }
      else{
        Arc newarc = _G.addArc(_labelToNode[label_src], _labelToNode[label_tgt]);
        _arcCost[newarc] = -1;
      }
    }
  }
  
}

void ParentChildGraph::writeDOT(std::ostream &out, int numTrees, int cost) const{
  out << "digraph T {" << std::endl;
  out << "\tlabel=\"Number of trees: " << numTrees << "\\nCost: " << cost << "\"" << std::endl;
  for (NodeIt u(_G); u != lemon::INVALID; ++u)
  {
    out << "\t" << _G.id(u) << " [label=\"" << _nodeToLabel[u] << "\"]" << std::endl;
  }
  
  for (ArcIt a(_G); a != lemon::INVALID; ++a)
  {
    out << "\t" << _G.id(_G.source(a)) << " -> " << _G.id(_G.target(a));
    out << " [label=\"" << _arcCost[a] << "\"";
    if (_mst[a])
    {
      out << ",penwidth=3,color=red";
    }
    out << "]" << std::endl;
    
  }
  
  out << "}" << std::endl;
}

void ParentChildGraph::writeDOT(std::ostream &out) const{
  out << "digraph T {" << std::endl;
  
  for (NodeIt u(_G); u != lemon::INVALID; ++u)
  {
    out << "\t" << _G.id(u) << " [label=\"" << _nodeToLabel[u] << "\"]" << std::endl;
  }
  
  for (ArcIt a(_G); a != lemon::INVALID; ++a)
  {
    out << "\t" << _G.id(_G.source(a)) << " -> " << _G.id(_G.target(a));
    out << " [label=\"" << _arcCost[a] << "\"";
    if (_mst[a])
    {
      out << ",penwidth=3,color=red";
    }
    out << "]" << std::endl;
    
  }
  
  out << "}" << std::endl;
}

int ParentChildGraph::parentChildDistance(const CloneTree & T){
  StringPairSet GEdges = getSelectedEdgeList();
  StringPairSet treeEdges = T.getEdgeSet();
  
  StringPairSet resSet;
  std::set_symmetric_difference(GEdges.begin(), GEdges.end(),
                                treeEdges.begin(), treeEdges.end(),
                                std::inserter(resSet, resSet.begin()));
  
  return resSet.size();
}

int ParentChildGraph::clusteringCost(const CloneTreeVector & cluster){
  int total = 0;
  for (const CloneTree& T : cluster){
    total += parentChildDistance(T);
  }
  
  return total;
}

void ParentChildGraph::SL_graphyc(){
  Digraph::ArcMap<bool> mst(_G, false);
  int bestCost = INT_MAX;
  
  for (NodeIt u(_G); u != lemon::INVALID; ++u){
    
    Digraph::NodeMap<bool> selectedNodes(_G, false);
    int arborescence_cost = lemon::minCostArborescence(_G, _arcCost, u, mst);
    for (ArcIt a(_G); a != lemon::INVALID; ++a){
      
      const Node& source = _G.source(a);
      const Node& target = _G.target(a);
      
      if (mst[a]){
        selectedNodes[source] = true;
        selectedNodes[target] = true;
      }
      
    }
    
    bool chosen = true;
    for (NodeIt n(_G); n != lemon::INVALID; ++n){
      
      if (selectedNodes[n] == false){
        chosen = false;
        break;
      }
    
    }
    if (arborescence_cost < bestCost && chosen){
      bestCost = arborescence_cost;
      
      for (ArcIt a(_G); a != lemon::INVALID; ++a){
        _mst[a] = mst[a];
      }
      
    }
    
  }
}
