// © Copyright IBM Corporation 2022. All Rights Reserved.
// LICENSE: Eclipse Public License - v 2.0, https://opensource.org/licenses/EPL-2.0
// SPDX-License-Identifier: EPL-2.0

#ifndef __SOLVER_HPP__
#define __SOLVER_HPP__

#include <fstream>
#include <iostream>
#include <sstream>

#include <cstdio>
#include <cstdlib>

#include <algorithm>

#include <vector>
#include <cstring>

#include "Data.hpp"
#include "Model2MasterLP.hpp"
#include <ilcplex/ilocplex.h>

using namespace std;

class Rankings {
private:
  vector<vector<int> > rankingsRightRaw_;
  vector<vector<int> > rankingsLeftRaw_;
  vector<vector<int> > rankingsRightFiltered_;
  vector<vector<int> > rankingsLeftFiltered_;

public:
  Rankings(int numTypes);
  ~Rankings() {}

  void addRankingsRightRaw(int rankingType, int rank)
  {rankingsRightRaw_[rankingType].push_back(rank);}
  void addRankingsLeftRaw(int rankingType, int rank)
  {rankingsLeftRaw_[rankingType].push_back(rank);}
  void addRankingsRightFiltered(int rankingType, int rank)
  {rankingsRightFiltered_[rankingType].push_back(rank);}
  void addRankingsLeftFiltered(int rankingType, int rank)
  {rankingsLeftFiltered_[rankingType].push_back(rank);}
  vector<int>& getRankingsRightRaw(int rankingType) 
  {return rankingsRightRaw_[rankingType];}
  vector<int>& getRankingsLeftRaw(int rankingType) 
  {return rankingsLeftRaw_[rankingType];}
  vector<int>& getRankingsRightFiltered(int rankingType) 
  {return rankingsRightFiltered_[rankingType];}
  vector<int>& getRankingsLeftFiltered(int rankingType) 
  {return rankingsLeftFiltered_[rankingType];}

};

class Solver {
private:
  Parameters& params_;
  Data data_;
  vector<int> maxComplexity_;
  //  int maxComplexity_;
  double minPercentCoverage_; // min percent of query pairs that need to be covered for the column to be added to the problem

  vector<vector<Rule> > rules_;
  vector<vector<int> > rulesadded_;
  vector<vector<double> > rulesselected_;
  vector<vector<double> > rulesweights_;

  vector<Rankings> rankings_;

  vector<vector<int> > rankingsAggressiveRightRaw_;
  vector<vector<int> > rankingsAggressiveRightFiltered_;
  vector<vector<int> > rankingsAggressiveLeftRaw_;
  vector<vector<int> > rankingsAggressiveLeftFiltered_;

  vector<vector<int> > rankingsMidPointRightRaw_;
  vector<vector<int> > rankingsMidPointRightFiltered_;
  vector<vector<int> > rankingsMidPointLeftRaw_;
  vector<vector<int> > rankingsMidPointLeftFiltered_;

  vector<vector<int> > rankingsRandomBreakRightRaw_;
  vector<vector<int> > rankingsRandomBreakRightFiltered_;
  vector<vector<int> > rankingsRandomBreakLeftRaw_;
  vector<vector<int> > rankingsRandomBreakLeftFiltered_;

  vector<vector<int> > rankingsRightRaw_;
  vector<vector<int> > rankingsRightFiltered_;
  vector<vector<int> > rankingsLeftRaw_;
  vector<vector<int> > rankingsLeftFiltered_;

public:
  Solver(Parameters& params):
    params_(params), 
    data_(params.getMaxComplexity())
    //    maxComplexity_(params.getMaxComplexity())
  {setMinPercentCoverage(0.0);}
  ~Solver() {}

