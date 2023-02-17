// © Copyright IBM Corporation 2022. All Rights Reserved.
// LICENSE: Eclipse Public License - v 2.0, https://opensource.org/licenses/EPL-2.0
// SPDX-License-Identifier: EPL-2.0

#include "Solver.hpp"

//#include <iostream>
#include <iomanip>
#include <set>

#define DEBUGR 0
#define GLOBALMAXPATHLEN 30
using namespace std;

// assume all distance values are -1
int Solver::find_sp(int snode, int enode, int relid, Arc* ar, vector<int> & distance, vector<int> & prev, vector<int> &prevrel, set<int> & tnodes, Rule & r, vector<int> &firstrel, vector<int> &firstinvrel)
{
  int maxRuleLength = params_.getMaxRuleLength();
  bool useRelationInRules = params_.getUseRelationInRules();
  bool useReverseArcs = params_.getUseReverseArcsInRules();
  vector<int>stack;
  int spos = 0, epos = 0, curdist = 0;
  int fdist = -1;
  
  distance[snode] = 0;
  stack.push_back(snode);
  tnodes.insert(snode);
  
  while (distance[enode] < 0 && curdist < maxRuleLength && spos <= epos){
    int cnode = stack[spos];
    spos ++;
    if (distance[cnode] > curdist) curdist = distance[cnode];
    
    vector<Arc*> &outarcs = data_.getOutArcs()[cnode];
    for (int j=0; j<outarcs.size(); j++){
      if (ar != NULL && outarcs[j] == ar) continue;
      int nextnode = outarcs[j]->getHead()->getId();
      int nextrel = outarcs[j]->getIdRelation();
      if (useRelationInRules==false && relid == nextrel) continue;
      if (cnode == snode && firstrel[nextrel] == 0) continue;
      if (tnodes.find(nextnode) != tnodes.end()) continue;
      
      stack.push_back(nextnode);
      distance[nextnode] = curdist + 1;
      prevrel[nextnode] = nextrel;
      prev[nextnode] = cnode;
      tnodes.insert(nextnode);
      if (nextnode == enode) break;
      epos++;
    }

    if (distance[enode] >= 0) break;
    if (!useReverseArcs) continue;
    
    vector<Arc*> &inarcs = data_.getInArcs()[cnode];
    for (int j=0; j<inarcs.size(); j++){
      if (ar != NULL && inarcs[j] == ar) continue;
      int nextnode = inarcs[j]->getTail()->getId();
      int nextrel = inarcs[j]->getIdRelation();
      if (useRelationInRules==false && relid == nextrel) continue;
      if (tnodes.find(nextnode) != tnodes.end()) continue;
      if (cnode == snode && firstinvrel[nextrel] == 0) continue;
      
      stack.push_back(nextnode);
      distance[nextnode] = curdist + 1;
      prevrel[nextnode] = -(nextrel+1);
      prev[nextnode] = cnode;
      tnodes.insert(nextnode);
      if (nextnode == enode) break;
      epos++;
    }
  }
  //printf ("distance[enode] = %d, curdist = %d\n", distance[enode], curdist + 1);
  fdist = distance[enode];
  
  if (distance[enode] >= 0){
    int cnode = enode;
    vector<int> rels;
    vector<bool> reldir;
    rels.resize(curdist + 1);
    reldir.resize(curdist + 1);

    int tdist = curdist;
    while (cnode != snode && tdist >= 0){
      int rel;
      bool tf;
      if (prevrel[cnode] < 0){
	rel =  -prevrel[cnode] - 1;
	tf = true;
      }
      else{
	rel =  prevrel[cnode];
	tf = false;
      }
      rels[tdist] = rel;
      reldir[tdist] = tf;
      tdist --; 
      cnode = prev[cnode];
    }
    for (int i=0; i<=curdist; i++)
      r.addRelationId(rels[i], reldir[i]);
  }
  for (set<int>::iterator it = tnodes.begin(); it != tnodes.end(); it++){
    distance[*it] = -1;
    prevrel[*it] = -1;
    prev[*it] = -1;
  }
  tnodes.clear();
  return fdist;
}

