// © Copyright IBM Corporation 2022. All Rights Reserved.
// LICENSE: Eclipse Public License - v 2.0, https://opensource.org/licenses/EPL-2.0
// SPDX-License-Identifier: EPL-2.0

#include "Data.hpp"

//#include <iostream>
#include <iomanip>
#include <cassert>
#include <queue>

using namespace std;

bool Rule::operator==(Rule& r)
{ 
  return getRelationIds() == r.getRelationIds() &&
    getIsReverseArc() == r.getIsReverseArc();
}

bool Rule::operator<(Rule& r)
{ 
  // first compare the lengths of the rules
  if(getLengthRule() < r.getLengthRule())
    return true;
  else if(getLengthRule() > r.getLengthRule())
    return false;

  // for rules of equal length compare the contents
  // of the vectors
  if(getRelationIds() < r.getRelationIds()) {
    if(getIsReverseArc() <= r.getIsReverseArc())
      return true;
    else
      return false;
  }

  if(getRelationIds() == r.getRelationIds() &&
     getIsReverseArc() < r.getIsReverseArc())
      return true;

  return false;
}

void Query::addEntityPair(int ent1, int ent2)
{
  entpairs_.push_back(pair<int,int>(ent1,ent2));
}

void Query::cleanup()
{
  entpairs_.clear();
  outArcsWithRelation_.clear();
}

void TestData::cleanup()
{
  entpairs_.clear();
  relentpairs_.clear();
}

void TestData::setupNumberOfRelations(int nrelations)
{
  relentpairs_.resize(nrelations);
}

void TestData::addEntityPair(int ent1, int ent2)
{
  entpairs_.push_back(pair<int,int>(ent1,ent2));
}

void TestData::addEntityPairAndRelation(int ent1, int relationid, int ent2)
{
  assert(relationid <= (int)relentpairs_.size());
  relentpairs_[relationid].push_back(pair<int,int>(ent1,ent2));
  setentpairs_.insert(pair<int,int>(ent1,ent2));
}

Data::~Data()
{
  cleanup();
}

void Data::cleanup()
{
  for (int i=0; i<(int)arcs_.size(); i++)
    delete arcs_[i];

  for (int i=0; i<(int)nodes_.size(); i++)
    delete nodes_[i];

  nodes_.clear();
  arcs_.clear();
  outarcs_.clear();
  inarcs_.clear();
  entities_.clear();
  relations_.clear();
  maprelations_.clear();
  relnodehasarc_.clear();
  relnodehasinvarc_.clear();
  query_.cleanup();
  for(int i=0; i<(int)queries_.size(); i++)
    queries_[i].cleanup();
  queries_.clear();
  testdata_.cleanup();
  validdata_.cleanup();
  maxcomplexity_=0;
  repeatedNodesAllowed_=false;

}

void Data::readStringIntFile(string fname, vector<string>& v,
			     map<string,int>& m)
{
  string s;
  string stemp1, stemp2;

  ifstream infile(fname.c_str());

  while (getline( infile, s )) {
    istringstream ss( s );
    ss >> stemp1 >> stemp2;
    int id = atoi(stemp2.c_str());
    assert(id == (int)v.size());
    v.push_back(stemp1);
    m[stemp1] = id;
  }

#if 0
  cout<<"v:"<<endl;
  for(int i=0; i<(int)v.size(); i++)
    cout<<v[i]<<" "<<i<<endl;

  cout<<"m:"<<endl;
  for(map<string,int>::iterator it=m.begin(); it!=m.end(); it++)
    cout<<it->first<<" "<<it->second<<endl;
#endif
}

void Data::readQueryFile(string fname, map<string,int>& mapentities)
{
  string s;
  string stemp1, stemp2, stemp3;

  ifstream infile(fname.c_str());

  while (getline( infile, s )) {
    int ent1, ent2;
    istringstream ss( s );
    ss >> stemp1 >> stemp3 >> stemp2;
    //    ss >> stemp2 >> stemp3 >> stemp1; // WN18RR
    map<string,int>::iterator it = mapentities.find(stemp1);
    if (it != mapentities.end())
      ent1 = it->second;
    it = mapentities.find(stemp2);
    if (it != mapentities.end())
      ent2 = it->second;
    query_.addEntityPair(ent1,ent2);
  }

#if 0
  vector<pair<int,int> >& pairs = query_.getEntityPairs();
  cout<<"query:"<<endl;
  for(int i=0; i<(int)pairs.size(); i++)
    cout<<entities_[pairs[i].first]<<" "<<entities_[pairs[i].second]<<endl;
#endif
}

