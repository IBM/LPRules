// © Copyright IBM Corporation 2022. All Rights Reserved.
// LICENSE: Eclipse Public License - v 2.0, https://opensource.org/licenses/EPL-2.0
// SPDX-License-Identifier: EPL-2.0

#include "Model2MasterLP.hpp"

//#include <iostream>
#include <iomanip>

using namespace std;

Model2MasterLP::Model2MasterLP(int relationId, Data& d)
  :relationId_(relationId), dat_(d)
{
  n_ = dat_.getNumPairsQuery(relationId_);
  numNonZerosPerRow_.resize(n_);
  minPercentCoverage_ = 0.0;
  penaltyOnPairsExtraCoverage_ = 0.0;
  model_ = IloModel(env_);
  createModelStructure();
  setInitParams();
}

void Model2MasterLP::createModelStructure()
{
  try {
    // initialize variables
    eta_ = IloNumVarArray(env_, n_, 0, 1, ILOFLOAT);
    eta_.setNames("_eta");
    x_ = IloNumVarArray(env_);
    w_ = IloNumVarArray(env_);

    // add objective function
    IloNumExpr exp(env_);
    for (IloInt i=0; i<n_; i++)
      exp += eta_[i];
    obj_ = IloObjective(env_, exp, IloObjective::Minimize, "obj");
    model_.add(obj_);
    exp.end();

    // add constraint 7
    con7_ = IloRange(env_, -IloInfinity, dat_.getMaxComplexity(), "Cardinality");
    model_.add(con7_);

    // add constraints 11
    con11_ = IloRangeArray(env_);
    for (IloInt i=0; i<n_; i++) {
      IloNumExpr exp(env_);
      exp += eta_[i];
      stringstream ss;
      ss << "con11." << con11_.getSize();
      string s = ss.str();
      con11_.add(IloRange(env_, 1, exp, IloInfinity, s.c_str()));
      exp.end();
    }
    model_.add(con11_);

    // initialize constraints 6
    con6_ = IloRangeArray(env_);

    cpx_ = IloCplex(model_);

  }
  catch (IloException& e) {
    cerr << "Concert exception caught: " << e << endl;
  }
  catch (...) {
    cerr << "Unknown exception caught" << endl;
  }

}

void Model2MasterLP::setMaxComplexity(int maxComplexity)
{
  con7_.setUB(maxComplexity);
}

bool Model2MasterLP::addCol(Rule& rule, double objPenalty)
{
  vector<int> numpaths; // each entry is either 0 or 1
  dat_.getNumPaths(relationId_, rule, numpaths);
  bool coladded = addCol(rule, numpaths, objPenalty);
  return coladded;
}

bool Model2MasterLP::addCol(Rule& rule, vector<int>& column, double objPenalty)
{
  assert(n_ == (int)column.size());
  int rows_covered = 0;
  for (int i=0; i<n_; i++) {
    if(column[i] > 0) {
      rows_covered += 1;
      numNonZerosPerRow_[i]++;
    }
  }

  if(!isThereEnoughCoverage(rows_covered)) {
    return false;
  }
  else {
    numNonZerosPerCol_.push_back(rows_covered);
  }

  bool coladded = addColToLP(rule, column, objPenalty);
  return coladded;
}

bool Model2MasterLP::addCol(Rule& rule, vector<double>& column, double objPenalty)
{
  assert(n_ == (int)column.size());
  int rows_covered = 0;
  for (int i=0; i<n_; i++) {
    if(column[i] > 0) {
      rows_covered += 1;
      numNonZerosPerRow_[i]++;
    }
  }

  if(!isThereEnoughCoverage(rows_covered)) {
    return false;
  }
  else {
    numNonZerosPerCol_.push_back(rows_covered);
  }

  bool coladded = addColToLP(rule, column, objPenalty);
  return coladded;
}