// assume all distance values are -1
int Solver::find_sp2(int snode, int enode, int truedist, int relid, Arc* ar, vector<int> & distance, vector<int> & prev, vector<int> &prevrel, set<int> & tnodes, Rule & r, vector<int> &firstrel, vector<int> &firstinvrel)
{
  int maxRuleLength = params_.getMaxRuleLength();
  bool useRelationInRules = params_.getUseRelationInRules();
  bool useReverseArcs = params_.getUseReverseArcsInRules();
  vector<int>stack;
  int spos = 0, epos = 0, curdist = 0;
  int fdist = -1;

  if (truedist < 0) return truedist;
  
  distance[snode] = 0;
  stack.push_back(snode);
  tnodes.insert(snode);
  
  while (distance[enode] < 0 && curdist < maxRuleLength && spos <= epos){
    int cnode = stack[spos];
    spos ++;
    if (distance[cnode] > curdist) curdist = distance[cnode];
    
    vector<Arc*> &outarcs = data_.getOutArcs()[cnode];
    for (int j=0; j<outarcs.size(); j++){
      if (ar != NULL && outarcs[j] == ar) continue;
      int nextnode = outarcs[j]->getHead()->getId();
      int nextrel = outarcs[j]->getIdRelation();
      if (useRelationInRules==false && relid == nextrel) continue;
      if (cnode == snode && firstrel[nextrel] == 0) continue;
      if (tnodes.find(nextnode) != tnodes.end()) continue;

      if (nextnode == enode && curdist < truedist) continue;
      stack.push_back(nextnode);
      distance[nextnode] = curdist + 1;
      prevrel[nextnode] = nextrel;
      prev[nextnode] = cnode;
      tnodes.insert(nextnode);
      if (nextnode == enode) break;
      epos++;
    }

    if (distance[enode] >= 0) break;
    if (!useReverseArcs) continue;
    
    vector<Arc*> &inarcs = data_.getInArcs()[cnode];
    for (int j=0; j<inarcs.size(); j++){
      if (ar != NULL && inarcs[j] == ar) continue;
      int nextnode = inarcs[j]->getTail()->getId();
      int nextrel = inarcs[j]->getIdRelation();
      if (useRelationInRules==false && relid == nextrel) continue;
      if (tnodes.find(nextnode) != tnodes.end()) continue;
      if (cnode == snode && firstinvrel[nextrel] == 0) continue;

      if (nextnode == enode && curdist < truedist) continue;
      stack.push_back(nextnode);
      distance[nextnode] = curdist + 1;
      prevrel[nextnode] = -(nextrel+1);
      prev[nextnode] = cnode;
      tnodes.insert(nextnode);
      if (nextnode == enode) break;
      epos++;
    }
  }
  //printf ("distance[enode] = %d, curdist = %d\n", distance[enode], curdist + 1);
  fdist = distance[enode];
  
  if (distance[enode] >= 0){
    int cnode = enode;
    vector<int> rels;
    vector<bool> reldir;
    rels.resize(curdist + 1);
    reldir.resize(curdist + 1);

    int tdist = curdist;
    while (cnode != snode && tdist >= 0){
      int rel;
      bool tf;
      if (prevrel[cnode] < 0){
	rel =  -prevrel[cnode] - 1;
	tf = true;
      }
      else{
	rel =  prevrel[cnode];
	tf = false;
      }
      rels[tdist] = rel;
      reldir[tdist] = tf;
      tdist --; 
      cnode = prev[cnode];
    }
    for (int i=0; i<=curdist; i++)
      r.addRelationId(rels[i], reldir[i]);
  }
  for (set<int>::iterator it = tnodes.begin(); it != tnodes.end(); it++){
    distance[*it] = -1;
    prevrel[*it] = -1;
    prev[*it] = -1;
  }
  tnodes.clear();
  return fdist;
}

void Solver::generateRulesS0(int relationId, vector<Rule>& rules)
{
  vector<string>& relations = data_.getRelations();
  int nrelations = (int) relations.size();
  //  int relationId = params_.getRelationId();
  int maxRuleLength = params_.getMaxRuleLength();
  bool useRelationInRules = params_.getUseRelationInRules();
  bool useRulesOfLengthOne = params_.getUseRulesOfLengthOne();
  bool useReverseArcs = params_.getUseReverseArcsInRules();
  int numpairs = data_.getQuery(relationId).getNumEntityPairs();
  //  int numpairs = data_.getQuery().getNumEntityPairs();
  int minpairs = (numpairs > 100) ? numpairs / 20 : numpairs / 2;
  set<Rule> allrules;
  int mindist=100, maxdist=0;
  int prune_rules = 0;

  vector<int> prev, prevrel, distance;
  distance.resize(data_.getNumberNodes(), -1);
  prev.resize(data_.getNumberNodes(), -1);
  prevrel.resize(data_.getNumberNodes(), -1);
  set<int> tnodes;
  int nr= 0, ng2=0, na2=0;
  int pathlen[GLOBALMAXPATHLEN];
 
  vector<int> firstrel;
  vector<int> firstinvrel;
  vector<int> lastrel;
  vector<int> lastinvrel;
  firstrel.resize(nrelations, 1);
  firstinvrel.resize(nrelations, 1);
  lastrel.resize(nrelations, 1);
  lastinvrel.resize(nrelations, 1);

  if (prune_rules){
    calculateFirstLast(relationId, firstrel, firstinvrel, lastrel, lastinvrel);
    for (int j=0; j<nrelations; j++){
      if (firstrel[j] > 0) printf("[%d, %d] ", j, firstrel[j]);
      if (firstinvrel[j] > 0) printf("[R%d, %d] ", j, firstinvrel[j]);
      if (lastrel[j] > 0) printf("[%d, %d] ", j, lastrel[j]);
      if (lastinvrel[j] > 0) printf("[R%d, %d] ", j, lastinvrel[j]);
    }
    printf("\n");
  }
  
  for (int i=0; i<maxRuleLength; i++) pathlen[i] = 0;
  
  assert(rules.size() == 0);
  assert(maxRuleLength > 0);

  //  for (int i=0; i<data_.getQuery().getNumEntityPairs(); i++){
  //    int node1 = data_.getQuery().getEntityPairs()[i].first;
  //    int node2 = data_.getQuery().getEntityPairs()[i].second;
  //    Arc* ar =  data_.getQuery().getOutArcsWithRelation()[i];
  for (int i=0; i<data_.getQuery(relationId).getNumEntityPairs(); i++){
    int node1 = data_.getQuery(relationId).getEntityPairs()[i].first;
    int node2 = data_.getQuery(relationId).getEntityPairs()[i].second;
    Arc* ar =  data_.getQuery(relationId).getOutArcsWithRelation()[i];

    Rule r;
    int dist= 0;

    /* TODO: clean up next line */
    for (int k=0; k<data_.getNumberNodes(); k++) distance[k] = -1;
    dist = find_sp(node1, node2, relationId, ar, distance, prev, prevrel, tnodes, r, firstrel, firstinvrel);
      
    if (dist > 0){
      int found = 0;
      if (dist < mindist) mindist = dist;
      if (dist > maxdist) maxdist = dist;
      pathlen[dist] ++;


      for (int j=0; j<rules.size(); j++)
	if (rules[j] == r){
	  found = 1;
	  break;
	}
      if (found == 0)
	rules.push_back(r);
      nr ++;

     /* new stuff */

      Rule r2;
      int dist2;
      ng2 = 0;
      na2 = 0;
      dist2 = find_sp2(node1, node2, dist, relationId, ar, distance, prev, prevrel, tnodes, r2, firstrel, firstinvrel);
      //printf("dist = %d, dist2 = %d\n", dist, dist2);
      if (dist2 > 0){
	found = 0;

	ng2 ++;
	for (int j=0; j<rules.size(); j++)
	  if (rules[j] == r2){
	    found = 1;
	    break;
	  }
	if (found == 0){
	  rules.push_back(r2);
	  na2 ++;
	}
      }

      //      if (rules.size() > 9) break;
    }
  } // end for each query pair

  printf ("generated %d (%d, %d) rules for %d pairs, mindist = %d, maxdist = %d\n", (int)(rules.size()), nr, ng2, na2, mindist, maxdist); fflush(stdout);
  int maxcomplx = 0;
  for (int i=1, tsum=0; i<maxRuleLength; i++){
    printf ("p[%d] %d, ", i, pathlen[i]);
    tsum += pathlen[i];
    if (tsum > (nr*9/10) /*(nr * 6)/7*/  && maxcomplx == 0){
        setMaxComplexity(relationId, i+1);
	maxcomplx = i+1;
	// maxcomplx = i+2; /* HACK */
	printf("\n numpairs = %d, nr = %d, tsum = %d, Set max complexity = %d\n", numpairs, nr, tsum, maxcomplx);
      }
  }
  printf("\n"); fflush(stdout);
  /*
  if (allrules.size() > 0){
    for (set<Rule>::iterator it = allrules.begin(); it != allrules.end(); it++)
      rules.push_back(*it);
  }
  */
}