void Data::readTestFile(string fname, TestData& testdata, map<string,int>& mapentities)
{
  string s;
  string stemp1, stemp2, stemp3;

  ifstream infile(fname.c_str());

  while (getline( infile, s )) {
    int ent1, ent2, idrelation;
    istringstream ss( s );
    ss >> stemp1 >> stemp2 >> stemp3;
    map<string,int>::iterator it = mapentities.find(stemp1);
    if (it != mapentities.end())
      ent1 = it->second;
    it = mapentities.find(stemp3);
    if (it != mapentities.end())
      ent2 = it->second;

    it = maprelations_.find(stemp2);
    if (it != maprelations_.end())
      idrelation = it->second;
    testdata.addEntityPairAndRelation(ent1,idrelation,ent2);

  }

#if 0
  for(int i=0; i<(int)relations_.size(); i++) {
    vector<pair<int,int> >& pairs = testdata.getEntityPairs(i);
    cout<<"test "<<relations_[i]<<": "<<endl;
    for(int i=0; i<(int)pairs.size(); i++)
      cout<<"("<<entities_[pairs[i].first]<<","<<entities_[pairs[i].second]<<")";
    cout<<endl;
  }
#endif
#if 0
  vector<pair<int,int> >& pairs = testdata.getEntityPairs();
  cout<<"test:"<<endl;
  for(int i=0; i<(int)pairs.size(); i++)
    cout<<entities_[pairs[i].first]<<" "<<entities_[pairs[i].second]<<endl;
#endif
}

void Data::readData(Parameters& params, string dataFile)
{
  string dname = params.getDirectory();
  repeatedNodesAllowed_ = params.getRepeatedNodesAllowed();

  // read entities
  map<string,int> mapentities;
  readStringIntFile(dname+"/entity2id.txt", entities_, mapentities);

  // create nodes
  nodes_.resize(entities_.size());
  for (int i=0; i<(int)entities_.size(); i++) {
    Node* node = new Node(i);
    nodes_[i] = node;
  }

  outarcs_.resize(entities_.size());
  inarcs_.resize(entities_.size());

  // read relations
  readStringIntFile(dname+"/relation2id.txt", relations_, maprelations_);
  testdata_.setupNumberOfRelations((int)relations_.size());
  validdata_.setupNumberOfRelations((int)relations_.size());
  queries_.resize((int)relations_.size());

  relnodehasarc_.resize(relations_.size());
  relnodehasinvarc_.resize(relations_.size());
  for(int i=0; i<(int)relations_.size(); i++) {
    relnodehasarc_[i].resize(entities_.size(),false);
    relnodehasinvarc_[i].resize(entities_.size(),false);
  }

  // create arcs
  string s;
  string stemp1, stemp2, stemp3;

  string fname = dname+"/"+dataFile;
  ifstream infile(fname.c_str());

  int idarc = 0;
  while (getline( infile, s )) {
    int idtail, idrelation, idhead;

    istringstream ss( s );
    ss >> stemp1 >> stemp2 >> stemp3;

    map<string,int>::iterator it = mapentities.find(stemp1);
    if (it != mapentities.end())
      idtail = it->second;
    it = maprelations_.find(stemp2);
    if (it != maprelations_.end())
      idrelation = it->second;
    it = mapentities.find(stemp3);
    if (it != mapentities.end())
      idhead = it->second;

    Arc* arc = new Arc(idarc, nodes_[idtail], nodes_[idhead], idrelation);
    idarc++;
    arcs_.push_back(arc);
    outarcs_[idtail].push_back(arc);
    inarcs_[idhead].push_back(arc);
    relnodehasarc_[idrelation][idtail] = true;
    relnodehasinvarc_[idrelation][idhead] = true;
  }

#if 0
  cout<<"arcs:"<<endl;;
  for (int i=0; i<(int)arcs_.size(); i++) {
    string tail = entities_[arcs_[i]->getTail()->getId()];
    string relation = relations_[arcs_[i]->getIdrelation()];
    string head = entities_[arcs_[i]->getHead()->getId()];
    cout<<tail<<" "<<relation<<" "<<head<<endl;
  }
#endif

#if 0
  for (int i=0; i<(int)outarcs_.size(); i++) {
    cout<<entities_[i]<<": ";
    for (int j=0; j<(int)outarcs_[i].size(); j++) {
      string tail = entities_[outarcs_[i][j]->getTail()->getId()];
      string relation = relations_[outarcs_[i][j]->getIdRelation()];
      string head = entities_[outarcs_[i][j]->getHead()->getId()];
      cout<<tail<<" "<<relation<<" "<<head<<" | ";
    }
    cout<<endl;
  }
#endif

#if 1
  //  createQueryFromTrainingData(params);
#else
  readQueryFile(dname+"/query.txt", mapentities);
#endif

  readTestFile(dname+"/test.txt", testdata_, mapentities);
  readTestFile(dname+"/valid.txt", validdata_, mapentities);

}

