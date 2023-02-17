// © Copyright IBM Corporation 2022. All Rights Reserved.
// LICENSE: Eclipse Public License - v 2.0, https://opensource.org/licenses/EPL-2.0
// SPDX-License-Identifier: EPL-2.0

#ifndef __DATA_HPP__
#define __DATA_HPP__

#include "Parameters.hpp"

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
#include <set>
#include <limits.h>

using namespace std;

class Rule {
private:
  vector<int> relationIds_;
  vector<bool> isReverseArc_;
  
public:
  Rule() {}
  ~Rule() {}

  void addRelationId(int id, bool isReverseArc=false)
  {
    relationIds_.push_back(id);
    isReverseArc_.push_back(isReverseArc);
  }
  vector<int>& getRelationIds() {return relationIds_;}
  vector<bool>& getIsReverseArc() {return isReverseArc_;}
  int getLengthRule() {return (int)relationIds_.size();}

  bool operator==(Rule& r);
  bool operator<(Rule& r);
};

class TestData {
private:
  vector<pair<int,int> > entpairs_;
  vector<vector<pair<int,int> > > relentpairs_;
  set<pair<int,int> > setentpairs_;
  
public:
  TestData() {}
  ~TestData() {cleanup();}

  void cleanup();
  void setupNumberOfRelations(int nrelations);

  void addEntityPair(int ent1, int ent2);
  void addEntityPairAndRelation(int ent1, int relationid, int ent2);

  vector<pair<int,int> >& getEntityPairs() {return entpairs_;}
  int getNumEntityPairs() {return (int)entpairs_.size();}
  vector<pair<int,int> >& getEntityPairs(int relationid) {return relentpairs_[relationid];}
  int getNumEntityPairs(int relationid) {return (int)relentpairs_[relationid].size();}
  set<pair<int,int> >& getSetEntityPairs() {return setentpairs_;}
};

class Node {
private:
  int id_;

public:
  Node(int id):id_(id) {}
  ~Node() {}

  int getId() {return id_;}
};

class Arc {
private:
  int id_;
  Node* tail_;
  Node* head_;
  int idrelation_;

public:
  Arc(int id, Node* tail, Node* head, int idrelation):id_(id),tail_(tail),head_(head),idrelation_(idrelation) {}
  ~Arc() {}

  int getId() {return id_;}
  Node* getTail() {return tail_;}
  Node* getHead() {return head_;}
  int getIdRelation() {return idrelation_;}
};

class Query {
private:
  vector<pair<int,int> > entpairs_;
  vector<Arc*> outArcsWithRelation_; // these two arrays are matched for the training data.

public:
  Query() {}
  ~Query() {cleanup();}

  void cleanup();
  void addArc(Arc* arc) {outArcsWithRelation_.push_back(arc);}
  void addEntityPair(int ent1, int ent2);

  vector<pair<int,int> >& getEntityPairs() {return entpairs_;}
  int getNumEntityPairs() {return (int)entpairs_.size();}
  vector<Arc*>& getOutArcsWithRelation() {return outArcsWithRelation_;}

  void resetQuery() {entpairs_.clear(); outArcsWithRelation_.clear();}
};

class Data {
private:
  vector<Node*> nodes_;
  vector<Arc*> arcs_;
  vector<vector<Arc*> > outarcs_;
  vector<vector<Arc*> > inarcs_;
  vector<string> entities_;
  vector<string> relations_;
  map<string,int> maprelations_;
  vector<vector<bool> > relnodehasarc_;
  vector<vector<bool> > relnodehasinvarc_;
  Query query_;
  vector<Query> queries_;
  TestData testdata_;
  TestData validdata_;
  int maxcomplexity_;
  bool repeatedNodesAllowed_;

public:
  Data(int maxcomplexity)
    :maxcomplexity_(maxcomplexity) {}
  ~Data();