void Solver::calculateFirstLast(int relationId, vector<int>& firstrel, vector<int>& firstinvrel, vector<int>& lastrel, vector<int>& lastinvrel)
{
  vector<string>& relations = data_.getRelations();
  int nrelations = (int) relations.size();
  bool useRelationInRules = params_.getUseRelationInRules();
  bool useReverseArcs = params_.getUseReverseArcsInRules();
  int numpairs = data_.getQuery(relationId).getNumEntityPairs();
  //  int numpairs = data_.getQuery().getNumEntityPairs();
  int minpairs = numpairs > 100 ? numpairs / 20 : nrelations / 2;
  
  for (int i=0; i<nrelations; i++){
    firstrel[i] = 0;
    firstinvrel[i] = 0;
    lastrel[i] = 0;
    lastinvrel[i] = 0;
  }

  //  for (int i=0; i<data_.getQuery().getNumEntityPairs(); i++){
  //    int node1 = data_.getQuery().getEntityPairs()[i].first;
  //    int node2 = data_.getQuery().getEntityPairs()[i].second;
  for (int i=0; i<data_.getQuery(relationId).getNumEntityPairs(); i++){
    int node1 = data_.getQuery(relationId).getEntityPairs()[i].first;
    int node2 = data_.getQuery(relationId).getEntityPairs()[i].second;
    set<int> relset;
    set<int> invrelset;
    set<int> lastrelset;
    set<int> lastinvrelset;
    
    for (int j=0; j<data_.getOutArcs()[node1].size(); j++){
      int rel = data_.getOutArcs()[node1][j]->getIdRelation(); 

      if (rel == relationId && useRelationInRules == false)
	continue;
      
      if (relset.find(rel) == relset.end()){
	relset.insert(rel);
	firstrel[rel] ++;
      }
    }
    // end arcs
    for (int j=0; j<data_.getInArcs()[node2].size(); j++){
      int rel = data_.getInArcs()[node2][j]->getIdRelation(); 

      if (rel == relationId && useRelationInRules == false)
	continue;
      
      if (lastrelset.find(rel) == lastrelset.end()){
	lastrelset.insert(rel);
	lastrel[rel] ++;
      }
    }
    
    if (useReverseArcs){
      for (int j=0; j<data_.getInArcs()[node1].size(); j++){
	int rel = data_.getInArcs()[node1][j]->getIdRelation(); 
	
	if (rel == relationId && useRelationInRules == false)
	  continue;

	if (invrelset.find(rel) == invrelset.end()){
	  invrelset.insert(rel);
	  firstinvrel[rel] ++;
	}
      }

      // end arcs
      for (int j=0; j<data_.getOutArcs()[node2].size(); j++){
	int rel = data_.getOutArcs()[node2][j]->getIdRelation(); 
	
	if (rel == relationId && useRelationInRules == false)
	  continue;
      
	if (lastinvrelset.find(rel) == lastinvrelset.end()){
	  lastinvrelset.insert(rel);
	  lastinvrel[rel] ++;
	}
      }

    }
  }
  int nfrel = 0, nifrel = 0;
  int nlfrel = 0, nlifrel = 0;
  
  for (int j=0; j<nrelations; j++){
    if (DEBUGR > 0) if (firstrel[j] > 0) printf("[%d, %d] ", j, firstrel[j]);
    if (firstrel[j] < minpairs) firstrel[j] = 0;
    if (firstrel[j] > 0) nfrel ++;
    if (DEBUGR > 0) if (firstinvrel[j] > 0) printf("[R%d, %d] ", j, firstinvrel[j]);
    if (firstinvrel[j] < minpairs) firstinvrel[j] = 0;
    if (firstinvrel[j] > 0) nifrel ++;

    if (DEBUGR > 0) if (lastrel[j] > 0) printf("[%d, %d] ", j, lastrel[j]);
    if (lastrel[j] < minpairs) lastrel[j] = 0;
    if (lastrel[j] > 0) nfrel ++;
    if (DEBUGR > 0) if (lastinvrel[j] > 0) printf("[R%d, %d] ", j, lastinvrel[j]);
    if (lastinvrel[j] < minpairs) lastinvrel[j] = 0;
    if (lastinvrel[j] > 0) nifrel ++;

  }
  printf("\n");
  printf ("nfrel = %d, nifrel = %d\n", nfrel, nifrel);
  return;
}

