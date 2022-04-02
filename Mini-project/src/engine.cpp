
// The engine for evaluating BGP SPARQL queries
#include<unordered_map>
#include <iostream>
#include<string>

#include"engine.h"





void substituteVAr(std::vector<TriplePattern> patterns, Term var1, Term res, int start_idx){
    for(size_t i = start_idx; i<patterns.size(); i++){
        if(patterns[i].subject.isVariable() && patterns[i].subject.name == var1.name){ //Task replace it with hash in the Term class
            patterns[i].subject = res;

        }
        if(patterns[i].predicate.isVariable() && patterns[i].predicate.name == var1.name){ //Task replace it with hash in the Term class
            patterns[i].predicate = res;
        }
        if(patterns[i].object.isVariable() && patterns[i].object.name == var1.name){ //Task replace it with hash in the Term class
            patterns[i].object = res;
        }
    }
}

void Engine::print_query_answers(Query & query){
//     // Optmise the next merge join to do here ..and by sorting the patterns
    string2term_map variablesMap;
    std::cout<<"Printing Query" <<std::endl;
    int mapSize= query.mappingVariables.size();
    for(int i = 0; i<mapSize; ++i){
        std::cout<<query.mappingVariables[i].name;
        if(i+1 != mapSize) std::cout<<"\t";

    }
    std::cout<<"\n";

    nested_index_loop_join( query, query.triplePatterns, variablesMap, 0);
         

}

void Engine::nested_index_loop_join(Query & query, std::vector<TriplePattern> patterns, string2term_map variablesMap, int pattern_index){
    if( pattern_index == (query.size() +1) ){
        for(Term  mappingVar: query.mappingVariables){
            std::cout<<variablesMap[mappingVar.name].value << "\t"; //TASK remove the last tab
        }
        std::cout<<std::endl;
    }
    else{
        auto newPattern = patterns[pattern_index];
        if(newPattern.isAtom()){
            nested_index_loop_join(query, patterns, variablesMap, pattern_index+1);
        }else{
            bool notmatched = true;
            for (auto patternMappings: rdfIndex.evaluate(newPattern.subject, newPattern.predicate, newPattern.object)) { //TASK not an it yet
                notmatched=  false;

                for(auto mapping: patternMappings){
                    cout<<(std::get<0>(mapping)).name << " -> " <<(std::get<1>(mapping)).value << " " ; //
                    //substitute
                    substituteVAr(patterns, get<0>(mapping), get<1>(mapping), pattern_index);

                }
                if (notmatched) return; // when a partern can't be matched from the index

                nested_index_loop_join(query, patterns, variablesMap, pattern_index+1);

            
            }
        }  
    }
}

