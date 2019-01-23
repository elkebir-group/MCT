/*
 * mctsolverilp.cpp
 *
 *  Created on: 19-jan-2019
 *      Author: M. El-Kebir
 */

#include "mctsolverilp.h"
#include "mctsolverilpcallbacklazy.h"
#include "mctsolverilpcallbackuser.h"

MCTSolverILP::MCTSolverILP(const CloneTreeVector& ctv,
                           int k,
                           int timeLimit,
                           int memoryLimit,
                           int nrThreads,
                           bool verbose)
  : MCTSolver(ctv, k)
  , _timeLimit(timeLimit)
  , _memoryLimit(memoryLimit)
  , _nrThreads(nrThreads)
  , _verbose(verbose)
  , _env()
  , _model(_env)
  , _cplex(_model)
  , _indexToMutation()
  , _mutationToIndex()
  , _a()
  , _b()
  , _x()
  , _y()
  , _z()
  , _w()
{
  init();
}

void MCTSolverILP::init()
{
  assert(!_ctv.empty());

  _indexToMutation.clear();
  _mutationToIndex.clear();
  for (NodeIt v(_ctv[0].tree()); v != lemon::INVALID; ++v)
  {
    const std::string& lbl = _ctv[0].label(v);
    assert(_mutationToIndex.count(lbl) == 0);
    _mutationToIndex[lbl] = _indexToMutation.size();
    _indexToMutation.push_back(lbl);
  }
  
  const int n = _ctv.size();
  const int m = lemon::countNodes(_ctv[0].tree());

  _a = BoolTensor(n, BoolMatrix(m, BoolVector(m, false)));
  for (int i = 0; i < n; ++i)
  {
    const CloneTree& T_i = _ctv[i];
    for (ArcIt uv(T_i.tree()); uv != lemon::INVALID; ++uv)
    {
      Node u = T_i.tree().source(uv);
      Node v = T_i.tree().target(uv);
      
      const std::string& lbl_u = T_i.label(u);
      const std::string& lbl_v = T_i.label(v);
      
      assert(_mutationToIndex.count(lbl_u));
      assert(_mutationToIndex.count(lbl_v));
      
      int idx_u = _mutationToIndex[lbl_u];
      int idx_v = _mutationToIndex[lbl_v];
      
      _a[i][idx_u][idx_v] = true;
    }
  }
  
  _b = BoolMatrix(m, BoolVector(m, false));
  for (int p = 0; p < m; ++p)
  {
    for (int q = 0; q < m; ++q)
    {
      for (int i = 0; i < n; ++i)
      {
        if (_a[i][p][q])
        {
          _b[p][q] = true;
        }
      }
    }
  }
  
  initVariables();
  initConstraints();
  initObjective();
}

