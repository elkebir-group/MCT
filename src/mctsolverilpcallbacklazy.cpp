/*
 * mctsolverilpcallbacklazy.cpp
 *
 *  Created on: 21-jan-2019
 *      Author: M. El-Kebir
 */

#include "mctsolverilpcallbacklazy.h"

MctSolverIlpCallbackLazy::MctSolverIlpCallbackLazy(IloEnv env,
                                                   int k,
                                                   const BoolMatrix& b,
                                                   const StringVector& indexToMutation,
                                                   IloNumVar3Matrix y,
                                                   IloNumVarMatrix z)
  : MctSolverIlpCallback(k, b, indexToMutation, y, z)
  , IloCplex::LazyConstraintCallbackI(env)
{
  init(getEnv());
}

MctSolverIlpCallbackLazy::MctSolverIlpCallbackLazy(const MctSolverIlpCallbackLazy& other)
  : MctSolverIlpCallback(other)
  , IloCplex::LazyConstraintCallbackI(other)
{
  init(getEnv());
}

void MctSolverIlpCallbackLazy::updateVarsVals()
{
  getValues(_values, _variables);
}

void MctSolverIlpCallbackLazy::addConstraint(IloExpr expr)
{
  add(1 <= expr).end();
}