void Solver::generateRulesS1(int relationId, vector<Rule>& rules)
{
  vector<string>& relations = data_.getRelations();
  int nrelations = (int) relations.size();
  //  int relationId = params_.getRelationId();
  int maxRuleLength = params_.getMaxRuleLength();
  bool useRelationInRules = params_.getUseRelationInRules();
  bool useRulesOfLengthOne = params_.getUseRulesOfLengthOne();
  bool useReverseArcs = params_.getUseReverseArcsInRules();
  int numpairs = data_.getQuery(relationId).getNumEntityPairs();
  //  int numpairs = data_.getQuery().getNumEntityPairs();
  int minpairs = numpairs > 100 ? numpairs / 20 : nrelations / 2;
  
  assert(rules.size() == 0);
  assert(maxRuleLength > 0);

  vector<int> active;
  vector<int> nchild;
  vector<int> isactive;
  int nproc = 0;
  isactive.resize(data_.getEntities().size());

  vector<int> firstrel;
  vector<int> firstinvrel;
  vector<int> lastrel;
  vector<int> lastinvrel;
  firstrel.resize(nrelations, 0);
  firstinvrel.resize(nrelations, 0);
  lastrel.resize(nrelations, 0);
  lastinvrel.resize(nrelations, 0);

  calculateFirstLast(relationId, firstrel, firstinvrel, lastrel, lastinvrel);
  for (int j=0; j<nrelations; j++){
    if (firstrel[j] > 0) printf("[%d, %d] ", j, firstrel[j]);
    if (firstinvrel[j] > 0) printf("[R%d, %d] ", j, firstinvrel[j]);
    if (lastrel[j] > 0) printf("[%d, %d] ", j, firstrel[j]);
    if (lastinvrel[j] > 0) printf("[R%d, %d] ", j, firstinvrel[j]);
  }
  printf("\n");

  for (int i=0; i<data_.getQuery(relationId).getNumEntityPairs(); i++){
    int node1 = data_.getQuery(relationId).getEntityPairs()[i].first;
    int node2 = data_.getQuery(relationId).getEntityPairs()[i].second;
    //  for (int i=0; i<data_.getQuery().getNumEntityPairs(); i++){
    //    int node1 = data_.getQuery().getEntityPairs()[i].first;
    //    int node2 = data_.getQuery().getEntityPairs()[i].second;

    active.push_back(node1);
    nchild.push_back(0);
    isactive[node1] = 1;

    while (active.size() > 0){
      int cnode = active[active.size()-1];
      int nchildproc = nchild[nchild.size()-1];
      int enode=-1;
      int tdeg = data_.getOutArcs()[cnode].size();
      if (useReverseArcs) tdeg += data_.getInArcs()[cnode].size();

      //printf ("active size = %d, cnode=%d, tdeg=%d, nchildproc = %d, odeg=%d, ideg=%d,\n", (int)active.size(), cnode, tdeg, nchildproc, data_.getOutArcs()[cnode].size(), data_.getInArcs()[cnode].size());
      if (nchildproc == tdeg){
	isactive[cnode] = 0;
	active.pop_back();
	nchild.pop_back();
	if (nchild.size() > 0)
	  nchild[nchild.size()-1] = nchild[nchild.size()-1] + 1;
      }
      else{
	int erel=-1;
	if (nchildproc < data_.getOutArcs()[cnode].size()){
	  int j = nchildproc;
	  enode = data_.getOutArcs()[cnode][j]->getHead()->getId();
	  erel = data_.getOutArcs()[cnode][j]->getIdRelation();
	}
	else{
	  int j = nchildproc - data_.getOutArcs()[cnode].size();
	  enode = data_.getInArcs()[cnode][j]->getTail()->getId();
	  erel = data_.getInArcs()[cnode][j]->getIdRelation();
	}
	if ( erel == relationId && !useRelationInRules){
	  nchild[nchild.size()-1] ++;
	  nproc ++;
	  continue;
	}
	/*
	if (active.size() == 1){
	  if ((nchildproc < data_.getOutArcs()[cnode].size() && firstrel[erel] == 0) ||
	      (nchildproc >= data_.getOutArcs()[cnode].size() && firstinvrel[erel] == 0)){
	    nchild[nchild.size()-1] ++;
	    nproc ++;
	    continue;
	  }
	}
	*/
	if (enode == node2){
	  // Found node2, trace back to root
	  Rule rule;
	  nproc ++;
	  
	  for (int k=0; k<nchild.size(); k++){
	    int j=0, rel=0;
	    bool revdir=false;
	    int anode = active[k];
	    
	    if (nchild[k] < data_.getOutArcs()[anode].size()){
	      j = nchild[k];
	      rel = data_.getOutArcs()[anode][j]->getIdRelation();
	      revdir = false;
	    }
	    else{
	      j = nchild[k] - data_.getOutArcs()[anode].size();
	      rel = data_.getInArcs()[anode][j]->getIdRelation(); 
	      revdir = true;
	    }
	    rule.addRelationId(rel, revdir);
	  }
	  nchild[nchild.size()-1] ++;

	  if(useRulesOfLengthOne && rule.getLengthRule() == 1) {
	    bool useRelation = params_.getUseRelationInLengthOneRule();
	    vector<int> & relids = rule.getRelationIds();
	    vector<bool> & getIsReverseArc();
	    
	    if(relids[0] == relationId && !useRelation)
	      continue; // do not add rule since it has relationId
	  }
	  bool found_rule = false;
	  for (int l=0; l<rules.size(); l++)
	    if  (rules[l] == rule){ found_rule = true; break;}
	  if (found_rule == false)
	    rules.push_back(rule);
	}
	else if (isactive[enode] == 0 && active.size() < maxRuleLength){
	  active.push_back(enode);
	  nchild.push_back(0);
	  isactive[enode] = 1;
	}
	else{
	  nchild[nchild.size()-1] ++;
	  nproc ++;
	}
      } // end else more children to be explored
    } // end while active list is empty
    // printf ("finished query pair\n");
  } // end for each query pair

  printf ("evaluated %d paths\n", nproc); fflush(stdout);
}