  enum Side {left, right, both};
  void setMaxComplexity(int relationId, int maxComplexity) {maxComplexity_[relationId]=maxComplexity;}
  void setMinPercentCoverage(double minCov);
  void run(string scoresFileName, string rulesFileName, string inputRulesFileName);
  void runOneRelation(int relationId);
  void setBestSettingsModel2(int relationId, Model2MasterLP& mlp);
  void runColumnGenerationOneRelation(int relationId);
  void printSolution(int relationId, bool printAll=false);
  int getNumPairsExtraCoverage(int modifiedRelationId, Rule& rule,
			       vector<int>& column);
  void getColumnForRule(int modifiedRelationId, Rule& rule,
			vector<double>& column);
  double getScore(int relationId, Rule& rule, int cpairId);
  double getScore(int relationId, pair<int,int>& cpair);
  void getEntitiesOfInterest(int relationId, int whichCombination, map<int,set<int> >& rEntities, map<int,set<int> >& lEntities);
  void getEntitiesOfInterestForHead(int relationId, int tail, vector<bool>& useEntity, bool useAllData=true);
  void getEntitiesOfInterestForTail(int relationId, int head, vector<bool>& useEntity, bool useAllData=true);
  bool shouldUpdateRanking(double basescore, double score, int rankingType);
  void getRightScores(Arc* outArcWithRelation, Rule& rule, int entityId, vector<double>& scores, bool useBFS);
  void getLeftScores(Arc* outArcWithRelation, Rule& rule, int entityId, vector<double>& scores, bool useBFS);
  void getRightScores(int relationId, int entityId, vector<double>& scores, bool useBFS);
  void getLeftScores(int relationId, int entityId, vector<double>& scores, bool useBFS);
  void getRightScores(int relationId, vector<set<int> >& destIds, vector<double>& weights, map<int,double>& scores);
  void getLeftScores(int relationId, vector<set<int> >& origIds, vector<double>& weights, map<int,double>& scores);
  void getRightEntities(int relationId, int entityId, map<int,vector<set<int> > >& rDestIds, map<int,vector<double> >& rWeights, bool useBFS);
  void getLeftEntities(int relationId, int entityId, map<int,vector<set<int> > >& lOrigIds, map<int,vector<double> >& lWeights, bool useBFS);
  int getMidPointRank(int rankAggressive, int numSameScore);
  void writeScoresToFile(int relationId, string fname);
  void findBestComplexityAndPenalty(int modifiedRelationId,
				    Model2MasterLP& mlp,
				    int& bestComplexity,
				    double& bestPenalty);
  int findBestComplexity(int modifiedRelationId,
			 Model2MasterLP& mlp);
  double computeMRR(vector<int>& rankings);
  void computeStatistics(string fname, string type, vector<vector<int> >& rankingsAggressive, vector<vector<int> >& rankingsMidPoint, vector<vector<int> >& rankingsRandomBreak, vector<vector<int> >& rankings, bool isFiltered);
  void computeStatisticsForRelations(string fname, string type, vector<vector<int> >& rankingsAggressive, vector<vector<int> >& rankingsMidPoint, vector<vector<int> >& rankingsRandomBreak, vector<vector<int> >& rankings, bool isFiltered);
  void generateRules(int relationId, vector<Rule>& rules);
  void generateRulesS0(int relationId, vector<Rule>& rules);
  void generateRulesS1(int relationId, vector<Rule>& rules);
  void generateRulesS2(int relationId, vector<Rule>& rules);
  void generateRulesS3(int relationId, vector<Rule>& rules);
  void generateRulesS0Duals(int relationId, vector<Rule>& rules, vector<double>& duals, int maxRuleLength);
  void calculateFirstLast(int relationId, vector<int>& firstrel, vector<int>& firstinvrel, vector<int>& lastrel, vector<int>& lastinvrel);
  int find_sp(int snode, int enode, int relid, vector<int> & distance, vector<int> & prev, vector<int> &prevrel, set<int> & tnodes, Rule & r, vector<int> &firstrel, vector<int> &firstinvrel);
  int find_sp(int snode, int enode, int relid, Arc* arc, vector<int> & distance, vector<int> & prev, vector<int> &prevrel, set<int> & tnodes, Rule & r, vector<int> &firstrel, vector<int> &firstinvrel);
  int find_sp_1(int snode, int enode, int relid, Arc* ar, vector<int> & distance, vector<int> & prev, vector<int> &prevrel, set<int> & tnodes, Rule & r, vector<int> &firstrel, vector<int> &firstinvrel, int maxRuleLength);
  int find_sp2(int snode, int enode, int truedist, int relid, Arc* ar, vector<int> & distance, vector<int> & prev, vector<int> &prevrel, set<int> & tnodes, Rule & r, vector<int> &firstrel, vector<int> &firstinvrel);

  void generateRulesHeuristic(int relationId, vector<Rule>& rules);
  void generateRulesHeuristic(int relationId,
			      vector<double>& duals,
			      vector<Rule>& rules);
  void getHighFrequencyRelations(int relationId, int numOutRelations, vector<int>& nodeIds, vector<double>& duals, vector<int>& outrelations, vector<int>& outinvrelations);
  void getEndNodesRule(Rule& rule, vector<int>& nodeIds, vector<int>& endNodeStarts, vector<int>& lengths, vector<int>& endNodeIds);
  void writeRulesToFile(int relationId, string fname);
  void readRulesFromFile(string fname);
  void readAnyBURLRulesFromFile(string fname);
  
};

#endif
