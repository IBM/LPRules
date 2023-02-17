// © Copyright IBM Corporation 2022. All Rights Reserved.
// LICENSE: Eclipse Public License - v 2.0, https://opensource.org/licenses/EPL-2.0
// SPDX-License-Identifier: EPL-2.0

#ifndef __MODEL2MASTERLP_HPP__
#define __MODEL2MASTERLP_HPP__

#include <fstream>
#include <iostream>
#include <sstream>

#include <cstdio>
#include <cstdlib>

#include <algorithm>

#include <vector>
#include <cstring>

#include "Data.hpp"
#include <ilcplex/ilocplex.h>

using namespace std;


class Model2MasterLP {
private:
  int relationId_;
  Data& dat_;

  int n_; // number of pairs in query
  vector<int> numNonZerosPerRow_; // for each pair, how many non-zeros in the matrix
  vector<int> numNonZerosPerCol_; // for each column, how many non-zeros in the matrix
  double minPercentCoverage_; // min percent of query pairs that need to be covered for the column to be added to the problem
  vector<int> numPairsExtraCoverage_; // number of entity pairs that are covered by columns but should not be
  double penaltyOnPairsExtraCoverage_;
  vector<double> xObjValues_;

  IloEnv env_;
  IloModel model_;
  IloCplex cpx_;
  IloNumVarArray eta_;
  IloNumVarArray x_;
  IloNumVarArray w_;
  IloRangeArray con6_;
  IloRange con7_;
  IloRangeArray con11_;
  IloObjective obj_;

public:
  Model2MasterLP(int relationId, Data& d);
  ~Model2MasterLP() {env_.end();}

  void createModelStructure();
  void setMaxComplexity(int maxComplexity);
  bool addCol(Rule& rule, double objPenalty);
  bool addCol(Rule& rule, vector<int>& column, double objPenalty);
  bool addCol(Rule& rule, vector<double>& column, double objPenalty);
  bool addColToLP(Rule& rule, vector<int>& column, double objPenalty);
  bool addColToLP(Rule& rule, vector<double>& column, double objPenalty);
  void solveModel(bool writeLpFile);
  void setInitParams();
  void getSolution(vector<double>& x, vector<double>& w);
  double getDuals(vector<double>& duals_con11);
  double getReducedCost(Rule& rule, vector<int>& column, 
			vector<double>& duals_con11);
  void printLPStatistics();
  void setMinPercentCoverage(double minCov);
  bool isThereEnoughCoverage(int rowsCovered);
  void addNumPairsExtraCoverage(int numPairsExtraCoverage);
  void setObjPenaltyOnNumPairsExtraCoverage(double penalty);
  void resetObjPenaltyOnNumPairsExtraCoverage();
};

#endif