void Solver::generateRulesS2(int relationId, vector<Rule>& rules)
{
  vector<string>& relations = data_.getRelations();
  int nrelations = (int) relations.size();
  //  int relationId = params_.getRelationId();
  int maxRuleLength = params_.getMaxRuleLength();
  bool useRelationInRules = params_.getUseRelationInRules();
  bool useRulesOfLengthOne = params_.getUseRulesOfLengthOne();
  bool useReverseArcs = params_.getUseReverseArcsInRules();

  assert(rules.size() == 0);
  assert(maxRuleLength > 0);

  // generate rules of length greater than one
  // calculate all end points of relations incident to starting nodes.

  vector<set<int> > secondrel;
  vector<int> endrel;
  vector<int> lengthone;
  vector<set<int> > lengthtwo; // relations are stored as consecutive numbers
  secondrel.resize(nrelations);
  endrel.resize(nrelations);
  lengthone.resize(nrelations);
  lengthtwo.resize(nrelations);
  
  int rcnt1 = 0, rcnt2 = 0, rcnt3 = 0;
  int tcnt1 = 0, tcnt2 = 0, tcnt3 = 0;

  vector<set<int> > secondrelinv;
  vector<int> endrelinv;
  vector<int> lengthoneinv;
  vector<set<int> > lengthtwoinv; // relations are stored as consecutive numbers
  if (useReverseArcs){
    secondrelinv.resize(nrelations);
    endrelinv.resize(nrelations);
    lengthoneinv.resize(nrelations);
    lengthtwoinv.resize(nrelations);
  }
  for (int i=0; i<data_.getQuery(relationId).getNumEntityPairs(); i++){
    int node1 = data_.getQuery(relationId).getEntityPairs()[i].first;
    int node2 = data_.getQuery(relationId).getEntityPairs()[i].second;
    //  for (int i=0; i<data_.getQuery().getNumEntityPairs(); i++){
    //    int node1 = data_.getQuery().getEntityPairs()[i].first;
    //    int node2 = data_.getQuery().getEntityPairs()[i].second;
    
    for (int j=0; j<data_.getOutArcs()[node1].size(); j++){
      int rel = data_.getOutArcs()[node1][j]->getIdRelation(); 
      int enode = data_.getOutArcs()[node1][j]->getHead()->getId();

      if (rel == relationId) continue;
      if (enode == node2)
	lengthone[rel]++;
      
      for (int k=0; k<data_.getOutArcs()[enode].size(); k++){
	int rel2 = data_.getOutArcs()[enode][k]->getIdRelation();
	int enode2 = data_.getOutArcs()[enode][k]->getHead()->getId();
	if (rel2 == relationId) continue;
	if (enode2 == node2){
	  lengthtwo[rel].insert(rel2);
	}
	secondrel[rel].insert(rel2);
      }

      // reverse second arcs
      for (int k=0; k<data_.getInArcs()[enode].size(); k++){
	int rel2 = data_.getInArcs()[enode][k]->getIdRelation();
	int enode2 = data_.getInArcs()[enode][k]->getTail()->getId();
	if (rel2 == relationId) continue;
	if (enode2 == node2){
	  lengthtwo[rel].insert(-(rel2+1));
	}
	secondrel[rel].insert(-(rel2+1));
      }
    }
    for (int j=0; j<data_.getInArcs()[node2].size(); j++){
      int rel = data_.getInArcs()[node2][j]->getIdRelation();
      if (rel == relationId) continue;
      endrel[rel] ++;
    }

    // now use reverse first arcs
    if (useReverseArcs){
      for (int j=0; j<data_.getInArcs()[node1].size(); j++){
	int rel = data_.getInArcs()[node1][j]->getIdRelation(); 
	int enode = data_.getInArcs()[node1][j]->getTail()->getId();

	if (rel == relationId) continue;
	if (enode == node2)
	  lengthoneinv[rel]++;
      
	for (int k=0; k<data_.getOutArcs()[enode].size(); k++){
	  int rel2 = data_.getOutArcs()[enode][k]->getIdRelation();
	  int enode2 = data_.getOutArcs()[enode][k]->getHead()->getId();
	  if (rel2 == relationId) continue;
	  if (enode2 == node2){
	    lengthtwoinv[rel].insert(rel2);
	  }
	  secondrelinv[rel].insert(rel2);
	}

	// reverse second arcs
	for (int k=0; k<data_.getInArcs()[enode].size(); k++){
	  int rel2 = data_.getInArcs()[enode][k]->getIdRelation();
	  int enode2 = data_.getInArcs()[enode][k]->getTail()->getId();
	  if (rel2 == relationId) continue;
	  if (enode2 == node2){
	    lengthtwoinv[rel].insert(-(rel2+1));
	  }
	  secondrelinv[rel].insert(-(rel2+1));
	}
      }
      for (int j=0; j<data_.getOutArcs()[node2].size(); j++){
	int rel = data_.getOutArcs()[node2][j]->getIdRelation();
	if (rel == relationId) continue;
	endrelinv[rel] ++;
      }
    } // end useReverseArcs
  }
  
  if(useRulesOfLengthOne) {
    bool useRelation = params_.getUseRelationInLengthOneRule();
    int nr = 0, ninvr = 0;
    
    for(int i=0; i<nrelations; i++) {
      if(lengthone[i] > 0 && (i != relationId || useRelation == true)){
	Rule rule;
	rule.addRelationId(i);
	rules.push_back(rule);
	nr ++;
      }
      if (useReverseArcs && lengthoneinv[i] > 0 && (i != relationId || useRelation == true)){
	Rule rule;
	rule.addRelationId(i, true);
	rules.push_back(rule);
	ninvr ++;
      }      
    }
    printf ("added %d rules of length 1, nr=%d, ninvr=%d\n", (int)rules.size(), nr, ninvr);
  }

  for (int i=0; i<nrelations; i++){
    if (lengthone[i] > 0) rcnt1 ++;  tcnt1 += lengthone[i];
    if (endrel[i] > 0) rcnt2 ++; tcnt2 += endrel[i];
    if (secondrel[i].size() > 0) rcnt3 ++; tcnt3 += secondrel[i].size();
  }
  
  printf("rcnt1 = %d, rcnt2 = %d, rcnt3 = %d, nrelations = %d \n", rcnt1, rcnt2, rcnt3, nrelations);
  printf("tcnt1 = %d, tcnt2 = %d, tcnt3 = %d, nrelations = %d \n", tcnt1, tcnt2, tcnt3, nrelations);
  /*
  for (int i=0; i<nrelations; i++)
    for (int j=0; j<nrelations; j++)
      if (startrel[i] > 0 && endrel[j] > 0 && secondrel[j] > 0){
	  Rule rule;
	  rule.addRelationId(i);
	  rule.addRelationId(j);
	  assert(rule.getLengthRule() == 2);
	  rules.push_back(rule);
      }
  */
  int sizeone = rules.size();
  for (int i=0; i<nrelations; i++){

    for (set<int>::iterator it = lengthtwo[i].begin(); it != lengthtwo[i].end(); it++){
      int rel2 = *it;
      if (rel2 < 0 && useReverseArcs){
	rel2 = -rel2 -1;

	Rule rule;
	rule.addRelationId(i);
	rule.addRelationId(rel2, true);
	rules.push_back(rule);
      }
      else{
	Rule rule;
	rule.addRelationId(i);
	rule.addRelationId(rel2);
	rules.push_back(rule);
      }
    }

    if (useReverseArcs){
      for (set<int>::iterator it = lengthtwoinv[i].begin(); it != lengthtwoinv[i].end(); it++){
	int rel2 = *it;
	if (rel2 < 0){
	  rel2 = -rel2 -1;

	  Rule rule;
	  rule.addRelationId(i, true);
	  rule.addRelationId(rel2, true);
	  rules.push_back(rule);
	}
	else{
	  Rule rule;
	  rule.addRelationId(i, true);
	  rule.addRelationId(rel2);
	  rules.push_back(rule);
	}
      }
    }
  }
  printf ("size 2 rules = %d\n", (int)rules.size() - sizeone);
  return;
}