void Data::getNumPaths(int relationId, Rule& rule, vector<int>& numpaths)
{
  int numpairs = getNumPairsQuery(relationId);
  vector<pair<int,int> >& pairs = queries_[relationId].getEntityPairs();
  vector<Arc*>& outArcsWithRelation = queries_[relationId].getOutArcsWithRelation();

  numpaths.resize(numpairs);

  for (int i=0; i<numpairs; i++) {
    if (hasPath(rule, pairs[i], outArcsWithRelation[i]))
      numpaths[i] = 1;
    else
      numpaths[i] = 0;
#if 0
    if(numpaths[i]==1) {
      cout<<"yes_path ";
      int len = rule.getLengthRule();
      vector<int>& rel = rule.getRelationIds();
      cout<<entities_[pairs[i].first];
      for(int j=0; j<len; j++)
	cout<<" "<<relations_[rel[j]];
      cout<<" "<<entities_[pairs[i].second]<<endl;
    }
#endif
  }

}

bool Data::hasPath(Rule& rule, pair<int,int>& pair)
{
  return hasPathDfs(rule, pair);
}

bool Data::hasPath(Rule& rule, pair<int,int>& pair, 
		   Arc* outArcWithRelation)
{
  return hasPathDfs(rule, pair, outArcWithRelation);
}

bool Data::nodeIsNotInPath(vector<int>& path, int nodeid)
{
  if(repeatedNodesAllowed_) {
    return true;
  }
  for (int i=0; i<(int)path.size(); i++) {
    if(path[i] == nodeid)
      return false;
  }
  return true;
}

bool Data::nodeIsNotInPath(vector<vector<pair<int,int> > >& q, int k, int l, int nodeid)
{
  int index = l;
  for(int i=k; i>=0; i--) {
    assert(index>=0 && index<q[i].size());
    pair<int,int>& p = q[i][index];
    if(p.first == nodeid)
      return false;
    index = p.second;
  }
  return true;
}

bool Data::depthFirstSearch(Rule& rule, int destid, 
			    vector<int>& path)
{
  vector<int>& relationIds = rule.getRelationIds();
  vector<bool>& isReverseArc = rule.getIsReverseArc();

  int lastnodeid = path.back();
  int pathlength = ((int)path.size())-1;

  if (pathlength == rule.getLengthRule()) {
    if(lastnodeid == destid)
      return true;
    else
      return false;
  }

  if(!isReverseArc[pathlength]) {
    if(relnodehasarc_[relationIds[pathlength]][path.back()]) {
      vector<Arc*>& arcs = outarcs_[path.back()];
      for (int i=0; i<(int)arcs.size(); i++) {
	if(arcs[i]->getIdRelation() == relationIds[pathlength]) {
	  int newnodeid = arcs[i]->getHead()->getId();
	  if (nodeIsNotInPath(path,newnodeid)) {
	    path.push_back(newnodeid);
	    bool haspath = depthFirstSearch(rule, destid, path);
	    if (haspath)
	      return true;
	    path.pop_back();
	  }
	}
      }
    }
  }
  else {
    if(relnodehasinvarc_[relationIds[pathlength]][path.back()]) {
      vector<Arc*>& arcs = inarcs_[path.back()];
      for (int i=0; i<(int)arcs.size(); i++) {
	if(arcs[i]->getIdRelation() == relationIds[pathlength]) {
	  int newnodeid = arcs[i]->getTail()->getId();
	  if (nodeIsNotInPath(path,newnodeid)) {
	    path.push_back(newnodeid);
	    bool haspath = depthFirstSearch(rule, destid, path);
	    if (haspath)
	      return true;
	    path.pop_back();
	  }
	}
      }
    }
  }

  return false;
}

