// © Copyright IBM Corporation 2022. All Rights Reserved.
// LICENSE: Eclipse Public License - v 2.0, https://opensource.org/licenses/EPL-2.0
// SPDX-License-Identifier: EPL-2.0

#ifndef __PARAMETERS_HPP__
#define __PARAMETERS_HPP__

#include <fstream>
#include <iostream>
#include <sstream>

#include <cstdio>
#include <cstdlib>

#include <algorithm>

#include <vector>
#include <map>
#include <iterator>
#include <cstring>

using namespace std;

class Parameters {
private:
  string directory_;
  int maxComplexity_;
  int modelNumber_; // model number can be 1 or 2
  int relationId_;
  bool useRelationInRules_;
  bool useRulesOfLengthOne_;
  bool useRelationInLengthOneRule_;
  int maxRuleLength_;
  bool reportStatsRightRemoval_;
  bool reportStatsLeftRemoval_;
  bool reportStatsAllRemoval_;
  double penaltyOnComplexity_;
  bool printScoresToFile_;
  bool writeLpFile_;
  int rankingType_; // 0 is aggresive, 1 is intermediate, 2 is conservative, 3 is randomBreak
  bool useReverseArcsInRules_;
  bool runOnlyForRelationsInTest_;
  bool runForReverseRelations_;
  bool runColumnGeneration_;
  int generateRules_; // 0 is enumeration, 1 is S0, 2 is heuristic
  bool runFindBestComplexity_;
  int findBestComplexityRankingType_; // 0 is aggresive, 1 is intermediate, 2 is conservative, 3 is randomBreak
  double alphaConvexCombinationModel3_;
  //  double penaltyOnNegativePairs_;
  vector<double> penaltyOnNegativePairs_;
  bool addPenaltyOnNegativePairs_;
  bool useBreadthFirstSearch_;
  bool repeatedNodesAllowed_;
  int runMode_; // 0 is normal, 1 is read rules + score, 2 is read rules + run LP + score, 3 is read rules + add new rules + run LP + score
  int maxItersColumnGeneration_;
  bool speedUpComputationNegK_;

  bool runOnlyWithRelationId_;

public:
  Parameters() {initialize();}
  ~Parameters() {}

  void initialize();

  void readParamsFile(string fname);
  void printParams();

  void addDirectory(string directory) {directory_ = directory;}
  string getDirectory() {return directory_;}
  void addMaxComplexity(int maxComplexity) {maxComplexity_ = maxComplexity;}
  int getMaxComplexity() {return maxComplexity_;}

  void addModelNumber(int modelNumber) {modelNumber_ = modelNumber;}
  int getModelNumber() {return modelNumber_;}

  void addRelationId(int relationId) {relationId_ = relationId;}
  int getRelationId() {return relationId_;}

  void addUseRelationInRules(bool useRelationInRules) {useRelationInRules_ = useRelationInRules;}
  bool getUseRelationInRules() {return useRelationInRules_;}

  void addUseRulesOfLengthOne(bool useRulesOfLengthOne) {useRulesOfLengthOne_ = useRulesOfLengthOne;}
  bool getUseRulesOfLengthOne() {return useRulesOfLengthOne_;}

  void addUseRelationInLengthOneRule(bool useRelationInLengthOneRule) {useRelationInLengthOneRule_ = useRelationInLengthOneRule;}
  bool getUseRelationInLengthOneRule() {return useRelationInLengthOneRule_;}

  void addMaxRuleLength(int maxRuleLength) {maxRuleLength_ = maxRuleLength;}
  int getMaxRuleLength() {return maxRuleLength_;}

  void addReportStatsRightRemoval(bool reportStatsRightRemoval) {reportStatsRightRemoval_ = reportStatsRightRemoval;}
  bool getReportStatsRightRemoval() {return reportStatsRightRemoval_;}

  void addReportStatsLeftRemoval(bool reportStatsLeftRemoval) {reportStatsLeftRemoval_ = reportStatsLeftRemoval;}
  bool getReportStatsLeftRemoval() {return reportStatsLeftRemoval_;}

  void addReportStatsAllRemoval(bool reportStatsAllRemoval) {reportStatsAllRemoval_ = reportStatsAllRemoval;}
  bool getReportStatsAllRemoval() {return reportStatsAllRemoval_;}