void Solver::generateRulesS3(int relationId, vector<Rule>& rules)
{
  vector<string>& relations = data_.getRelations();
  int nrelations = (int) relations.size();
  //  int relationId = params_.getRelationId();
  int maxRuleLength = params_.getMaxRuleLength();
  bool useRelationInRules = params_.getUseRelationInRules();
  bool useRulesOfLengthOne = params_.getUseRulesOfLengthOne();
  bool useReverseArcs = params_.getUseReverseArcsInRules();
  int numpairs = data_.getQuery(relationId).getNumEntityPairs();
  //  int numpairs = data_.getQuery().getNumEntityPairs();
  int minpairs = numpairs > 100 ? numpairs / 20 : nrelations / 2;
  
  assert(rules.size() == 0);
  assert(maxRuleLength > 0);

  vector<int> active;
  vector<int> nchild;
  vector<int> isactive;
  int nproc = 0;
  isactive.resize(data_.getEntities().size());

  vector<int> firstrel;
  vector<int> firstinvrel;
  vector<int> lastrel;
  vector<int> lastinvrel;
  firstrel.resize(nrelations, 0);
  firstinvrel.resize(nrelations, 0);
  lastrel.resize(nrelations, 0);
  lastinvrel.resize(nrelations, 0);

  calculateFirstLast(relationId, firstrel, firstinvrel, lastrel, lastinvrel);
  for (int j=0; j<nrelations; j++){
    if (firstrel[j] > 0) printf("[%d, %d] ", j, firstrel[j]);
    if (firstinvrel[j] > 0) printf("[R%d, %d] ", j, firstinvrel[j]);
    if (lastrel[j] > 0) printf("[%d, %d] ", j, firstrel[j]);
    if (lastinvrel[j] > 0) printf("[R%d, %d] ", j, firstinvrel[j]);
  }
  printf("\n");

  for (int i=0; i<nrelations; i++){

    if (useRulesOfLengthOne){
      Rule rule;

      if (firstrel[i] && lastrel[i]){
	rule.addRelationId(i);
	rules.push_back(rule);
      }
      if (firstinvrel[i] && lastinvrel[i] && useReverseArcs){
	rule.addRelationId(i, true);
	rules.push_back(rule);
      }
    }

    if (firstrel[i]){
      for (int j=0; j<nrelations; j++){
	if (lastrel[j]){
	  Rule rule;
	  rule.addRelationId(i);
	  rule.addRelationId(j);
	  rules.push_back(rule);
	}

	if (lastinvrel[j]  && useReverseArcs){
	  Rule rule;
	  rule.addRelationId(i);
	  rule.addRelationId(j, true);
	  rules.push_back(rule);
	}
      }
    }

    if (firstinvrel[i] && useReverseArcs){
      for (int j=0; j<nrelations; j++){
	if (lastrel[j]){
	  Rule rule;
	  rule.addRelationId(i, true);
	  rule.addRelationId(j);
	  rules.push_back(rule);
	}

	if (lastinvrel[j]){
	  Rule rule;
	  rule.addRelationId(i, true);
	  rule.addRelationId(j, true);
	  rules.push_back(rule);
	}
      }
    }
  }
	
	
}

