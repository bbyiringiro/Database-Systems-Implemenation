#pragma once

#include"common.h"
#include"rdf_index.h"

using string2term_map = unordered_map<std::string, Term>;

void print_hyphens(); // helpher function for printing hyphens
class Engine{
    public:

        Engine(RdfIndex & _rdfIndex, vector<tuple<std::string, bool>> & _id_2_res_v):rdfIndex(_rdfIndex), id_2_res_v(_id_2_res_v){}

        int print_query_answers(Query & query);
        void nested_index_loop_join(Query & query, 
                                    std::vector<TriplePattern> * patternsPtr, 
                                    string2term_map variablesMap, 
                                    int pattern_index);
        void substituteVAr(std::vector<TriplePattern> * patternsPtr, 
                                    std::vector<TriplePattern> * newPatternsPtr, 
                                    Term & var1, Term & newVar, 
                                    int start_idx, 
                                    bool isFirstVAr);

        RdfIndex & rdfIndex; 
        vector<tuple<std::string, bool>> & id_2_res_v;

    private:
        std::string id2VariableConverter(id_t res_id);
        long results_counter = 0;


};