void MCTSolverILP::initVariables()
{
  assert(!_ctv.empty());
  char buf[1024];
  
  const int n = _ctv.size();
  const int m = lemon::countNodes(_ctv[0].tree());
  
  // x[i][s] = 1 iff tree i assigned to cluster s
  _x = IloBoolVarMatrix(_env, n);
  for (int i = 0; i < n; i++)
  {
    _x[i] = IloBoolVarArray(_env, _k);
    for (int s = 0; s < _k; s++)
    {
      snprintf(buf, 1024, "x_%d_%d", i, s);
      _x[i][s] = IloBoolVar(_env, buf);
    }
  }
  
  // y[s][p][q] = 1 iff mutation p is the parent of mutation q in cluster s
//  _y = IloBoolVar3Matrix(_env, _k);
//  for (int s = 0; s < _k; s++)
//  {
//    _y[s] = IloBoolVarMatrix(_env, m);
//    for (int p = 0; p < m; p++)
//    {
//      _y[s][p] = IloBoolVarArray(_env, m);
//      for (int q = 0; q < m; q++)
//      {
//        snprintf(buf, 1024, "y_%d_%d_%d", s, p, q);
//        _y[s][p][q] = IloBoolVar(_env, buf);
//      }
//    }
//  }
  
  _y = IloNumVar3Matrix(_env, _k);
  for (int s = 0; s < _k; s++)
  {
    _y[s] = IloNumVarMatrix(_env, m);
    for (int p = 0; p < m; p++)
    {
      _y[s][p] = IloNumVarArray(_env, m);
      for (int q = 0; q < m; q++)
      {
        snprintf(buf, 1024, "y_%d_%d_%d", s, p, q);
        _y[s][p][q] = IloNumVar(_env, 0., 1., buf);
      }
    }
  }
  
  // z[s][p] = 1 iff mutation p is the root of cluster s
//  _z = IloBoolVarMatrix(_env, _k);
  _z = IloNumVarMatrix(_env, _k);
  for (int s = 0; s < _k; s++)
  {
//    _z[s] = IloBoolVarArray(_env, m);
    _z[s] = IloNumVarArray(_env, m);
    for (int p = 0; p < m; p++)
    {
      snprintf(buf, 1024, "z_%d_%d", s, p);
      _z[s][p] = IloNumVar(_env, 0., 1., buf);
//      _z[s][p] = IloBoolVar(_env, buf);
    }
  }
  
  // w[i][s][p][q] = x[i][s] * |a[i][p][q] - y[s][p][q])|
//  _w = IloBoolVar4Matrix(_env, n);
//  for (int i = 0; i < n; ++i)
//  {
//    _w[i] = IloBoolVar3Matrix(_env, _k);
//    for (int s = 0; s < _k; ++s)
//    {
//      _w[i][s] = IloBoolVarMatrix(_env, m);
//      for (int p = 0; p < m; ++p)
//      {
//        _w[i][s][p] = IloBoolVarArray(_env, m);
//        for (int q = 0; q < m; ++q)
//        {
//          snprintf(buf, 1024, "w_%d_%d_%d_%d", i, s, p, q);
//          _w[i][s][p][q] = IloBoolVar(_env, buf);
//        }
//      }
//    }
//  }
  
  _w = IloNumVar4Matrix(_env, n);
  for (int i = 0; i < n; ++i)
  {
    _w[i] = IloNumVar3Matrix(_env, _k);
    for (int s = 0; s < _k; ++s)
    {
      _w[i][s] = IloNumVarMatrix(_env, m);
      for (int p = 0; p < m; ++p)
      {
        _w[i][s][p] = IloNumVarArray(_env, m);
        for (int q = 0; q < m; ++q)
        {
          snprintf(buf, 1024, "w_%d_%d_%d_%d", i, s, p, q);
          _w[i][s][p][q] = IloNumVar(_env, 0., 1.0, buf);
        }
      }
    }
  }
}