  void addPenaltyOnComplexity(double penaltyOnComplexity) {penaltyOnComplexity_ = penaltyOnComplexity;}
  double getPenaltyOnComplexity() {return penaltyOnComplexity_;}

  void addPrintScoresToFile(bool printScoresToFile) {printScoresToFile_ = printScoresToFile;}
  bool getPrintScoresToFile() {return printScoresToFile_;}

  void addRunOnlyWithRelationId(bool runOnlyWithRelationId) {runOnlyWithRelationId_ = runOnlyWithRelationId;}
  bool getRunOnlyWithRelationId() {return runOnlyWithRelationId_;}

  void addWriteLpFile(bool writeLpFile) {writeLpFile_ = writeLpFile;}
  bool getWriteLpFile() {return writeLpFile_;}

  void addRankingType(int rankingType) {rankingType_ = rankingType;}
  int getRankingType() {return rankingType_;}

  void addUseReverseArcsInRules(bool useReverseArcsInRules) {useReverseArcsInRules_ = useReverseArcsInRules;}
  bool getUseReverseArcsInRules() {return useReverseArcsInRules_;}

  void addRunOnlyForRelationsInTest(bool runOnlyForRelationsInTest) {runOnlyForRelationsInTest_ = runOnlyForRelationsInTest;}
  bool getRunOnlyForRelationsInTest() {return runOnlyForRelationsInTest_;}

  void addRunForReverseRelations(bool runForReverseRelations) {runForReverseRelations_ = runForReverseRelations;}
  bool getRunForReverseRelations() {return runForReverseRelations_;}

  void addRunColumnGeneration(bool runColumnGeneration) {runColumnGeneration_ = runColumnGeneration;}
  bool getRunColumnGeneration() {return runColumnGeneration_;}

  void addGenerateRules(int generateRules) {generateRules_ = generateRules;}
  int getGenerateRules() {return generateRules_;}

  void addRunFindBestComplexity(bool runFindBestComplexity) {runFindBestComplexity_ = runFindBestComplexity;}
  bool getRunFindBestComplexity() {return runFindBestComplexity_;}

  void addFindBestComplexityRankingType(int rankingType) {findBestComplexityRankingType_ = rankingType;}
  int getFindBestComplexityRankingType() {return findBestComplexityRankingType_;}

  void addAlphaConvexCombinationModel3(double alphaConvexCombinationModel3) {alphaConvexCombinationModel3_ = alphaConvexCombinationModel3;}
  double getAlphaConvexCombinationModel3() {return alphaConvexCombinationModel3_;}

  //  void addPenaltyOnNegativePairs(double penaltyOnNegativePairs) {penaltyOnNegativePairs_ = penaltyOnNegativePairs;}
  //  double getPenaltyOnNegativePairs() {return penaltyOnNegativePairs_;}
  void addPenaltyOnNegativePairs(double penaltyOnNegativePairs) {penaltyOnNegativePairs_.push_back(penaltyOnNegativePairs);}
  vector<double>& getPenaltyOnNegativePairs() {return penaltyOnNegativePairs_;}
  bool getAddPenaltyOnNegativePairs() {return addPenaltyOnNegativePairs_;}

  void addUseBreadthFirstSearch(bool useBreadthFirstSearch) {useBreadthFirstSearch_ = useBreadthFirstSearch;}
  bool getUseBreadthFirstSearch() {return useBreadthFirstSearch_;}

  void addRepeatedNodesAllowed(bool repeatedNodesAllowed) {repeatedNodesAllowed_ = repeatedNodesAllowed;}
  bool getRepeatedNodesAllowed() {return repeatedNodesAllowed_;}

  void addRunMode(int runMode) {runMode_ = runMode;}
  int getRunMode() {return runMode_;}

  void addMaxItersColumnGeneration(int maxItersColumnGeneration) {maxItersColumnGeneration_ = maxItersColumnGeneration;}
  int getMaxItersColumnGeneration() {return maxItersColumnGeneration_;}

  void addSpeedUpComputationNegK(bool speedUpComputationNegK) {speedUpComputationNegK_ = speedUpComputationNegK;}
  bool getSpeedUpComputationNegK() {return speedUpComputationNegK_;}

};

#endif