// assume all distance values are -1
int Solver::find_sp_1(int snode, int enode, int relid, Arc* ar, vector<int> & distance, vector<int> & prev, vector<int> &prevrel, set<int> & tnodes, Rule & r, vector<int> &firstrel, vector<int> &firstinvrel, int maxRuleLength)
{
  //  int maxRuleLength = params_.getMaxRuleLength();
  bool useRelationInRules = params_.getUseRelationInRules();
  bool useReverseArcs = params_.getUseReverseArcsInRules();
  vector<int>stack;
  int spos = 0, epos = 0, curdist = 0;
  int fdist = -1;
  
  distance[snode] = 0;
  stack.push_back(snode);
  tnodes.insert(snode);
  
  while (distance[enode] < 0 && curdist < maxRuleLength && spos <= epos){
    int cnode = stack[spos];
    spos ++;
    if (distance[cnode] > curdist) curdist = distance[cnode];
    
    vector<Arc*> &outarcs = data_.getOutArcs()[cnode];
    for (int j=0; j<outarcs.size(); j++){
      if (ar != NULL && outarcs[j] == ar) continue;
      int nextnode = outarcs[j]->getHead()->getId();
      int nextrel = outarcs[j]->getIdRelation();
      if (useRelationInRules==false && relid == nextrel) continue;
      if (cnode == snode && firstrel[nextrel] == 0) continue;
      if (tnodes.find(nextnode) != tnodes.end()) continue;
      
      stack.push_back(nextnode);
      distance[nextnode] = curdist + 1;
      prevrel[nextnode] = nextrel;
      prev[nextnode] = cnode;
      tnodes.insert(nextnode);
      if (nextnode == enode) break;
      epos++;
    }

    if (distance[enode] >= 0) break;
    if (!useReverseArcs) continue;
    
    vector<Arc*> &inarcs = data_.getInArcs()[cnode];
    for (int j=0; j<inarcs.size(); j++){
      if (ar != NULL && inarcs[j] == ar) continue;
      int nextnode = inarcs[j]->getTail()->getId();
      int nextrel = inarcs[j]->getIdRelation();
      if (useRelationInRules==false && relid == nextrel) continue;
      if (tnodes.find(nextnode) != tnodes.end()) continue;
      if (cnode == snode && firstinvrel[nextrel] == 0) continue;
      
      stack.push_back(nextnode);
      distance[nextnode] = curdist + 1;
      prevrel[nextnode] = -(nextrel+1);
      prev[nextnode] = cnode;
      tnodes.insert(nextnode);
      if (nextnode == enode) break;
      epos++;
    }
  }
  //printf ("distance[enode] = %d, curdist = %d\n", distance[enode], curdist + 1);
  fdist = distance[enode];
  
  if (distance[enode] >= 0){
    int cnode = enode;
    vector<int> rels;
    vector<bool> reldir;
    rels.resize(curdist + 1);
    reldir.resize(curdist + 1);

    int tdist = curdist;
    while (cnode != snode && tdist >= 0){
      int rel;
      bool tf;
      if (prevrel[cnode] < 0){
	rel =  -prevrel[cnode] - 1;
	tf = true;
      }
      else{
	rel =  prevrel[cnode];
	tf = false;
      }
      rels[tdist] = rel;
      reldir[tdist] = tf;
      tdist --; 
      cnode = prev[cnode];
    }
    for (int i=0; i<=curdist; i++)
      r.addRelationId(rels[i], reldir[i]);
  }
  for (set<int>::iterator it = tnodes.begin(); it != tnodes.end(); it++){
    distance[*it] = -1;
    prevrel[*it] = -1;
    prev[*it] = -1;
  }
  tnodes.clear();
  return fdist;
}

