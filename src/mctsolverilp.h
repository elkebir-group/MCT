/*
 * mctsolverilp.h
 *
 *  Created on: 19-jan-2019
 *      Author: M. El-Kebir
 */

#ifndef MCTSOLVERILP_H
#define MCTSOLVERILP_H

#include <ilcplex/ilocplex.h>
#include "mctsolver.h"

class MCTSolverILP : public MCTSolver
{
public:
  /// Constructor
  ///
  /// @param ctv Input clone trees
  /// @param k Number of clusters
  /// @param timeLimit Time limit in seconds
  /// @param memoryLimit Memory limit in MB
  /// @param nrThreads Number of threads
  /// @param verbose Indicates whether verbose output should be generated
  MCTSolverILP(const CloneTreeVector& ctv,
               int k,
               int timeLimit,
               int memoryLimit,
               int nrThreads,
               bool verbose);
  
  virtual void solve();
  
  virtual std::string getMethodName() const
  {
    return "ILP";
  }
  
private:
  void init();
  void initVariables();
  void initConstraints();
  void initObjective();
  
  typedef IloArray<IloNumVarArray> IloNumVarMatrix;
  typedef IloArray<IloNumVarMatrix> IloNumVar3Matrix;
  typedef IloArray<IloNumVar3Matrix> IloNumVar4Matrix;
  typedef IloArray<IloBoolVarArray> IloBoolVarMatrix;
  typedef IloArray<IloBoolVarMatrix> IloBoolVar3Matrix;
  typedef IloArray<IloBoolVar3Matrix> IloBoolVar4Matrix;
  typedef std::vector<BoolMatrix> BoolTensor;
  
private:
  /// Time limit in seconds
  int _timeLimit;
  /// Memory limit in MB
  int _memoryLimit;
  /// Number of threads
  int _nrThreads;
  /// Verbose output
  bool _verbose;
  /// Environment
  IloEnv _env;
  /// CPlex model
  IloModel _model;
  /// Solver
  IloCplex _cplex;
  /// Index to mutation
  StringVector _indexToMutation;
  /// Mutation to index
  StringToIntMap _mutationToIndex;
  /// a[i][p][q] = 1 iff tree i contains directed edge (p,q)
  BoolTensor _a;
  /// b[p][q] = 1 iff there exists a tree i with directed edge (p,q)
  BoolMatrix _b;
  /// x[i][s] = 1 iff tree i assigned to cluster s
  IloBoolVarMatrix _x;
  /// y[s][p][q] = 1 iff mutation p is the parent of mutation q in cluster s
  IloNumVar3Matrix _y;
  /// z[s][p] = 1 iff mutation p is the root of cluster s
  IloNumVarMatrix _z;
  /// w[i][s][p][q] = x[i][s] * |a[i][p][q] - y[s][p][q])|
  IloNumVar4Matrix _w;
};

#endif // MCTSOLVERBF_H