  void cleanup();
  void readStringIntFile(string fname, vector<string>& v,
			 map<string,int>& m);
  void readQueryFile(string fname, map<string,int>& mapentities);
  void readTestFile(string fname, TestData& testdata, map<string,int>& mapentities);
  void readData(Parameters& params, string dataFile="train.txt");

  vector<Node*>& getNodes() {return nodes_;}
  int getNumberNodes() {return (int)nodes_.size();}
  vector<Arc*>& getArcs() {return arcs_;}
  vector<vector<Arc*> >& getOutArcs() {return outarcs_;}
  vector<vector<Arc*> >& getInArcs() {return inarcs_;}
  vector<string>& getEntities() {return entities_;}
  vector<string>& getRelations() {return relations_;}
  map<string,int>& getMapRelations() {return  maprelations_;}  
  int getNumberRelations() {return (int)relations_.size();}
  vector<vector<bool> >& getRelationNodeHasArc() {return relnodehasarc_;}
  vector<vector<bool> >& getRelationNodeHasInvArc() {return relnodehasinvarc_;}
  Query& getQuery() {return query_;}
  Query& getQuery(int relationId) {return queries_[relationId];}
  int getNumPairsQuery() {return query_.getNumEntityPairs();}
  int getNumPairsQuery(int relationId) {return queries_[relationId].getNumEntityPairs();}
  int getMaxComplexity() {return maxcomplexity_;}

  void getNumPaths(int relationId, Rule& rule, vector<int>& numpaths);

  bool hasPath(Rule& rule, pair<int,int>& pair);
  bool hasPath(Rule& rule, pair<int,int>& pair, 
	       Arc* outArcWithRelation);
  bool nodeIsNotInPath(vector<int>& path, int nodeid);
  bool nodeIsNotInPath(vector<vector<pair<int,int> > >& q, int k, int l, int nodeid);

  bool depthFirstSearch(Rule& rule, int destid, 
			vector<int>& path);
  bool depthFirstSearch(Rule& rule, int destid, 
			vector<int>& path, 
			Arc* outArcWithRelation);
  bool hasPathDfs(Rule& rule, pair<int,int>& pair);
  bool hasPathDfs(Rule& rule, pair<int,int>& pair,
		  Arc* outArcWithRelation);

  void rightEntitiesUsingBFS(Rule& rule, 
			     int origId,
			     set<int>& destIds);
  void rightEntitiesUsingDFS(Rule& rule, 
			     set<int>& destIds, 
			     vector<int>& path);
  void getRightEntities(Rule& rule, int origId, 
			set<int>& destIds, bool useBFS);
  void leftEntitiesUsingBFS(Rule& rule, 
			    int destId,
			    set<int>& origIds);
  void leftEntitiesUsingDFS(Rule& rule, 
			    set<int>& origIds, 
			    vector<int>& path);
  void getLeftEntities(Rule& rule, int destId, 
		       set<int>& origIds, bool useBFS);
  void rightEntitiesUsingBFS(Arc* outArcWithRelation,
			     Rule& rule, 
			     int origId,
			     set<int>& destIds);
  void rightEntitiesUsingDFS(Arc* outArcWithRelation,
			     Rule& rule, 
			     set<int>& destIds, 
			     vector<int>& path);
  void getRightEntities(Arc* outArcWithRelation, Rule& rule, 
			int origId, set<int>& destIds, 
			bool useBFS);
  void leftEntitiesUsingBFS(Arc* outArcWithRelation,
			    Rule& rule, 
			    int destId,
			    set<int>& origIds);
  void leftEntitiesUsingDFS(Arc* outArcWithRelation,
			    Rule& rule, 
			    set<int>& origIds, 
			    vector<int>& path);
  void getLeftEntities(Arc* outArcWithRelation, Rule& rule, 
		       int destId, set<int>& origIds,
		       bool useBFS);

  TestData& getTestData() {return testdata_;}
  TestData& getValidData() {return validdata_;}

  void createQueryFromTrainingData(Parameters& params);
  void createQueryFromTrainingData(int relationId);
};

#endif
