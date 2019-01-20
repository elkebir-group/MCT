/*
 * mctsolverilpcallback.cpp
 *
 *  Created on: 20-jan-2019
 *      Author: M. El-Kebir
 */

#include "mctsolverilpcallback.h"

MctSolverIlpCallback::MctSolverIlpCallback(IloEnv env,
                                           int k,
                                           const BoolMatrix& b,
                                           const StringVector& indexToMutation,
                                           IloBoolVar3Matrix y,
                                           IloBoolVarMatrix z)
  : IloCplex::LazyConstraintCallbackI(env)
  , _env(env)
  , _k(k)
  , _b(b)
  , _indexToMutation(indexToMutation)
  , _variables()
  , _values()
  , _z(z)
  , _G()
  , _root(lemon::INVALID)
  , _mutationToNode()
  , _nodeToMutation(_G)
  , _rootArcs()
  , _arcCost(_G)
  , _minCutAlg(_G, _arcCost)
  , _cutMap(_G)
{
  const int m = _b.size();
  
  _variables = IloBoolVarArray(_env, _k*m*m + _k*m);
  for (int s = 0; s < _k; ++s)
  {
    for (int p = 0; p < m; ++p)
    {
      for (int q = 0; q < m; ++q)
      {
        int idx = getIndex(s, p, q);
        _variables[idx] = y[s][p][q];
      }
    }
  }
  
  for (int s = 0; s < _k; ++s)
  {
    for (int p = 0; p < m; ++p)
    {
      int idx = getIndex(s, p);
      _variables[idx] = z[s][p];
    }
  }
  
  _values = IloNumArray(_env, _k*m*m + _k*m);
  
  init();
}

MctSolverIlpCallback::MctSolverIlpCallback(const MctSolverIlpCallback& other)
  : IloCplex::LazyConstraintCallbackI(other)
  , _env(other._env)
  , _k(other._k)
  , _b(other._b)
  , _indexToMutation(other._indexToMutation)
  , _variables(other._variables)
  , _values(other._values)
  , _G()
  , _root(lemon::INVALID)
  , _mutationToNode()
  , _nodeToMutation(_G)
  , _rootArcs()
  , _arcCost(_G)
  , _minCutAlg(_G, _arcCost)
  , _cutMap(_G)
{
  init();
}

void MctSolverIlpCallback::init()
{
  const int m = _b.size();
  
  _root = _G.addNode();
  _nodeToMutation[_root] = -1;
  for (int p = 0; p < m; ++p)
  {
    _mutationToNode.push_back(_G.addNode());
    _nodeToMutation[_mutationToNode.back()] = p;
    _rootArcs.push_back(_G.addArc(_root, _mutationToNode.back()));
  }
  
  for (int p = 0; p < m; ++p)
  {
    for (int q = 0; q < m; ++q)
    {
      if (_b[p][q])
      {
        _G.addArc(_mutationToNode[p], _mutationToNode[q]);
      }
    }
  }
}

void MctSolverIlpCallback::writeDOT(std::ostream& out) const
{
  out << "digraph G {" << std::endl;
  
  for (NodeIt v(_G); v != lemon::INVALID; ++v)
  {
    out << "\t" << _nodeToMutation[v] << " [label=\"";
    if (v == _root)
    {
      out << "root\"";
    }
    else
    {
      out << _indexToMutation[_nodeToMutation[v]] << "\"";
    }
    
    if (_cutMap[v])
    {
      out << ",color=red";
    }
    out << "]" << std::endl;
  }
  
  for (ArcIt a(_G); a != lemon::INVALID; ++a)
  {
    Node u = _G.source(a);
    Node v = _G.target(a);
    
    out << "\t" << _nodeToMutation[u] << " -> " << _nodeToMutation[v] << " [label=\"";
    out << _arcCost[a] << "\"";
    if (_cutMap[u] != _cutMap[v])
    {
      out << ",color=red";
    }
    out << "]" << std::endl;
  }
  
  out << "}" << std::endl;
}

void MctSolverIlpCallback::main()
{
  getValues(_values, _variables);
  
  IloExpr sum(_env);
  
  for (int s = 0; s < _k; ++s)
  {
    for (ArcIt a(_G); a != lemon::INVALID; ++a)
    {
      Node v_p = _G.source(a);
      Node v_q = _G.target(a);
      
      int p = _nodeToMutation[v_p];
      int q = _nodeToMutation[v_q];
      
      if (v_p == _root)
      {
        _arcCost[a] = _values[getIndex(s, q)];
      }
      else
      {
        _arcCost[a] = _values[getIndex(s, p, q)];
      }
    }
    
    // separate
    _minCutAlg.init(_root);
    _minCutAlg.calculateOut();
    _minCutAlg.minCutMap(_cutMap);
    
//    std::cerr << _minCutAlg.minCutValue() << std::endl;
//    std::ofstream out("/tmp/GG.dot");
//    writeDOT(out);
//    out.close();
    
    if (g_tol.less(_minCutAlg.minCutValue(), 1.))
    {
      
      for (ArcIt a(_G); a != lemon::INVALID; ++a)
      {
        Node u = _G.source(a);
        Node v = _G.target(a);
        
        int p = _nodeToMutation[u];
        int q = _nodeToMutation[v];
        
        if (_cutMap[u] != _cutMap[v])
        {
          if (u == _root)
          {
            sum += _variables[getIndex(s, q)];
          }
          else
          {
            sum += _variables[getIndex(s, p, q)];
          }
        }
      }
      add(1 <= sum).end();
    }
  }
  
  sum.end();
}
