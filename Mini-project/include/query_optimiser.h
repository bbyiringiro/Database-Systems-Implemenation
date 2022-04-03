#pragma once

#include<vector>
#include<set>
#include<string>
#include "common.h"

using namespace std;
set<string> var(TriplePattern & triplePattern);
short getHeuristicScore(Term & s_term, Term & p_term, Term & o_term);

 vector<TriplePattern> planQuery(vector<TriplePattern>  unproccessedTriples);