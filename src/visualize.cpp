/*
 * visualize.cpp
 *
 *  Created on: 17-jan-2019
 *      Author: M. El-Kebir
 */

#include "visualize.h"

Visualize::Visualize(const CloneTreeVector& ctv,
                     const IntVector& clustering,
                     int k)
  : _ctv(ctv)
  , _clustering(clustering)
  , _k(k)
  , _cluster2trees()
  , _cluster2consensus()
{
  init();
}

void Visualize::init()
{
  // update _cluster2trees
  const int n = _ctv.size();
  _cluster2trees = IntSetVector(_k);
  for (int i = 0; i < n; ++i)
  {
    _cluster2trees[_clustering[i]].insert(i);
  }
  
  _cluster2consensus = std::vector<ParentChildGraph*>(_k, NULL);
  int clusterIdx = 0;
  for (const IntSet& trees : _cluster2trees){
    std::vector<CloneTree> ctvcopy;
    
    for (int treeIdx : trees){
      ctvcopy.push_back(_ctv[treeIdx]);
    }
    _cluster2consensus[clusterIdx] = new ParentChildGraph(ctvcopy);
    _cluster2consensus[clusterIdx]->SL_graphyc();
    
    ++clusterIdx;
  }
}

Visualize::~Visualize(){
  for (auto it = _cluster2consensus.begin();
       it != _cluster2consensus.end(); it++)
  {
    delete *it;
    *it = NULL;
  }
}

void Visualize::writeDOT(std::ostream& out, bool allArcs) const
{
  StringToIntMap mapToIdx;
  
  out << "digraph G {" << std::endl;
  
  int maxCount = 0;
  for (int clusterIdx = 0; clusterIdx < _k; ++clusterIdx)
  {
    maxCount = std::max(maxCount,
                        _cluster2consensus[clusterIdx]->getMaxCount(allArcs));
  }
  
  if (!_ctv.empty())
  {
    // output nodes
    for (NodeIt v(_ctv[0].tree()); v != lemon::INVALID; ++v)
    {
      const std::string& lbl_v = _ctv[0].label(v);
      if (mapToIdx.count(lbl_v) == 0)
      {
        int idx = mapToIdx.size();
        mapToIdx[lbl_v] = idx;
      }
      out << "\t" << mapToIdx[lbl_v] << " [label=\"" << lbl_v << "\"]" << std::endl;
    }
    
    for (int clusterIdx = 0; clusterIdx < _k; ++clusterIdx)
    {
      ParentChildGraph::StringPairIntSet arcs =
        _cluster2consensus[clusterIdx]->getSelectedEdgeListWithCounts(allArcs);
      for (const auto arc : arcs)
      {
        if (arc.second > 0)
        {
          out << "\t" << mapToIdx[arc.first.first] << " -> "
              << mapToIdx[arc.first.second] << " [penwidth=" << 1. + arc.second / float(maxCount) * 4
              << ", colorscheme=\"set28\", color="
              << clusterIdx + 1
              << ", label=\""
              << "\"]" << std::endl;
        }
        else
        {
          out << "\t" << mapToIdx[arc.first.first] << " -> "
              << mapToIdx[arc.first.second] << " [penwidth=" << 1. - arc.second / float(maxCount) * 4
              << ", colorscheme=\"set28\", style=dashed, color="
              << clusterIdx + 1
              << ", label=\""
              << "\"]" << std::endl;
        }
      }
    }
  }
  
  out << "}" << std::endl;
}