class DualsMore {
public:
  int operator() (const pair<int,double>& x, const pair<int,double>& y) const {
    return (x.second > y.second);
  }
};

void Solver::generateRulesS0Duals(int relationId, vector<Rule>& rules, vector<double>& duals, int maxRuleLength)
{
  vector<string>& relations = data_.getRelations();
  int nrelations = (int) relations.size();
  //  int relationId = params_.getRelationId();
  //  int maxRuleLength = params_.getMaxRuleLength();
  bool useRelationInRules = params_.getUseRelationInRules();
  bool useRulesOfLengthOne = params_.getUseRulesOfLengthOne();
  bool useReverseArcs = params_.getUseReverseArcsInRules();
  int numpairs = data_.getQuery(relationId).getNumEntityPairs();
  //  int numpairs = data_.getQuery().getNumEntityPairs();
  int minpairs = (numpairs > 100) ? numpairs / 20 : numpairs / 2;
  set<Rule> allrules;
  int mindist=100, maxdist=0;
  int prune_rules = 0;

  vector<int> prev, prevrel, distance;
  distance.resize(data_.getNumberNodes(), -1);
  prev.resize(data_.getNumberNodes(), -1);
  prevrel.resize(data_.getNumberNodes(), -1);
  set<int> tnodes;
  int nr= 0;
  int pathlen[GLOBALMAXPATHLEN];
 
  vector<int> firstrel;
  vector<int> firstinvrel;
  vector<int> lastrel;
  vector<int> lastinvrel;
  firstrel.resize(nrelations, 1);
  firstinvrel.resize(nrelations, 1);
  lastrel.resize(nrelations, 1);
  lastinvrel.resize(nrelations, 1);

  if (prune_rules){
    calculateFirstLast(relationId, firstrel, firstinvrel, lastrel, lastinvrel);
    for (int j=0; j<nrelations; j++){
      if (firstrel[j] > 0) printf("[%d, %d] ", j, firstrel[j]);
      if (firstinvrel[j] > 0) printf("[R%d, %d] ", j, firstinvrel[j]);
      if (lastrel[j] > 0) printf("[%d, %d] ", j, lastrel[j]);
      if (lastinvrel[j] > 0) printf("[R%d, %d] ", j, lastinvrel[j]);
    }
    printf("\n");
  }
  
  for (int i=0; i<maxRuleLength; i++) pathlen[i] = 0;
  
  //  assert(rules.size() == 0);
  int rulesSize = (int)rules.size();
  assert(maxRuleLength > 0);

  int numPairs = data_.getQuery(relationId).getNumEntityPairs();
  assert(numPairs == (int)duals.size());
  vector<pair<int,double> > dualsSorted(numPairs);
  for(int i=0; i<numPairs; i++)
    dualsSorted[i] = pair<int,double>(i,duals[i]);
  sort(dualsSorted.begin(),dualsSorted.end(),DualsMore());
#if 0
  cout<<"duals: ";
  for(int i=0; i<numPairs; i++)
    cout<<dualsSorted[i].second<<" ";
  cout<<endl;
#endif
  //  for (int i=0; i<data_.getQuery().getNumEntityPairs(); i++){
  //    int node1 = data_.getQuery().getEntityPairs()[i].first;
  //    int node2 = data_.getQuery().getEntityPairs()[i].second;
  //    Arc* ar =  data_.getQuery().getOutArcsWithRelation()[i];
  //  for (int i=0; i<data_.getQuery(relationId).getNumEntityPairs(); i++){
  for (int iduals=0; iduals<numPairs; iduals++){
    int i = dualsSorted[iduals].first;
    int node1 = data_.getQuery(relationId).getEntityPairs()[i].first;
    int node2 = data_.getQuery(relationId).getEntityPairs()[i].second;
    Arc* ar =  data_.getQuery(relationId).getOutArcsWithRelation()[i];

    Rule r;
    int dist= 0;

    /* TODO: clean up next line */
    for (int k=0; k<data_.getNumberNodes(); k++) distance[k] = -1;
    dist = find_sp_1(node1, node2, relationId, ar, distance, prev, prevrel, tnodes, r, firstrel, firstinvrel, maxRuleLength);
      
    if (dist > 0){
      int found = 0;
      if (dist < mindist) mindist = dist;
      if (dist > maxdist) maxdist = dist;
      pathlen[dist] ++;
      
      for (int j=0; j<rules.size(); j++)
	if (rules[j] == r){
	  found = 1;
	  break;
	}
      if (found == 0)
	rules.push_back(r);
      nr ++;
      //      if (rules.size() > 800) break;
      if (rules.size() > 9+rulesSize) break;
    }
  } // end for each query pair

  printf ("generated %d rules for %d pairs, mindist = %d, maxdist = %d\n", (int)(rules.size()), nr, mindist, maxdist); fflush(stdout);
  int maxcomplx = 0;
  for (int i=1, tsum=0; i<maxRuleLength; i++){
    printf ("p[%d] %d, ", i, pathlen[i]);
    tsum += pathlen[i];
    if (tsum > (nr * 6)/7 && maxcomplx == 0){
        setMaxComplexity(relationId, i+1);
	maxcomplx = i+1;
	printf("\n numpairs = %d, nr = %d, tsum = %d, Set max complexity = %d\n", numpairs, nr, tsum, maxcomplx);
      }
  }
  printf("\n"); fflush(stdout);
  /*
  if (allrules.size() > 0){
    for (set<Rule>::iterator it = allrules.begin(); it != allrules.end(); it++)
      rules.push_back(*it);
  }
  */
}