void MCTSolverILP::initConstraints()
{
  assert(!_ctv.empty());
  
  const int n = _ctv.size();
  const int m = lemon::countNodes(_ctv[0].tree());
  
  IloExpr sum(_env);
  
  // each tree is assigned to exactly one cluster
  for (int i = 0; i < n; ++i)
  {
    for (int s = 0; s < _k; ++s)
    {
      sum += _x[i][s];
    }
    
    _model.add(sum == 1);
    sum.clear();
  }
  
  // no cluster can be empty
  for (int s = 0; s < _k; ++s)
  {
    for (int i = 0; i < n; ++i)
    {
      sum += _x[i][s];
    }
    
    _model.add(sum >= 1);
    sum.clear();
  }
  
  // only one node can be the root
  for (int s = 0; s < _k; ++s)
  {
    for (int p = 0; p < m; ++p)
    {
      sum += _z[s][p];
    }
    
    _model.add(sum == 1);
    sum.clear();
  }
  
  // y[s] must encode a tree: unique parentage
  for (int s = 0; s < _k; ++s)
  {
    for (int q = 0; q < m; ++q)
    {
      for (int p = 0; p < m; ++p)
      {
        sum += _y[s][p][q];
      }

      _model.add(sum == 1 - _z[s][q]);
      sum.clear();
    }
  }
  
  for (int s = 0; s < _k; ++s)
  {
    for (int q = 0; q < m; ++q)
    {
      for (int p = 0; p < m; ++p)
      {
        sum += _y[s][p][q];
      }
    }
    _model.add(sum == (m - 1));
    sum.clear();
  }
  
  // force root node:
  IloExpr sum2(_env);
//  for (int s = 0; s < _k; ++s)
//  {
//    for (int i = 0; i < n; ++i)
//    {
//      sum += _x[i][s];
//    }
//
//    for (int p = 0; p < m; ++p)
//    {
//      for (int q = 0; q < m; ++q)
//      {
//        for (int i = 0; i < n; ++i)
//        {
//          sum2 += _x[i][s] * (1 - _a[i][q][p]);
//        }
//      }
//      _model.add(_z[s][p] >= sum2 - sum*(m-1) + 1);
//      sum2.clear();
//    }
//    sum.clear();
//    _model.add(_z[s][8] == 1);
//  }

  
  // objective
  for (int i = 0; i < n; ++i)
  {
    for (int s = 0; s < _k; ++s)
    {
      for (int p = 0; p < m; ++p)
      {
        for (int q = 0; q < m; ++q)
        {
//          _model.add(_w[i][s][p][q] <= (_a[i][p][q] ? 1 - _y[s][p][q] : _y[s][p][q]));
          _model.add(_w[i][s][p][q] <= _a[i][p][q]);
          _model.add(_w[i][s][p][q] <= _y[s][p][q]);
          _model.add(_w[i][s][p][q] <= _x[i][s]);
//          _model.add(_w[i][s][p][q] >= _x[i][s] + _y[s][p][q] - 1);
//          _model.add(_w[i][s][p][q] >= (_a[i][p][q] ? 1 - _y[s][p][q] : _y[s][p][q])
//                    + _x[i][s] - 1);
        }
      }
    }
  }
//
//  for (int i = 0; i < n; ++i)
//  {
//    for (int s = 0; s < _k; ++s)
//    {
//      for (int p = 0; p < m; ++p)
//      {
//        for (int q = 0; q < m; ++q)
//        {
//          sum += _w[i][s][p][q];
//        }
//      }
//      _model.add(sum <= _x[i][s] * (m-1));
//      sum.clear();
//    }
//  }

  for (int s = 0; s < _k; ++s)
  {
    for (int i = 0; i < n; ++i)
    {
      sum += _x[i][s] * (m - 1);
    }
    
    for (int i = 0; i < n; ++i)
    {
      for (int p = 0; p < m; ++p)
      {
        for (int q = 0; q < m; ++q)
        {
          sum2 += _w[i][s][p][q];
        }
      }
      _model.add(sum2 <= sum);
      sum2.clear();
    }
    sum.clear();
  }
  
  // disable edges not in input trees
  for (int p = 0; p < m; ++p)
  {
    for (int q = 0; q < m; ++q)
    {
      for (int s = 0; s < _k; ++s)
      {
        sum += _y[s][p][q];
      }
      
      if (!_b[p][q])
      {
        _model.add(sum == 0);
      }
      sum.clear();
    }
  }
  
  // disable edges not in input trees: part 2
  for (int s = 0; s < _k; ++s)
  {
    for (int p = 0; p < m; ++p)
    {
      for (int q = 0; q < m; ++q)
      {
        for (int i = 0; i < n; ++i)
        {
          if (_a[i][p][q])
          {
            sum += _x[i][s];
          }
        }
        _model.add(_y[s][p][q] <= sum);
        sum.clear();
      }
    }
  }
  
  // if q has the same parent p in all trees assigned to cluster s
  // then y[s][p][q] must be 1
  for (int s = 0; s < _k; ++s)
  {
    for (int i = 0; i < n; ++i)
    {
      sum += _x[i][s];
    }
    
    for (int p = 0; p < m; ++p)
    {
      for (int q = 0; q < m; ++q)
      {
        for (int i = 0; i < n; ++i)
        {
          sum2 += _x[i][s] * _a[i][p][q];
        }
        _model.add(_y[s][p][q] >= sum2 - sum + 1);
        sum2.clear();
      }
    }
    sum.clear();
  }
  
//  for (int s = 0; s < _k; ++s)
//  {
//    for (int p = 0; p < m; ++p)
//    {
//      for (int q = 0; q < m; ++q)
//      {
//        for (int i = 0; i < n; ++i)
//        {
//          sum += _w[i][s][p][q];
//        }
//      }
//    }
//
//    if (s > 0)
//    {
//      _model.add(sum >= sum2);
//    }
//    sum2 = sum;
//    sum.clear();
//  }
  
  // symmetry breaking
  for (int s = 0; s < _k; ++s)
  {
    for (int i = 0; i < n; ++i)
    {
      sum += _x[i][s];
    }
    
    if (s > 0)
    {
      _model.add(sum >= sum2);
    }
    sum2 = sum;
    sum.clear();
  }
  
  sum2.end();
  if (_k > 1)
  {
    sum.end();
  }
}

void MCTSolverILP::initObjective()
{
  const int n = _ctv.size();
  const int m = lemon::countNodes(_ctv[0].tree());
  
  IloExpr sum(_env);
  
//  for (int i = 0; i < n; ++i)
//  {
//    for (int s = 0; s < _k; ++s)
//    {
//      for (int p = 0; p < m; ++p)
//      {
//        for (int q = 0; q < m; ++q)
//        {
//          sum += _w[i][s][p][q];
//        }
//      }
//    }
//  }
  
  for (int s = 0; s < _k; ++s)
  {
    // compute n_s: number of trees in cluster s
//    for (int i = 0; i < n; ++i)
//    {
//      sum += _x[i][s] * (m - 1);
//    }

    for (int i = 0; i < n; ++i)
    {
      for (int p = 0; p < m; ++p)
      {
        for (int q = 0; q < m; ++q)
        {
          sum -= _w[i][s][p][q];
        }
      }
    }
  }
  
  _model.add(IloMinimize(_env, 2*n*(m-1) + 2*sum));
  
  sum.end();
}