bool Data::depthFirstSearch(Rule& rule, int destid, 
			    vector<int>& path, 
			    Arc* outArcWithRelation)
{
  vector<int>& relationIds = rule.getRelationIds();
  vector<bool>& isReverseArc = rule.getIsReverseArc();

  int lastnodeid = path.back();
  int pathlength = ((int)path.size())-1;

  if (pathlength == rule.getLengthRule()) {
    if(lastnodeid == destid)
      return true;
    else
      return false;
  }

  if(!isReverseArc[pathlength]) {
    if(relnodehasarc_[relationIds[pathlength]][path.back()]) {
      vector<Arc*>& arcs = outarcs_[path.back()];
      for (int i=0; i<(int)arcs.size(); i++) {
	if(arcs[i]->getIdRelation() == relationIds[pathlength] &&
	   arcs[i] != outArcWithRelation) {
	  int newnodeid = arcs[i]->getHead()->getId();
	  if (nodeIsNotInPath(path,newnodeid)) {
	    path.push_back(newnodeid);
	    bool haspath = depthFirstSearch(rule, destid, path, 
					    outArcWithRelation);
	    if (haspath)
	      return true;
	    path.pop_back();
	  }
	}
      }
    }
  }
  else {
    if(relnodehasinvarc_[relationIds[pathlength]][path.back()]) {
      vector<Arc*>& arcs = inarcs_[path.back()];
      for (int i=0; i<(int)arcs.size(); i++) {
	if(arcs[i]->getIdRelation() == relationIds[pathlength] &&
	   arcs[i] != outArcWithRelation) {
	  int newnodeid = arcs[i]->getTail()->getId();
	  if (nodeIsNotInPath(path,newnodeid)) {
	    path.push_back(newnodeid);
	    bool haspath = depthFirstSearch(rule, destid, path,
					    outArcWithRelation);
	    if (haspath)
	      return true;
	    path.pop_back();
	  }
	}
      }
    }
  }

  return false;
}

bool Data::hasPathDfs(Rule& rule, pair<int,int>& pair)
{

  vector<int>& relationIds = rule.getRelationIds();
  assert(rule.getLengthRule() >= 1);

  int origid = pair.first;
  int destid = pair.second;

  vector<int> path;
  path.push_back(origid);
  
  bool haspath = depthFirstSearch(rule, destid, path);

  return haspath;
}

 bool Data::hasPathDfs(Rule& rule, pair<int,int>& pair,
		       Arc* outArcWithRelation)
{

  vector<int>& relationIds = rule.getRelationIds();
  assert(rule.getLengthRule() >= 1);

  int origid = pair.first;
  int destid = pair.second;

  vector<int> path;
  path.push_back(origid);
  
  bool haspath = depthFirstSearch(rule, destid, path, outArcWithRelation);

  return haspath;
}

void Data::rightEntitiesUsingBFS(Rule& rule, 
				 int origId,
				 set<int>& destIds)
{
  vector<int>& relationIds = rule.getRelationIds();
  vector<bool>& isReverseArc = rule.getIsReverseArc();
  int rulelength = rule.getLengthRule();

  vector<vector<pair<int,int> > > q(rulelength); // each position corresponds to a level in the search tree. The first int is the nodeId and the second int is the index of the previous node in the path
  q[0].push_back(pair<int,int>(origId,-1));

  for(int k=0; k<rulelength-1; k++) {
    for(int l=0; l<(int)q[k].size(); l++) {
      pair<int,int>& p = q[k][l];
      if(!isReverseArc[k]) {
	if(relnodehasarc_[relationIds[k]][p.first]) {
	  vector<Arc*>& arcs = outarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[k]) {
	      int newnodeid = arcs[i]->getHead()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		q[k+1].push_back(pair<int,int>(newnodeid,l));
	    }
	  }
	}
      }
      else {
	if(relnodehasinvarc_[relationIds[k]][p.first]) {
	  vector<Arc*>& arcs = inarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[k]) {
	      int newnodeid = arcs[i]->getTail()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		q[k+1].push_back(pair<int,int>(newnodeid,l));
	    }
	  }
	}
      }
    }
  }

  {
    int k=rulelength-1;
    for(int l=0; l<(int)q[k].size(); l++) {
      pair<int,int>& p = q[k][l];
      if(!isReverseArc[k]) {
	if(relnodehasarc_[relationIds[k]][p.first]) {
	  vector<Arc*>& arcs = outarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[k]) {
	      int newnodeid = arcs[i]->getHead()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		destIds.insert(newnodeid);
	    }
	  }
	}
      }
      else {
	if(relnodehasinvarc_[relationIds[k]][p.first]) {
	  vector<Arc*>& arcs = inarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[k]) {
	      int newnodeid = arcs[i]->getTail()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		destIds.insert(newnodeid);
	    }
	  }
	}
      }
    }
  }

  return;
}