bool Model2MasterLP::addColToLP(Rule& rule, vector<int>& column, double objPenalty)
{
  //  IloNumColumn colx = obj_(0); // this is for the model without penalty
  double objx = objPenalty*(1+rule.getLengthRule());
  xObjValues_.push_back(objx);
  IloNumColumn colx = obj_(objx);
  IloNumColumn colw = obj_(0);

  for (int i=0; i<n_; i++) {
    if(column[i] > 0) {
      colw += con11_[i](column[i]);
    }
  }

  IloRange newcon;
  {
    stringstream ss;
    ss << "con6." << con6_.getSize();
    string s = ss.str();
    newcon = IloRange(env_, -IloInfinity, 0, s.c_str());
    con6_.add(newcon);
    model_.add(newcon);
    colw += newcon(1);
    colx += newcon(-1);
  }

  colx += con7_(1+rule.getLengthRule());

  {
    stringstream ss;
    ss << "x" << x_.getSize();
    string s = ss.str();
    x_.add(IloNumVar(colx, 0, 1, ILOFLOAT, s.c_str()));
    //    x_.add(IloNumVar(colx, 0, 1, ILOBOOL, s.c_str()));
  }
  {
    stringstream ss;
    ss << "w" << w_.getSize();
    string s = ss.str();
    w_.add(IloNumVar(colw, 0, 1, ILOFLOAT, s.c_str()));
  }

  //  newcon.end();
  colx.end();
  colw.end();

  return true;
}

bool Model2MasterLP::addColToLP(Rule& rule, vector<double>& column, double objPenalty)
{
  //  IloNumColumn colx = obj_(0); // this is for the model without penalty
  IloNumColumn colx = obj_(objPenalty*(1+rule.getLengthRule()));
  IloNumColumn colw = obj_(0);

  for (int i=0; i<n_; i++) {
    if(column[i] > 0) {
      colw += con11_[i](column[i]);
    }
  }

  IloRange newcon;
  {
    stringstream ss;
    ss << "con6." << con6_.getSize();
    string s = ss.str();
    newcon = IloRange(env_, -IloInfinity, 0, s.c_str());
    con6_.add(newcon);
    model_.add(newcon);
    colw += newcon(1);
    colx += newcon(-1);
  }

  colx += con7_(1+rule.getLengthRule());

  {
    stringstream ss;
    ss << "x" << x_.getSize();
    string s = ss.str();
    x_.add(IloNumVar(colx, 0, 1, ILOFLOAT, s.c_str()));
    //    x_.add(IloNumVar(colx, 0, 1, ILOBOOL, s.c_str()));
  }
  {
    stringstream ss;
    ss << "w" << w_.getSize();
    string s = ss.str();
    w_.add(IloNumVar(colw, 0, 1, ILOFLOAT, s.c_str()));
  }

  //  newcon.end();
  colx.end();
  colw.end();

  return true;
}

void Model2MasterLP::solveModel(bool writeLpFile)
{
  try {
    if(writeLpFile)
      cpx_.exportModel("model.lp");

    // Optimize the problem and obtain solution.
    if ( !cpx_.solve() ) {
      env_.error() << "Failed to optimize LP" << endl;
      throw(-1);
    }

    IloNumArray vals(env_);
    env_.out() << "Solution status = " << cpx_.getStatus() << endl;
    env_.out() << "Solution value  = " << cpx_.getObjValue() << endl;
#if 0
    cpx_.getValues(vals, x_);
    env_.out() << "Values x       = " << vals << endl;
    cpx_.getValues(vals, w_);
    env_.out() << "Values w       = " << vals << endl;


    env_.out() << "Slacks        = " << vals << endl;
    cpx_.getDuals(vals, con);
    env_.out() << "Duals         = " << vals << endl;
    cpx_.getReducedCosts(vals, x_);
    env_.out() << "Reduced Costs x = " << vals << endl;
    cpx_.getReducedCosts(vals, w_);
    env_.out() << "Reduced Costs w = " << vals << endl;
#endif
  }
  catch (IloException& e) {
    cerr << "Concert exception caught: " << e << endl;
  }
  catch (...) {
    cerr << "Unknown exception caught" << endl;
  }
  
}

void Model2MasterLP::setInitParams()
{
  try {
    int numberThreads = 1;
    cpx_.setParam(IloCplex::Param::Threads, numberThreads);
    env_.out() << "Maximum number of threads in CPLEX: " << numberThreads << endl;
    //    int timelimit = 30;
    //    cpx_.setParam(IloCplex::Param::TimeLimit, timelimit);
    //    env_.out() << "Time Limit in secs: " << timelimit << endl;
    int lpAlgorithm = 0; // 0 (automatic), 1 (primal) 2 (Dual)
    cpx_.setParam(IloCplex::Param::RootAlgorithm, lpAlgorithm);
    env_.out() << "LP algorithm: " << lpAlgorithm << endl;

  }
  catch (IloException& e) {
    cerr << "Concert exception caught: " << e << endl;
  }
  catch (...) {
    cerr << "Unknown exception caught" << endl;
  }
  
}

