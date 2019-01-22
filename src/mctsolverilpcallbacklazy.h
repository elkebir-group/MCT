/*
 * mctsolverilpcallbacklazy.h
 *
 *  Created on: 21-jan-2019
 *      Author: M. El-Kebir
 */

#ifndef MCTSOLVERILPCALLBACKLAZY_H
#define MCTSOLVERILPCALLBACKLAZY_H

#include "mctsolverilpcallback.h"

class MctSolverIlpCallbackLazy : public MctSolverIlpCallback, public IloCplex::LazyConstraintCallbackI
{
public:
  MctSolverIlpCallbackLazy(IloEnv env,
                           int k,
                           const BoolMatrix& b,
                           const StringVector& indexToMutation,
                           IloNumVar3Matrix y,
                           IloNumVarMatrix z);
  
  MctSolverIlpCallbackLazy(const MctSolverIlpCallbackLazy& other);
  
  void main()
  {
//    std::cerr << "lazy" << std::endl;
    if (_nodeNumber != getNnodes())
    {
      _nodeNumber = getNnodes();
      _cutCount = 0;
//      _makeAttempt = _backOff.makeAttempt();
    }
    if (_cutCount < 10)
    {
      separate(getEnv());
      ++_cutCount;
    }
  }
  
  IloCplex::CallbackI *duplicateCallback() const
  {
    return (new (getEnv()) MctSolverIlpCallbackLazy(*this));
  }
  
protected:
  virtual void updateVarsVals();
  virtual void addConstraint(IloExpr expr);
};

#endif // MCTSOLVERILPCALLBACKLAZY_H