void Data::rightEntitiesUsingDFS(Rule& rule, 
				 set<int>& destIds, 
				 vector<int>& path)
{
  vector<int>& relationIds = rule.getRelationIds();
  vector<bool>& isReverseArc = rule.getIsReverseArc();

  int pathlength = ((int)path.size())-1;

  if (pathlength == rule.getLengthRule()) {
    int lastnodeid = path.back();
    destIds.insert(lastnodeid);
    return;
  }

  if(!isReverseArc[pathlength]) {
    if(relnodehasarc_[relationIds[pathlength]][path.back()]) {
      vector<Arc*>& arcs = outarcs_[path.back()];
      for (int i=0; i<(int)arcs.size(); i++) {
	if(arcs[i]->getIdRelation() == relationIds[pathlength]) {
	  int newnodeid = arcs[i]->getHead()->getId();
	  if (nodeIsNotInPath(path,newnodeid)) {
	    path.push_back(newnodeid);
	    rightEntitiesUsingDFS(rule, destIds, path);
	    path.pop_back();
	  }
	}
      }
    }
  }
  else {
    if(relnodehasinvarc_[relationIds[pathlength]][path.back()]) {
      vector<Arc*>& arcs = inarcs_[path.back()];
      for (int i=0; i<(int)arcs.size(); i++) {
	if(arcs[i]->getIdRelation() == relationIds[pathlength]) {
	  int newnodeid = arcs[i]->getTail()->getId();
	  if (nodeIsNotInPath(path,newnodeid)) {
	    path.push_back(newnodeid);
	    rightEntitiesUsingDFS(rule, destIds, path);
	    path.pop_back();
	  }
	}
      }
    }
  }

  return;
}

void Data::getRightEntities(Rule& rule, int origId, 
			    set<int>& destIds, bool useBFS)
{
  assert(rule.getLengthRule() >= 1);
  if(useBFS)
    rightEntitiesUsingBFS(rule, origId, destIds);
  else {
    vector<int> path;
    path.push_back(origId);
    rightEntitiesUsingDFS(rule, destIds, path);
  }
}

void Data::leftEntitiesUsingBFS(Rule& rule, 
				int destId,
				set<int>& origIds)
{
  vector<int>& relationIds = rule.getRelationIds();
  vector<bool>& isReverseArc = rule.getIsReverseArc();
  int rulelength = rule.getLengthRule();

  vector<vector<pair<int,int> > > q(rulelength); // each position corresponds to a level in the search tree. The first int is the nodeId and the second int is the index of the previous node in the path
  q[0].push_back(pair<int,int>(destId,-1));

  for(int k=0; k<rulelength-1; k++) {
    int position = rulelength - k - 1;
    for(int l=0; l<(int)q[k].size(); l++) {
      pair<int,int>& p = q[k][l];
      if(!isReverseArc[position]) {
	if(relnodehasinvarc_[relationIds[position]][p.first]) {
	  vector<Arc*>& arcs = inarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[position]) {
	      int newnodeid = arcs[i]->getTail()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		q[k+1].push_back(pair<int,int>(newnodeid,l));
	    }
	  }
	}
      }
      else {
	if(relnodehasarc_[relationIds[position]][p.first]) {
	  vector<Arc*>& arcs = outarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[position]) {
	      int newnodeid = arcs[i]->getHead()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		q[k+1].push_back(pair<int,int>(newnodeid,l));
	    }
	  }
	}
      }
    }
  }

  {
    int k=rulelength-1;
    int position = rulelength - k - 1;
    for(int l=0; l<(int)q[k].size(); l++) {
      pair<int,int>& p = q[k][l];
      if(!isReverseArc[position]) {
	if(relnodehasinvarc_[relationIds[position]][p.first]) {
	  vector<Arc*>& arcs = inarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[position]) {
	      int newnodeid = arcs[i]->getTail()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		origIds.insert(newnodeid);
	    }
	  }
	}
      }
      else {
	if(relnodehasarc_[relationIds[position]][p.first]) {
	  vector<Arc*>& arcs = outarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[position]) {
	      int newnodeid = arcs[i]->getHead()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		origIds.insert(newnodeid);
	    }
	  }
	}
      }
    }
  }

  return;
}

