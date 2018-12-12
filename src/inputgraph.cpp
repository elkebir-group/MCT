/*
 * inputgraph.cpp
 *
 *  Created on: 6-dec-2018
 *      Author: N. Aguse
 */
#include "inputgraph.h"

InputGraph::InputGraph(const CloneTreeVector& ctv)
  : _ctv(ctv)
  , _G()
  , _labelToNode()
  , _nodeToLabel(_G)
  , _arcCost(_G)
  , _mst(_G){
  init();
}

void InputGraph::init(){
//  std::cout << _ctv.size() << std::endl;

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
      
      DynArcLookUp ae(_G);
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

void InputGraph::writeDOT(std::ostream &out) const{
  out << "digraph T {" << std::endl;
  out << "\t{" << std::endl;
  
  for (NodeIt u(_G); u != lemon::INVALID; ++u)
  {
    out << "\t" << _G.id(u) << " [label=\"" << _nodeToLabel[u] << "\"]" << std::endl;
    
  }
  
  for (ArcIt a(_G); a != lemon::INVALID; ++a)
  {
    out << "\t" << _G.id(_G.source(a)) << " -> " << _G.id(_G.target(a)) << std::endl;
  }
  
  out << "}" << std::endl;
}


int InputGraph::parentChildDistance(const CloneTree & T){
  StringPairList GEdges = getSelectedEdgeList();
  GEdges.sort();
  StringPairList treeEdges = T.getEdgeList();
  treeEdges.sort();
  int symm_diff = 0;
  for (auto e1:GEdges){
    bool exist = false;
    for (auto e2:treeEdges){
      if (e1 == e2)
        exist = true;
    }
    if (!exist){
      symm_diff ++;
    }
  }
  for (auto e1:treeEdges){
    bool exist = false;
    for (auto e2:GEdges){
      if (e1 == e2)
        exist = true;
    }
    if (!exist){
      symm_diff ++;
    }
  }
  return symm_diff;
}

int InputGraph::clusteringCost(const CloneTreeVector & cluster){
  int total = 0;
  for (const CloneTree& T : cluster){
    total += parentChildDistance(T);
  }
  
  return total;
}

void InputGraph::SL_graphyc(){
  Digraph::ArcMap<bool> mst(_G, false);
  int bestCost = 100000;
  
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
