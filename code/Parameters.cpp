// © Copyright IBM Corporation 2022. All Rights Reserved.
// LICENSE: Eclipse Public License - v 2.0, https://opensource.org/licenses/EPL-2.0
// SPDX-License-Identifier: EPL-2.0

#include "Parameters.hpp"

#include <iomanip>
#include <cassert>
#include <queue>

using namespace std;

void Parameters::initialize()
{
  relationId_ = -1;
  penaltyOnComplexity_ = 0.0;
  alphaConvexCombinationModel3_ = 1.0;
  //  penaltyOnNegativePairs_ = 0.0;
  penaltyOnNegativePairs_.push_back(0.0);
  addPenaltyOnNegativePairs_ = false;
  runOnlyWithRelationId_ = false;  
  repeatedNodesAllowed_ = false;
  runMode_ = 0;
  maxItersColumnGeneration_ = 15;
  speedUpComputationNegK_ = false;
}

void Parameters::readParamsFile(string fname)
{
  string s;
  string stemp1, stemp2, stemp3;

  ifstream infile(fname.c_str());

  while (getline( infile, s )) {
    int ent1, ent2;
    istringstream ss( s );
    //    ss >> stemp1 >> stemp2;
    int counter=1;
    vector<string> extraValues;
    while (ss) {
      string s;
      if (!getline( ss, s, ' ' )) break;
      if(counter==1)
	stemp1 = s;
      else if(counter==2)
	stemp2 = s;
      else if(counter>2)
	extraValues.push_back(s);
      counter++;
    }
    if(stemp1 == "data_directory")
      directory_ = stemp2;
    else if(stemp1 == "max_complexity")
      maxComplexity_ =  atoi(stemp2.c_str());
    else if(stemp1 == "model_number")
      modelNumber_ = atoi(stemp2.c_str());
    else if(stemp1 == "relation_id")
      relationId_ = atoi(stemp2.c_str());
    else if(stemp1 == "use_relation_in_rules") {
      if(stemp2 == "true")
	useRelationInRules_ = true;
      else
	useRelationInRules_ = false;
    }
    else if(stemp1 == "use_rules_of_length_one") {
      if(stemp2 == "true")
	useRulesOfLengthOne_ = true;
      else
	useRulesOfLengthOne_ = false;
    }
    else if(stemp1 == "use_relation_in_length_one_rule")
      if(stemp2 == "true")
	useRelationInLengthOneRule_ = true;
      else
	useRelationInLengthOneRule_ = false;
    else if(stemp1 == "max_rule_length")
      maxRuleLength_ = atoi(stemp2.c_str());
    else if(stemp1 == "report_stats_right_removal") {
      if(stemp2 == "true")
	reportStatsRightRemoval_ = true;
      else
	reportStatsRightRemoval_ = false;
    }
    else if(stemp1 == "report_stats_left_removal") {
      if(stemp2 == "true")
	reportStatsLeftRemoval_ = true;
      else
	reportStatsLeftRemoval_ = false;
    }
    else if(stemp1 == "report_stats_all_removal") {
      if(stemp2 == "true")
	reportStatsAllRemoval_ = true;
      else
	reportStatsAllRemoval_ = false;
    }
    else if(stemp1 == "penalty_on_complexity")
      penaltyOnComplexity_ =  atof(stemp2.c_str());
    else if(stemp1 == "print_scores_to_file") {
      if(stemp2 == "true")
	printScoresToFile_ = true;
      else
	printScoresToFile_ = false;
    }
    else if(stemp1 == "write_lp_file") {
      if(stemp2 == "true")
	writeLpFile_ = true;
      else
	writeLpFile_ = false;
    }
    else if(stemp1 == "ranking_type") {
      rankingType_ =  atoi(stemp2.c_str());
      findBestComplexityRankingType_ = rankingType_;
    }
    else if(stemp1 == "use_reverse_arcs_in_rules") {
      if(stemp2 == "true")
	useReverseArcsInRules_ = true;
      else
	useReverseArcsInRules_ = false;
    }
    else if(stemp1 == "run_only_for_relations_in_test") {
      if(stemp2 == "true")
	runOnlyForRelationsInTest_ = true;
      else
	runOnlyForRelationsInTest_ = false;
    }
    else if(stemp1 == "run_for_reverse_relations") {
      if(stemp2 == "true")
	runForReverseRelations_ = true;
      else
	runForReverseRelations_ = false;
    }
    else if(stemp1 == "run_column_generation") {
      if(stemp2 == "true")
	runColumnGeneration_ = true;
      else
	runColumnGeneration_ = false;
    }
    else if(stemp1 == "generate_rules")
      generateRules_ =  atoi(stemp2.c_str());
    else if(stemp1 == "run_find_best_complexity") {
      if(stemp2 == "true")
	runFindBestComplexity_ = true;
      else
	runFindBestComplexity_ = false;
    }
    else if(stemp1 == "find_best_complexity_ranking_type")
      findBestComplexityRankingType_ =  atoi(stemp2.c_str());
    else if(stemp1 == "alpha_convex_combination_model3")
      alphaConvexCombinationModel3_ =  atof(stemp2.c_str());
    else if(stemp1 == "penalty_on_negative_pairs") {
      //      penaltyOnNegativePairs_ =  atof(stemp2.c_str());
      penaltyOnNegativePairs_[0] = atof(stemp2.c_str());
      for(int i=0; i<(int)extraValues.size(); i++)
	penaltyOnNegativePairs_.push_back(atof(extraValues[i].c_str()));
      if(penaltyOnNegativePairs_.size()>1 || penaltyOnNegativePairs_[0]>0.0)
	addPenaltyOnNegativePairs_ = true;
    }
    else if(stemp1 == "use_breadth_first_search") {
      if(stemp2 == "true")
	useBreadthFirstSearch_ = true;
      else
	useBreadthFirstSearch_ = false;
    }
    else if(stemp1 == "repeated_nodes_allowed") {
      if(stemp2 == "true")
	repeatedNodesAllowed_ = true;
      else
	repeatedNodesAllowed_ = false;
    }
    else if(stemp1 == "run_mode") {
      runMode_ =  atoi(stemp2.c_str());
    }
    else if(stemp1 == "max_iters_column_generation")
      maxItersColumnGeneration_ =  atoi(stemp2.c_str());
    else if(stemp1 == "speed_up_computation_neg_k") {
      if(stemp2 == "true")
	speedUpComputationNegK_ = true;
      else
	speedUpComputationNegK_ = false;
    }

  }

  if(useRelationInRules_ == false)
    useRelationInLengthOneRule_ = false;    

  if(relationId_ >= 0)
    runOnlyWithRelationId_ = true;
}

