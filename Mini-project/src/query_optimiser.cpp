// The greedy join order optimisation query planner
#include<iostream>
#include<vector>
#include<set>
#include<string>
#include<common.h>
// #include <iterator>
#include<algorithm>
#include "query_optimiser.h"


using namespace std;




set<string> var(TriplePattern & triplePattern){
    set<string> results;
    if (triplePattern.subject.isVariable()){
        results.insert(triplePattern.subject.name);
    }
    if (triplePattern.predicate.isVariable()){
        results.insert(triplePattern.predicate.name);
    }
    if (triplePattern.object.isVariable()){
        results.insert(triplePattern.object.name);
    }

    return results;
}

short optmiserScore(Term & s_term, Term & p_term, Term & o_term){
    //(s, p, o) 
    if( !s_term.isVariable() && !p_term.isVariable() && !o_term.isVariable() ) return 1;
    //(s, ?, o) 
    else if( !s_term.isVariable() && p_term.isVariable() && !o_term.isVariable() ) return 2;
    //(?, p, o) 
    else if(s_term.isVariable() && !p_term.isVariable() && !o_term.isVariable() ) return 3;
    //(s, p, ?) 
    else if(!s_term.isVariable() && !p_term.isVariable()&& o_term.isVariable()) return 4;
    // //(?, ?, o)
    else if(s_term.isVariable() && p_term.isVariable() && !o_term.isVariable() ) return 5;
    //(s, ?, ?)
    else if(!s_term.isVariable() && p_term.isVariable() && o_term.isVariable()) return 6;
    //(?, p, ?)
    else if(s_term.isVariable() && !p_term.isVariable() && o_term.isVariable()) return 7;
    //(?, ?, ?)
    else if(s_term.isVariable() && p_term.isVariable() && o_term.isVariable()) return 8;
}

vector<TriplePattern> planQuery(vector<TriplePattern>  unproccessedTriples){
    set<string> bound;
    vector<TriplePattern> queryPlan;

    while(unproccessedTriples.size()>0){
        TriplePattern tripleBest;
        bool first=true;
        short scoreBest = 100;
        for(auto & unprocPattern: unproccessedTriples){

            short scoreCurTriple = optmiserScore(unprocPattern.subject, unprocPattern.predicate, unprocPattern.object);

            auto tripleVariables = var(unprocPattern);

            std::vector<string> varBIntersect;
            set_intersection(varBIntersect.begin(),varBIntersect.end(),bound.begin(),bound.end(), std::back_inserter(varBIntersect));
            if( (first || scoreCurTriple < scoreBest)&& tripleVariables.empty() || varBIntersect.size()==0){
                tripleBest = unprocPattern;
                scoreBest = scoreCurTriple;
                first=false;

            }
        }
        queryPlan.push_back(tripleBest);

        auto bestVars= var(tripleBest);
        bound.insert(bestVars.begin(), bestVars.end());
        unproccessedTriples.erase(std::remove(unproccessedTriples.begin(), unproccessedTriples.end(), tripleBest), unproccessedTriples.end());
        
    }

    return queryPlan;
}