void Data::leftEntitiesUsingDFS(Rule& rule, 
				set<int>& origIds, 
				vector<int>& path)
{
  vector<int>& relationIds = rule.getRelationIds();
  vector<bool>& isReverseArc = rule.getIsReverseArc();

  int pathlength = ((int)path.size())-1;

  if (pathlength == rule.getLengthRule()) {
    int lastnodeid = path.back();
    origIds.insert(lastnodeid);
    return;
  }

  int position = rule.getLengthRule() - pathlength - 1;
  if(!isReverseArc[position]) {
    if(relnodehasinvarc_[relationIds[position]][path.back()]) {
      vector<Arc*>& arcs = inarcs_[path.back()];
      for (int i=0; i<(int)arcs.size(); i++) {
	if(arcs[i]->getIdRelation() == relationIds[position]) {
	  int newnodeid = arcs[i]->getTail()->getId();
	  if (nodeIsNotInPath(path,newnodeid)) {
	    path.push_back(newnodeid);
	    leftEntitiesUsingDFS(rule, origIds, path);
	    path.pop_back();
	  }
	}
      }
    }
  }
  else {
    if(relnodehasarc_[relationIds[position]][path.back()]) {
      vector<Arc*>& arcs = outarcs_[path.back()];
      for (int i=0; i<(int)arcs.size(); i++) {
	if(arcs[i]->getIdRelation() == relationIds[position]) {
	  int newnodeid = arcs[i]->getHead()->getId();
	  if (nodeIsNotInPath(path,newnodeid)) {
	    path.push_back(newnodeid);
	    leftEntitiesUsingDFS(rule, origIds, path);
	    path.pop_back();
	  }
	}
      }
    }
  }

  return;
}

void Data::getLeftEntities(Rule& rule, int destId, 
			   set<int>& origIds, bool useBFS)
{
  assert(rule.getLengthRule() >= 1);
  if(useBFS)
    leftEntitiesUsingBFS(rule, destId, origIds);
  else {
    vector<int> path;
    path.push_back(destId);
    leftEntitiesUsingDFS(rule, origIds, path);
  }
}

void Data::rightEntitiesUsingBFS(Arc* outArcWithRelation,
				 Rule& rule, 
				 int origId,
				 set<int>& destIds)
{
  vector<int>& relationIds = rule.getRelationIds();
  vector<bool>& isReverseArc = rule.getIsReverseArc();
  int rulelength = rule.getLengthRule();

  vector<vector<pair<int,int> > > q(rulelength); // each position corresponds to a level in the search tree. The first int is the nodeId and the second int is the index of the previous node in the path
  q[0].push_back(pair<int,int>(origId,-1));

  for(int k=0; k<rulelength-1; k++) {
    for(int l=0; l<(int)q[k].size(); l++) {
      pair<int,int>& p = q[k][l];
      if(!isReverseArc[k]) {
	if(relnodehasarc_[relationIds[k]][p.first]) {
	  vector<Arc*>& arcs = outarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[k] &&
	       arcs[i] != outArcWithRelation) {
	      int newnodeid = arcs[i]->getHead()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		q[k+1].push_back(pair<int,int>(newnodeid,l));
	    }
	  }
	}
      }
      else {
	if(relnodehasinvarc_[relationIds[k]][p.first]) {
	  vector<Arc*>& arcs = inarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[k] &&
	       arcs[i] != outArcWithRelation) {
	      int newnodeid = arcs[i]->getTail()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		q[k+1].push_back(pair<int,int>(newnodeid,l));
	    }
	  }
	}
      }
    }
  }

  {
    int k=rulelength-1;
    for(int l=0; l<(int)q[k].size(); l++) {
      pair<int,int>& p = q[k][l];
      if(!isReverseArc[k]) {
	if(relnodehasarc_[relationIds[k]][p.first]) {
	  vector<Arc*>& arcs = outarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[k] &&
	       arcs[i] != outArcWithRelation) {
	      int newnodeid = arcs[i]->getHead()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		destIds.insert(newnodeid);
	    }
	  }
	}
      }
      else {
	if(relnodehasinvarc_[relationIds[k]][p.first]) {
	  vector<Arc*>& arcs = inarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[k] &&
	       arcs[i] != outArcWithRelation) {
	      int newnodeid = arcs[i]->getTail()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		destIds.insert(newnodeid);
	    }
	  }
	}
      }
    }
  }

  return;
}