void Parameters::printParams()
{
  cout<<"-------------------------"<<endl;
  cout<<"Parameters:"<<endl;
  cout<<"data_directory "<<directory_<<endl;
  cout<<"max_complexity "<<maxComplexity_<<endl;
  cout<<"model_number "<<modelNumber_<<endl;
  cout<<"relation_id "<<relationId_<<endl;
  if(useRelationInRules_)
    cout<<"use_relation_in_rules true"<<endl;
  else
    cout<<"use_relation_in_rules false"<<endl;
  if(useRulesOfLengthOne_)
    cout<<"use_rules_of_length_one true"<<endl;
  else
    cout<<"use_rules_of_length_one false"<<endl;
  if(useRelationInLengthOneRule_)
    cout<<"use_relation_in_length_one_rule true"<<endl;
  else
    cout<<"use_relation_in_length_one_rule false"<<endl;
  cout<<"max_rule_length "<<maxRuleLength_<<endl;
  if(reportStatsRightRemoval_)
    cout<<"report_stats_right_removal true"<<endl;
  else
    cout<<"report_stats_right_removal false"<<endl;
  if(reportStatsLeftRemoval_)
    cout<<"report_stats_left_removal true"<<endl;
  else
    cout<<"report_stats_left_removal false"<<endl;
  if(reportStatsAllRemoval_)
    cout<<"report_stats_all_removal true"<<endl;
  else
    cout<<"report_stats_all_removal false"<<endl;
  cout<<"penalty_on_complexity "<<penaltyOnComplexity_<<endl;
  if(printScoresToFile_)
    cout<<"print_scores_to_file true"<<endl;
  else
    cout<<"print_scores_to_file false"<<endl;
  if(writeLpFile_)
    cout<<"write_lp_file true"<<endl;
  else
    cout<<"write_lp_file false"<<endl;
  cout<<"ranking_type "<<rankingType_<<endl;
  if(useReverseArcsInRules_)
    cout<<"use_reverse_arcs_in_rules true"<<endl;
  else
    cout<<"use_reverse_arcs_in_rules false"<<endl;
  if(runOnlyForRelationsInTest_)
    cout<<"run_only_for_relations_in_test true"<<endl;
  else
    cout<<"run_only_for_relations_in_test false"<<endl;
  if(runForReverseRelations_)
    cout<<"run_for_reverse_relations true"<<endl;
  else
    cout<<"run_for_reverse_relations false"<<endl;
  if(runColumnGeneration_)
    cout<<"run_column_generation true"<<endl;
  else
    cout<<"run_column_generation false"<<endl;
  cout<<"generate_rules "<<generateRules_<<endl;
  if(runFindBestComplexity_)
    cout<<"run_find_best_complexity true"<<endl;
  else
    cout<<"run_find_best_complexity false"<<endl;
  cout<<"find_best_complexity_ranking_type "<<findBestComplexityRankingType_<<endl;
  cout<<"alpha_convex_combination_model3 "<<alphaConvexCombinationModel3_<<endl;
  //  cout<<"penalty_on_negative_pairs "<<penaltyOnNegativePairs_<<endl;
  cout<<"penalty_on_negative_pairs";
  for(int i=0; i<(int)penaltyOnNegativePairs_.size(); i++)
    cout<<" "<<penaltyOnNegativePairs_[i];
  cout<<endl;
  if(useBreadthFirstSearch_)
    cout<<"use_breadth_first_search true"<<endl;
  else
    cout<<"use_breadth_first_search false"<<endl;
  if(repeatedNodesAllowed_)
    cout<<"repeated_nodes_allowed true"<<endl;
  else
    cout<<"repeated_nodes_allowed false"<<endl;
  cout<<"run_mode "<<runMode_<<endl;
  cout<<"max_iters_column_generation "<<maxItersColumnGeneration_<<endl;
  if(speedUpComputationNegK_)
    cout<<"speed_up_computation_neg_k true"<<endl;
  else
    cout<<"speed_up_computation_neg_k false"<<endl;
  cout<<"-------------------------"<<endl;  
}
