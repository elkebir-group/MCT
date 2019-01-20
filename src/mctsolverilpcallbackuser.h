/*
 * mctsolverilpcallbackuser.h
 *
 *  Created on: 20-jan-2019
 *      Author: M. El-Kebir
 */

#ifndef MCTSOLVERILPCALLBACKUSER_H
#define MCTSOLVERILPCALLBACKUSER_H

#include <ilcplex/ilocplex.h>
#include <lemon/hao_orlin.h>
#include "utils.h"

class MctSolverIlpCallbackUser : public IloCplex::UserCutCallbackI
{
public:
  typedef IloArray<IloBoolVarArray> IloBoolVarMatrix;
  typedef IloArray<IloBoolVarMatrix> IloBoolVar3Matrix;
  typedef IloArray<IloBoolVar3Matrix> IloBoolVar4Matrix;
  
  MctSolverIlpCallbackUser(IloEnv env,
                           int k,
                           const BoolMatrix& b,
                           const StringVector& indexToMutation,
                           IloBoolVar3Matrix y,
                           IloBoolVarMatrix z);
  
  MctSolverIlpCallbackUser(const MctSolverIlpCallbackUser& other);
  
  IloCplex::CallbackI *duplicateCallback() const
  {
    return (new (getEnv()) MctSolverIlpCallbackUser(*this));
  }
  
  void main();
  
private:
  void init();
  
  void writeDOT(std::ostream& out) const;

  typedef std::vector<Arc> ArcVector;
  
  int getIndex(int s, int p, int q) const
  {
    const int m = _b.size();
    
    assert(0 <= s && s < _k);
    assert(0 <= p && p < m);
    assert(0 <= q && q < m);
    
    return s * m * m + p * m + q;
  }
  
  int getIndex(int s, int p) const
  {
    const int m = _b.size();
    
    assert(0 <= s && s < _k);
    assert(0 <= p && p < m);
    
    return _k*m*m + s * m + p;
  }
  
  /// Environment
  IloEnv _env;
  /// Number of clusters
  const int _k;
  /// b[p][q] = 1 iff there exists a tree i with directed edge (p,q)
  const BoolMatrix& _b;
  /// Index to mutation
  const StringVector& _indexToMutation;
  
  IloBoolVarArray _variables;
  
  IloNumArray _values;
  
  /// y[s][p][q] = 1 iff mutation p is the parent of mutation q in cluster s
  IloBoolVar3Matrix _y;
  /// z[s][p] = 1 iff mutation p is the root of cluster s
  IloBoolVarMatrix _z;
  
  Digraph _G;
  
  Node _root;
  
  NodeVector _mutationToNode;
  
  IntNodeMap _nodeToMutation;
  
  ArcVector _rootArcs;
  
  DoubleArcMap _arcCost;
  
  lemon::HaoOrlin<Digraph, DoubleArcMap> _minCutAlg;
  
  BoolNodeMap _cutMap;
};

#endif // MCTSOLVERILPCALLBACKUSER_H