void Model2MasterLP::getSolution(vector<double>& x, vector<double>& w)
{
  try {
    IloNumArray vals(env_);

    assert(x.size() == 0);
    cpx_.getValues(vals, x_);
    for (IloInt i=0; i<vals.getSize(); i++)
      x.push_back(vals[i]);

    assert(w.size() == 0);
    cpx_.getValues(vals, w_);
    for (IloInt i=0; i<vals.getSize(); i++)
      w.push_back(vals[i]);

  }
  catch (IloException& e) {
    cerr << "Concert exception caught: " << e << endl;
  }
  catch (...) {
    cerr << "Unknown exception caught" << endl;
  }

}

double Model2MasterLP::getDuals(vector<double>& duals_con11)
{
  double dual_con7;

  try {
    assert(duals_con11.size() == n_);
    for (IloInt i=0; i<n_; i++)
      duals_con11[i] = cpx_.getDual(con11_[i]);

    double dual_con7 = cpx_.getDual(con7_);
    return dual_con7;
  }
  catch (IloException& e) {
    cerr << "Concert exception caught: " << e << endl;
  }
  catch (...) {
    cerr << "Unknown exception caught" << endl;
  }

}

double Model2MasterLP::getReducedCost(Rule& rule, vector<int>& column, vector<double>& duals_con11)
{
  if(column.size()==0) {
    dat_.getNumPaths(relationId_, rule, column);
  }

  double rc = 0.0;
  for (int i=0; i<n_; i++)
    rc -= duals_con11[i] * column[i];
  //  cout<<"reduced cost: "<<rc<<endl;
  return rc;
}

void Model2MasterLP::printLPStatistics()
{
  int npairscovered = 0;
  for(int i=0; i<(int)numNonZerosPerRow_.size(); i++) {
    if(numNonZerosPerRow_[i] > 0)
      npairscovered++;
  }

  cout<<"number of pairs in query covered by at least one column: "<<npairscovered<<endl;

  cout<<"number nonzeros per column: ";
  for(int i=0; i<(int)numNonZerosPerCol_.size(); i++) {
    //    cout<<"("<<i<<", "<<numNonZerosPerCol_[i]<<") ";
    cout<<"("<<i<<", "<<numNonZerosPerCol_[i]<<", "<<numPairsExtraCoverage_[i]<<") ";
  }
  cout<<endl;

}

void Model2MasterLP::setMinPercentCoverage(double minCov)
{
  minPercentCoverage_=minCov;
  //  cout<<"Minimum percent coverage to add column: "<<minPercentCoverage_<<endl;
}

bool Model2MasterLP::isThereEnoughCoverage(int rowsCovered)
{
  if(rowsCovered == 0)
    return false;

  if(rowsCovered < minPercentCoverage_ * n_)
    return false;

  return true;
}

void Model2MasterLP::addNumPairsExtraCoverage(int numPairsExtraCoverage)
{
  numPairsExtraCoverage_.push_back(numPairsExtraCoverage);
}

void Model2MasterLP::setObjPenaltyOnNumPairsExtraCoverage(double penalty)
{
  assert(x_.getSize() == xObjValues_.size());
  assert(x_.getSize() == numPairsExtraCoverage_.size());
  for (IloInt i = 0; i < x_.getSize(); i++) {
    double coef = xObjValues_[i];
    coef += numPairsExtraCoverage_[i]*(penalty-penaltyOnPairsExtraCoverage_);
    obj_.setLinearCoef(x_[i], coef);
    xObjValues_[i] = coef;
  }
  penaltyOnPairsExtraCoverage_ = penalty;
}

void Model2MasterLP::resetObjPenaltyOnNumPairsExtraCoverage()
{
  assert(x_.getSize() == xObjValues_.size());
  assert(x_.getSize() == numPairsExtraCoverage_.size());
  for (IloInt i = 0; i < x_.getSize(); i++) {
    double coef = xObjValues_[i];
    coef -= numPairsExtraCoverage_[i]*penaltyOnPairsExtraCoverage_;
    obj_.setLinearCoef(x_[i], coef);
    xObjValues_[i] = coef;
  }
  penaltyOnPairsExtraCoverage_ = 0.0;
}