void Data::rightEntitiesUsingDFS(Arc* outArcWithRelation,
				 Rule& rule, 
				 set<int>& destIds, 
				 vector<int>& path)
{
  vector<int>& relationIds = rule.getRelationIds();
  vector<bool>& isReverseArc = rule.getIsReverseArc();

  int pathlength = ((int)path.size())-1;

  if (pathlength == rule.getLengthRule()) {
    int lastnodeid = path.back();
    destIds.insert(lastnodeid);
    return;
  }

  if(!isReverseArc[pathlength]) {
    if(relnodehasarc_[relationIds[pathlength]][path.back()]) {
      vector<Arc*>& arcs = outarcs_[path.back()];
      for (int i=0; i<(int)arcs.size(); i++) {
	if(arcs[i]->getIdRelation() == relationIds[pathlength] &&
	   arcs[i] != outArcWithRelation) {
	  int newnodeid = arcs[i]->getHead()->getId();
	  if (nodeIsNotInPath(path,newnodeid)) {
	    path.push_back(newnodeid);
	    rightEntitiesUsingDFS(outArcWithRelation, rule, destIds, path);
	    path.pop_back();
	  }
	}
      }
    }
  }
  else {
    if(relnodehasinvarc_[relationIds[pathlength]][path.back()]) {
      vector<Arc*>& arcs = inarcs_[path.back()];
      for (int i=0; i<(int)arcs.size(); i++) {
	if(arcs[i]->getIdRelation() == relationIds[pathlength] &&
	   arcs[i] != outArcWithRelation) {
	  int newnodeid = arcs[i]->getTail()->getId();
	  if (nodeIsNotInPath(path,newnodeid)) {
	    path.push_back(newnodeid);
	    rightEntitiesUsingDFS(outArcWithRelation, rule, destIds, path);
	    path.pop_back();
	  }
	}
      }
    }
  }

  return;
}

void Data::getRightEntities(Arc* outArcWithRelation, Rule& rule, 
			    int origId, set<int>& destIds,
			    bool useBFS)
{
  assert(rule.getLengthRule() >= 1);
  if(useBFS)
    rightEntitiesUsingBFS(outArcWithRelation, rule, origId, destIds);
  else {
    vector<int> path;
    path.push_back(origId);
    rightEntitiesUsingDFS(outArcWithRelation, rule, destIds, path);
  }
}

void Data::leftEntitiesUsingBFS(Arc* outArcWithRelation,
				Rule& rule, 
				int destId,
				set<int>& origIds)
{
  vector<int>& relationIds = rule.getRelationIds();
  vector<bool>& isReverseArc = rule.getIsReverseArc();
  int rulelength = rule.getLengthRule();

  vector<vector<pair<int,int> > > q(rulelength); // each position corresponds to a level in the search tree. The first int is the nodeId and the second int is the index of the previous node in the path
  q[0].push_back(pair<int,int>(destId,-1));

  for(int k=0; k<rulelength-1; k++) {
    int position = rulelength - k - 1;
    for(int l=0; l<(int)q[k].size(); l++) {
      pair<int,int>& p = q[k][l];
      if(!isReverseArc[position]) {
	if(relnodehasinvarc_[relationIds[position]][p.first]) {
	  vector<Arc*>& arcs = inarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[position] &&
	       arcs[i] != outArcWithRelation) {
	      int newnodeid = arcs[i]->getTail()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		q[k+1].push_back(pair<int,int>(newnodeid,l));
	    }
	  }
	}
      }
      else {
	if(relnodehasarc_[relationIds[position]][p.first]) {
	  vector<Arc*>& arcs = outarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[position] &&
	       arcs[i] != outArcWithRelation) {
	      int newnodeid = arcs[i]->getHead()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		q[k+1].push_back(pair<int,int>(newnodeid,l));
	    }
	  }
	}
      }
    }
  }

  {
    int k=rulelength-1;
    int position = rulelength - k - 1;
    for(int l=0; l<(int)q[k].size(); l++) {
      pair<int,int>& p = q[k][l];
      if(!isReverseArc[position]) {
	if(relnodehasinvarc_[relationIds[position]][p.first]) {
	  vector<Arc*>& arcs = inarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[position] &&
	       arcs[i] != outArcWithRelation) {
	      int newnodeid = arcs[i]->getTail()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		origIds.insert(newnodeid);
	    }
	  }
	}
      }
      else {
	if(relnodehasarc_[relationIds[position]][p.first]) {
	  vector<Arc*>& arcs = outarcs_[p.first];
	  for (int i=0; i<(int)arcs.size(); i++) {
	    if(arcs[i]->getIdRelation() == relationIds[position] &&
	       arcs[i] != outArcWithRelation) {
	      int newnodeid = arcs[i]->getHead()->getId();
	      if (nodeIsNotInPath(q,k,l,newnodeid))
		origIds.insert(newnodeid);
	    }
	  }
	}
      }
    }
  }

  return;
}

