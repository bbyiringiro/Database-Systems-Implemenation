#pragma once

#include"common.h"
#include"rdf_index.h"

using string2term_map = unordered_map<std::string, Term>;


class Engine{
    public:

        Engine(RdfIndex & _rdfIndex):rdfIndex(_rdfIndex){}

        void print_query_answers(Query & query);
        void nested_index_loop_join(Query & query, std::vector<TriplePattern> patterns, string2term_map variablesMap, int pattern_index);

        RdfIndex & rdfIndex; //TASK


};