void MCTSolverILP::solve()
{
  if (_nrThreads > 0)
  {
    _cplex.setParam(IloCplex::Threads, _nrThreads);
  }
  if (_timeLimit > 0)
  {
    _cplex.setParam(IloCplex::TiLim, _timeLimit);
  }
  if (_memoryLimit > 0)
  {
    _cplex.setParam(IloCplex::WorkMem, _memoryLimit);
  }
  if (!_verbose)
  {
    _cplex.setOut(_env.getNullStream());
    _cplex.setError(_env.getNullStream());
    _cplex.setWarning(_env.getNullStream());
  }
  
  const int n = _ctv.size();
//  const int m = lemon::countNodes(_ctv[0].tree());
//  std::cerr << _y[0][1][3].getName() << std::endl;
//  std::cerr << _y[0][3][1].getName() << std::endl;
  
//  _model.add(_y[0][1][3] == 1);
//  _model.add(_y[0][3][1] == 1);
//  _cplex.exportModel("/tmp/test.lp");
//  _cplex.use(IloCplex::Callback(new (_env) MctSolverIlpCallbackLazy(_env, _k, _b,
//                                                                    _indexToMutation, _y, _z)));
  _cplex.use(IloCplex::Callback(new (_env) MctSolverIlpCallbackUser(_env, _k, _b,
                                                                    _indexToMutation, _y, _z)));

//  try
  {
    if (!_cplex.solve())
      return;
  }
//  catch (IloException& e)
//  {
//    std::cerr << e.getMessage() << std::endl;
//    return;
//  }
  
  std::cerr << "Obj: " << _cplex.getObjValue() << std::endl;
//  for (int i = 0; i < n; ++i)
//  {
//    for (int s = 0; s < _k; ++s)
//    {
//      for (int p = 0; p < m; ++p)
//      {
//        for (int q = 0; q < m; ++q)
//        {
//          std::cerr << _w[i][s][p][q].getName() << " = " << _cplex.getValue(_w[i][s][p][q]) << std::endl;
//        }
//      }
//    }
//    std::cerr << std::endl;
//  }
//  std::cerr << std::endl;
  
  // 1. identify clustering
  _clustering = IntVector(n, -1);
  for (int i = 0; i < n; ++i)
  {
    for (int s = 0; s < _k; ++s)
    {
      if (_cplex.getValue(_x[i][s]) >= 0.4)
      {
        _clustering[i] = s;
      }
    }
    assert(_clustering[i] != -1);
  }

  // update _cluster2trees
  _cluster2trees = IntSetVector(_k);
  for (int i = 0; i < n; ++i)
  {
    _cluster2trees[_clustering[i]].insert(i);
  }
  
  // update _cluster2totaltrees
  for (int s = 0; s < _k; ++s)
  {
    _cluster2totaltrees[s] = _cluster2trees.size();
  }
  
  generateParentChildGraphs();
  
  // 2. identify consensus trees and costs
  for (int s = 0; s < _k; ++s)
  {
    int mstCost = 0;
    
    _cluster2consensus[s]->clearMST();
    for (ArcIt a(_cluster2consensus[s]->getGraph()); a != lemon::INVALID; ++a)
    {
      Node u = _cluster2consensus[s]->getGraph().source(a);
      Node v = _cluster2consensus[s]->getGraph().target(a);
      
      const std::string& lbl_u = _cluster2consensus[s]->getLabel(u);
      const std::string& lbl_v = _cluster2consensus[s]->getLabel(v);
      
      assert(_mutationToIndex.count(lbl_u));
      assert(_mutationToIndex.count(lbl_v));
      
      int p = _mutationToIndex[lbl_u];
      int q = _mutationToIndex[lbl_v];
      
      if (_cplex.getValue(_y[s][p][q]) >= 0.4)
      {
        _cluster2consensus[s]->setConsensusTreeArc(a, true);
        mstCost += _cluster2consensus[s]->getArcCost(a);
      }
    }
    
    _cluster2consensus[s]->setMstCost(mstCost);
  }
  
  // 3. compute cost
//  for (int s = 0; s < _k; ++s)
//  {
//    int cost = 0;
//    for (int i = 0; i < n; ++i)
//    {
//      for (int p = 0; p < m; ++p)
//      {
//        for (int q = 0; q < m; ++q)
//        {
//          if (_cplex.getValue(_w[i][s][p][q]) >= 0.4)
//          {
//            ++cost;
//          }
//        }
//      }
//    }
//    _cluster2cost[s] = cost;
//    std::cerr << "Cluster " << s << " -- cost: " << cost << std::endl;
//  }
}
