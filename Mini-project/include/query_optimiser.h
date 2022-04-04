#pragma once

#define GENERATOR(name) \
struct name \
{ \
    template<typename F> \
    void operator()(F yield) \
/**/
#define _ };
 
template<typename Gen>
struct Adaptor
{
    Gen f;
    template<typename C>
    void operator*(C cont)
    {
        f(cont);
    }
};
 
template<typename Gen>
Adaptor<Gen> make_adaptor(Gen gen)
{
    return {gen};
}
 
#define FOREACH(arg, gen) make_adaptor(gen) * [&](arg)
 
/******************************************************/

#include<vector>
#include<set>
#include<string>
#include "common.h"

using namespace std;
set<string> var(TriplePattern & triplePattern);
short getHeuristicScore(Term & s_term, Term & p_term, Term & o_term);

 vector<TriplePattern> planQuery(vector<TriplePattern>  unproccessedTriples);