
// The engine for evaluating BGP SPARQL queries
#include<unordered_map>
#include <iostream>
#include<string>

#include"engine.h"
#include "query_optimiser.h"



//--------------------------------------------------------------------
// Engine::print_query_answers
//
// Input    : a formed query
// Purpose  : matches the query with the RDF loaded in memeory and print or count the query results

// Return   : number of matched results.. or -1 if it was given an empty query.
//--------------------------------------------------------------------



int Engine::print_query_answers(Query & query){
    if(query.size()>0){
        string2term_map variablesMap;

        // prints the variable names
        if(query.command == Query::COMMAND::SELECT){ 
            print_hyphens();
            int mapSize= query.mappingVariables.size();
            for(int i = 0; i<mapSize; ++i){ 
                std::cout<<"?"<<query.mappingVariables[i].name;
                if(i+1 != mapSize) std::cout<<"\t";
            }
            std::cout<<"\n";
        }

        results_counter = 0;
        // std::cout<<"query.command "<< query.command << endl;
        // //DEBUG
        // cout<<"before optimiser size("<<query.triplePatterns.size()<<std::endl;
        // for (auto & pattern: query.triplePatterns){
        //     cout<<pattern.subject.name<<" < ";
        // }
        // cout<<std::endl;
        auto optimisedQueryPatterns = planQuery(query.triplePatterns);

        // cout<<"after optimiser ("<<optimisedQueryPatterns.size()<<std::endl;

        // for (auto & pattern: optimisedQueryPatterns){
        //     cout<<pattern.subject.name<<" < ";
        // }
        // cout<<"querying now..."<<endl;
        nested_index_loop_join( query, &optimisedQueryPatterns, variablesMap, 0);

        if(query.command == Query::COMMAND::SELECT){
            print_hyphens();
        }
        return results_counter;
    }
    else return -1;
         

}


// given a term id it returns it real variable name. Integer ID decoder
std::string Engine::id2VariableConverter(id_t res_id){
    if(res_id>=id_2_res_v.size()){
        std::cout<<"Error, the id you want to get its associated name is not in memory" << std::endl;
        throw;
    }

    auto name_type=id_2_res_v[res_id];
    if(std::get<1>(name_type)){
        return "<"+(std::get<0>(name_type))+">";
    }
    else{
        return "\""+(std::get<0>(name_type))+"\"";
    }


}



//--------------------------------------------------------------------
// Engine::substituteVAr
//
// Input    : a formed query
// Purpose  : Used during query matching to match variables that of mapped query

// Post Condition   : Given patternsPtr and var1, it copies all patern in patternsPtr to 
//newPatternsPtr as they are up to start_idx and then subject where the variable subsequently appear
//--------------------------------------------------------------------

void Engine::substituteVAr(std::vector<TriplePattern> * patternsPtr, std::vector<TriplePattern> * newPatternsPtr, Term & var1, Term & newVar, int start_idx, bool isFirstVAr){

    auto & patterns = *patternsPtr; // vector is not copied here
    auto & newPatterns = *newPatternsPtr;


    // first copy the already derived facts
    if(isFirstVAr)
        for(int i = 0; i<=start_idx; i++){
            newPatterns.push_back(patterns[i]);
        }

    for(size_t i = start_idx+1; i<patterns.size(); i++){
        if(isFirstVAr)
            newPatterns.push_back(patterns[i]);
        if(patterns[i].subject.isVariable() && patterns[i].subject.name == var1.name){ 
            newPatterns[i].subject = newVar;
        }
        if(patterns[i].predicate.isVariable() && patterns[i].predicate.name == var1.name){ 
            newPatterns[i].predicate = newVar;

        }
        if(patterns[i].object.isVariable() && patterns[i].object.name == var1.name){ 
            newPatterns[i].object = newVar;

        }

    }
}

/*
Nested index loop joins evaluate the query from left to right: when a triple pattern
is evaluated, it will bind the variables in subsequent triple patterns. Therefore, ≺
should not be applied to triple patterns directly. Rather, when scoring the remaining
triple patterns in each iteration of the greedy algorithm, one should treat all variables
from the previous triple patterns as bound.
Nested index loop join as described in the paper https://ojs.aaai.org/index.php/AAAI/article/view/8730 and the mini-project paper
*/
void Engine::nested_index_loop_join(Query & query, 
                                    std::vector<TriplePattern> * patternsPtr, 
                                    string2term_map variablesMap, int pattern_index){ 


    if( pattern_index == query.size() ){
        if(query.command == Query::COMMAND::SELECT){
            int mapSize= query.mappingVariables.size();
            for(int i = 0; i<mapSize; ++i){ 
                std::cout<<id2VariableConverter(variablesMap[query.mappingVariables[i].name].value);
                if(i+1 != mapSize) std::cout<<"\t";
            }
            std::cout<<std::endl;
        }
        // std::cout<<"query.command "<< query.command << endl;

        results_counter++;

        
        
    }
    else{
        auto & patterns = *patternsPtr; // the vector is not copied here

        auto newPattern = patterns[pattern_index];
        
        if(newPattern.isAtom()){
            nested_index_loop_join(query, patternsPtr, variablesMap, pattern_index+1);
        }
        else{
            bool notmatched = true;
            for(RdfIndex::Iterator it=rdfIndex.evaluate(newPattern.subject, newPattern.predicate, newPattern.object); it!=rdfIndex.end(); ++it){
                notmatched=  false;

                auto * newPatternsPtr = new std::vector<TriplePattern>; // allocate a new memory for the changed patterns
                auto & newPatterns = *newPatternsPtr;


                bool isFirstVAr = true;
                auto patternMappings =(*it);
                for(auto mapping: patternMappings){                    
                    
                    //substitute
                    substituteVAr(patternsPtr,newPatternsPtr, get<0>(mapping), get<1>(mapping), pattern_index, isFirstVAr);
                    isFirstVAr = false;
                    
                    auto value = std::get<1>(mapping);
                    auto key = std::get<0>(mapping).name;
                    if(variablesMap.find(key) == variablesMap.end()) 
                        variablesMap[key]=value;


                }

                if (notmatched) return; // when a partern can't be matched from the index

                nested_index_loop_join(query, newPatternsPtr, variablesMap, pattern_index+1);
                // reverse the process by removing the last mapped variables.
                for(auto mapping: patternMappings){
                    auto key = std::get<0>(mapping).name;
                    if(variablesMap.find(key) != variablesMap.end()) 
                        variablesMap.erase(key);
                }


                delete newPatternsPtr; // clear the created memeory

            
            }
        }  
    }
}
// just a helpher function/

void print_hyphens(){ 
    for (int i=0; i<10; ++i) std::cout<<"-";
            std::cout<<std::endl;
}