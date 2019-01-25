/*
 * mctsolverca.cpp
 *
 *  Created on: 12-dec-2018
 *      Author: N. Aguse
 */

#include "mctsolverca.h"

MCTSolverCA::MCTSolverCA(const CloneTreeVector& ctv,
                         int k,
                         int timeLimit,
                         int nrMaxRestarts)
  : MCTSolver(ctv, k, timeLimit)
  , _nrMaxRestarts(nrMaxRestarts)
  , _nrRestarts(0)
{
  assert(k <= ctv.size());
  init();
}

void MCTSolverCA::writeSummaryHeader(std::ostream& out, bool newLine) const
{
  MCTSolver::writeSummaryHeader(out, false);
  out << "\t" << "restarts" << std::endl;
}

void MCTSolverCA::writeSummary(std::ostream& out, bool newLine) const
{
  MCTSolver::writeSummary(out, false);
  out << "\t" << _nrRestarts << std::endl;
}

void MCTSolverCA::init()
{
  
}

void MCTSolverCA::updateClustering()
{
  assert(_cluster2trees.size() != 0);
  
  resetClustering();
  for(int i = 0; i < _ctv.size(); i++)
  {
    int bestCluster = -1;
    int bestDist = std::numeric_limits<int>::max();
    
    for (int s = 0; s < _k; ++s)
    {
      int dist = _cluster2consensus[s]->parentChildDistance(_ctv[i]);
      if (dist < bestDist)
      {
        bestDist = dist;
        bestCluster = s;
      }
    }
    _cluster2trees[bestCluster].insert(i);
    _clustering.push_back(bestCluster);
  }
}

void MCTSolverCA::generateInitialClustering()
{
  assert(_k >= 1);
  
  std::uniform_int_distribution<> unif(0, _k - 1);
  
  while(true)
  {
    resetClustering();
    
    IntVector clustering;
    IntVector check(_k, 0);
    for (int i = 0; i < getNumTrees(); i++)
    {
      // assign each tree to a random cluster
      int cluster = unif(g_rng);
      clustering.push_back(cluster);
      check[cluster]++;
    }
    
    bool valid = true;
    for (int i: check)
    {
      if (i == 0)
      {
        valid = false;
      }
    }
    if (valid)
    {
      setClustering(clustering);
      return;
    }
  }
}

void MCTSolverCA::solve()
{
  
  generateInitialClustering();
  
  int minCost = std::numeric_limits<int>::max();
  IntVector bestClustering;
  
  _nrRestarts = 0;
  SecondsType timeLimit(_timeLimit);
  
  while (true)
  {
    generateInitialClustering();
    IntVector currClustering = _clustering;
    
    int currCost = -1;
    while (true)
    {
      generateParentChildGraphs();
      generateConsensusTrees();
      updateClustering();
      int newCost = getClusteringCost();
      const IntVector& newClustering = getClustering();
      if (currClustering == newClustering){
        currCost = newCost;
        break;
      }
      currCost = newCost;
      currClustering = newClustering;
    }
    
    if (currCost < minCost)
    {
      minCost = currCost;
      bestClustering = currClustering;
    }
    
    if (_nrMaxRestarts > 0 && _nrRestarts == _nrMaxRestarts)
    {
      break;
    }
    _nrRestarts++;
    
    if (_nrMaxRestarts <= 0)
    {
      auto finish = std::chrono::high_resolution_clock::now();
      if (finish - _startTimePoint >= timeLimit)
      {
        break;
      }
    }
  }
  
  setClustering(bestClustering);
  generateParentChildGraphs();
  generateConsensusTrees();
  updateClusteringCost();
}