void Data::leftEntitiesUsingDFS(Arc* outArcWithRelation,
				Rule& rule, 
				set<int>& origIds, 
				vector<int>& path)
{
  vector<int>& relationIds = rule.getRelationIds();
  vector<bool>& isReverseArc = rule.getIsReverseArc();

  int pathlength = ((int)path.size())-1;

  if (pathlength == rule.getLengthRule()) {
    int lastnodeid = path.back();
    origIds.insert(lastnodeid);
    return;
  }

  int position = rule.getLengthRule() - pathlength - 1;
  if(!isReverseArc[position]) {
    if(relnodehasinvarc_[relationIds[position]][path.back()]) {
      vector<Arc*>& arcs = inarcs_[path.back()];
      for (int i=0; i<(int)arcs.size(); i++) {
	if(arcs[i]->getIdRelation() == relationIds[position] &&
	   arcs[i] != outArcWithRelation) {
	  int newnodeid = arcs[i]->getTail()->getId();
	  if (nodeIsNotInPath(path,newnodeid)) {
	    path.push_back(newnodeid);
	    leftEntitiesUsingDFS(outArcWithRelation, rule, origIds, path);
	    path.pop_back();
	  }
	}
      }
    }
  }
  else {
    if(relnodehasarc_[relationIds[position]][path.back()]) {
      vector<Arc*>& arcs = outarcs_[path.back()];
      for (int i=0; i<(int)arcs.size(); i++) {
	if(arcs[i]->getIdRelation() == relationIds[position] &&
	   arcs[i] != outArcWithRelation) {
	  int newnodeid = arcs[i]->getHead()->getId();
	  if (nodeIsNotInPath(path,newnodeid)) {
	    path.push_back(newnodeid);
	    leftEntitiesUsingDFS(outArcWithRelation, rule, origIds, path);
	    path.pop_back();
	  }
	}
      }
    }
  }

  return;
}

void Data::getLeftEntities(Arc* outArcWithRelation, Rule& rule, 
			   int destId, set<int>& origIds,
			   bool useBFS)
{
  assert(rule.getLengthRule() >= 1);
  if(useBFS)
    leftEntitiesUsingBFS(outArcWithRelation, rule, destId, origIds);
  else {
    vector<int> path;
    path.push_back(destId);
    leftEntitiesUsingDFS(outArcWithRelation, rule, origIds, path);
  }
}

void Data::createQueryFromTrainingData(Parameters& params)
{
  int relationId = params.getRelationId();

  for(int i=0; i<(int)arcs_.size(); i++) {
    Arc* arc = arcs_[i];
    if(arc->getIdRelation() == relationId) {
      int ent1 = arc->getTail()->getId();
      int ent2 = arc->getHead()->getId();
      query_.addEntityPair(ent1,ent2);
      query_.addArc(arc);
    }
  }

#if 0
  vector<pair<int,int> >& pairs = query_.getEntityPairs();
  cout<<"query:"<<endl;
  for(int i=0; i<(int)pairs.size(); i++)
    cout<<entities_[pairs[i].first]<<" "<<entities_[pairs[i].second]<<endl;
#endif
}

void Data::createQueryFromTrainingData(int relationId)
{
  queries_[relationId].resetQuery();

  int nrelations = (int)relations_.size();
  if(relationId >= nrelations) {
    // this is the case of a reverse arc
    relationId = relationId - nrelations;
    assert(relationId >= 0 && relationId < nrelations);
    for(int i=0; i<(int)arcs_.size(); i++) {
      Arc* arc = arcs_[i];
      if(arc->getIdRelation() == relationId) {
	int ent1 = arc->getTail()->getId();
	int ent2 = arc->getHead()->getId();
	queries_[relationId].addEntityPair(ent2,ent1); // reverse the nodes
	queries_[relationId].addArc(arc);
      }
    }
  }
  else {
    for(int i=0; i<(int)arcs_.size(); i++) {
      Arc* arc = arcs_[i];
      if(arc->getIdRelation() == relationId) {
	int ent1 = arc->getTail()->getId();
	int ent2 = arc->getHead()->getId();
	queries_[relationId].addEntityPair(ent1,ent2);
	queries_[relationId].addArc(arc);
      }
    }
  }

#if 0
  vector<pair<int,int> >& pairs = queries_[relationId].getEntityPairs();
  cout<<"query:"<<endl;
  for(int i=0; i<(int)pairs.size(); i++)
    cout<<entities_[pairs[i].first]<<" "<<entities_[pairs[i].second]<<endl;
#endif
}
