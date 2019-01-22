/*
 * mctsolverilpcallback.h
 *
 *  Created on: 20-jan-2019
 *      Author: M. El-Kebir
 */

#ifndef MCTSOLVERILPCALLBACK_H
#define MCTSOLVERILPCALLBACK_H

#include <ilcplex/ilocplex.h>
#include <lemon/hao_orlin.h>
#include "utils.h"

class MctSolverIlpCallback
{
public:
  typedef IloArray<IloNumVarArray> IloNumVarMatrix;
  typedef IloArray<IloNumVarMatrix> IloNumVar3Matrix;
  typedef IloArray<IloNumVar3Matrix> IloNumVar4Matrix;
  typedef IloArray<IloBoolVarArray> IloBoolVarMatrix;
  typedef IloArray<IloBoolVarMatrix> IloBoolVar3Matrix;
  typedef IloArray<IloBoolVar3Matrix> IloBoolVar4Matrix;
  
  MctSolverIlpCallback(int k,
                       const BoolMatrix& b,
                       const StringVector& indexToMutation,
                       IloNumVar3Matrix y,
                       IloNumVarMatrix z);
  
  MctSolverIlpCallback(const MctSolverIlpCallback& other);
  
protected:
  void separate(IloEnv env);
  void init(IloEnv env);
  
  virtual void updateVarsVals() = 0;
  virtual void addConstraint(IloExpr expr) = 0;
  
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
  
  /// Number of clusters
  const int _k;
  /// b[p][q] = 1 iff there exists a tree i with directed edge (p,q)
  const BoolMatrix& _b;
  /// Index to mutation
  const StringVector& _indexToMutation;
  
  /// y[s][p][q] = 1 iff mutation p is the parent of mutation q in cluster s
  IloNumVar3Matrix _y;
  /// z[s][p] = 1 iff mutation p is the root of cluster s
  IloNumVarMatrix _z;

  
  IloNumVarArray _variables;
  
  IloNumArray _values;
  
  Digraph _G;
  
  Node _root;
  
  NodeVector _mutationToNode;
  
  IntNodeMap _nodeToMutation;
  
  ArcVector _rootArcs;
  
  DoubleArcMap _arcCost;
  
  lemon::HaoOrlin<Digraph, DoubleArcMap> _minCutAlg;
  
  BoolNodeMap _cutMap;
  
  int _nodeNumber;
  
  int _cutCount;
};

#endif // MCTSOLVERILPCALLBACK_H
