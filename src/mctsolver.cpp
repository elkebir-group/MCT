/*
 * mctsolver.cpp
 *
 *  Created on: 12-dec-2018
 *      Author: N. Aguse
 */

#include "mctsolver.h"

MCTSolver::MCTSolver(const CloneTreeVector& ctv, int k, int timeLimit)
  : _ctv(ctv)
  , _k(k)
  , _startTimePoint(std::chrono::high_resolution_clock::now())
  , _clusteringCost(-1)
  , _cluster2trees(k)
  , _cluster2consensus(k)
  , _cluster2cost(k)
  , _cluster2totaltrees(k)
  , _clustering()
  , _id()
  , _timeLimit(timeLimit)
{
}

MCTSolver::~MCTSolver()
{
  clearConsensusTrees();
}

void MCTSolver::run(MCTSolver& solver,
                    const std::string& outputPrefix)
{
  if (solver._k > solver._ctv.size())
  {
    std::cerr << "Error: more clusters than input trees" << std::endl;
    return;
  }
  
  solver.solve();
  
  {
    std::ofstream outFile;
    outFile.open(outputPrefix + "_clustering.txt");
    solver.writeClustering(outFile);
  }
  
  {
    std::ofstream outFile;
    outFile.open(outputPrefix + "_summary.tsv");
    solver.writeSummaryHeader(outFile, true);
    solver.writeSummary(outFile, true);
  }
    
//  solver.displayConsensusTrees();
  
  char buf[1024];
  for (int j = 0; j < solver._k; ++j)
  {
    snprintf(buf, 1024, "%s_cluster%d.dot", outputPrefix.c_str(), j);
    std::ofstream outFile(buf);
    solver._cluster2consensus[j]->writeDOT(outFile,
                                           solver._cluster2totaltrees[j],
                                           solver._cluster2cost[j]);
    outFile.close();
  }
}

void MCTSolver::displayConsensusTrees() const
{
  for (int i = 0; i < _k; ++i)
  {
    auto edges = _cluster2consensus[i]->getSelectedEdgeList();
    for (const auto& edge : edges)
    {
      std::cerr << edge.first << " " << edge.second << std::endl;
    }
    std::cerr << std::endl << std::endl;
  }
}

void MCTSolver::resetClustering()
{
  _clustering.clear();
  _cluster2trees = IntSetVector(_k);
}

void MCTSolver::clearConsensusTrees()
{
  for (auto it = _cluster2consensus.begin();
       it != _cluster2consensus.end();
       it++)
  {
    delete *it;
    *it = NULL;
  }
}

void MCTSolver::generateParentChildGraphs()
{
  clearConsensusTrees();
  assert(_cluster2trees.size() == _k);
  
  int clusterIdx = 0;
  for (const IntSet& trees : _cluster2trees)
  {
    std::vector<CloneTree> ctvcopy;
    
    for (int treeIdx : trees)
    {
      ctvcopy.push_back(_ctv[treeIdx]);
    }
    _cluster2consensus[clusterIdx] = new ParentChildGraph(ctvcopy);
    
    ++clusterIdx;
  }
}

void MCTSolver::generateConsensusTrees()
{
  assert(_cluster2trees.size() == _k);
  
  for (int s = 0; s < _k; ++s)
  {
    _cluster2consensus[s]->SL_graphyc();
  }
}

void MCTSolver::updateClusteringCost()
{
  _clusteringCost = 0;
  
  assert(_cluster2trees.size() != 0);
  assert(_cluster2consensus.size() != 0);
  
  for (auto it = _cluster2trees.begin(); it != _cluster2trees.end(); it++ ){
    std::vector<CloneTree> ctvcopy;
    for (int idx: *it){
      ctvcopy.push_back(_ctv[idx]);
    }
    int idx = it - _cluster2trees.begin();
    _cluster2cost[idx] = _cluster2consensus[idx]->clusteringCost(ctvcopy);
    _cluster2totaltrees[idx] = ctvcopy.size();
    _clusteringCost += _cluster2cost[idx];
  }
}

void MCTSolver::setClustering(const std::vector<int>& clustering)
{
  assert(clustering.size() == _ctv.size());
  _clustering = clustering;
  
  // update _cluster2trees
  const int n = _ctv.size();
  _cluster2trees = IntSetVector(_k);
  for (int i = 0; i < n; ++i)
  {
    _cluster2trees[clustering[i]].insert(i);
  }
  
  generateParentChildGraphs();
  generateConsensusTrees();
  updateClusteringCost();
}

void MCTSolver::writeClustering(std::ostream& out) const
{
  for (const auto &e : _clustering) out << e << "\n";
}

void MCTSolver::writeSummaryHeader(std::ostream& out, bool newLine) const
{
  out << "method\ttime\tk\tcost";
  for (int s = 0; s < _k; ++s)
  {
    out << "\tcost-" << s;
  }
  out << "\ttrees";
  for (int s = 0; s < _k; ++s)
  {
    out << "\ttrees-" << s;
  }
  
  if (newLine)
  {
    out << std::endl;
  }
}

void MCTSolver::writeSummary(std::ostream& out, bool newLine) const
{
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> diff = finish - _startTimePoint;
  double secondsElapsed = diff.count();
  
  out << getMethodName() << "\t" << secondsElapsed << "\t" << _k << "\t " << getClusteringCost();
  
  for (int s = 0; s < _k; ++s)
  {
    out << "\t" << _cluster2cost[s];
  }
  
  out << "\t" << _ctv.size();
  for (int s = 0; s < _k; ++s)
  {
    out << "\t" << _cluster2totaltrees[s];
  }
  
  if (newLine)
  {
    out << std::endl;
  }
}
