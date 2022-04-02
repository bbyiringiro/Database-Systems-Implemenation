#pragma once

#include"common.h"
#include"rdf_index.h"

using string2term_map = unordered_map<std::string, Term>;


class Engine{
    public:

        Engine(RdfIndex & _rdfIndex, vector<tuple<std::string, bool>> & _id_2_res_v):rdfIndex(_rdfIndex), id_2_res_v(_id_2_res_v){}

        void print_query_answers(Query & query);
        void nested_index_loop_join(Query & query, std::vector<TriplePattern> * patternsPtr, string2term_map variablesMap, int pattern_index);

        RdfIndex & rdfIndex; //TASK
        vector<tuple<std::string, bool>> & id_2_res_v;

    private:
        std::string id2VariableConverter(id_t res_id);
        long results_counter = 0;


};

