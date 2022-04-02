
// The engine for evaluating BGP SPARQL queries
#include<unordered_map>
#include <iostream>
#include<string>

#include"engine.h"


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

    nested_index_loop_join( query, &query.triplePatterns, variablesMap, 0);
         

}




void substituteVAr(std::vector<TriplePattern> * patternsPtr, std::vector<TriplePattern> * newPatternsPtr, Term & var1, Term & newVar, int start_idx){

    auto & patterns = *patternsPtr; // vector is not copied here
    auto & newPatterns = *newPatternsPtr;


    // first copy the already derived facts
    for(size_t i = 0; i<start_idx; i++){
        newPatterns.push_back(patterns[i]);
    }

    for(size_t i = start_idx; i<patterns.size(); i++){
        newPatterns.push_back(patterns[i]);
        if(patterns[i].subject.isVariable() && patterns[i].subject.name == var1.name){ //Task replace it with hash in the Term class
            newPatterns[i].subject = newVar;

        }
        if(patterns[i].predicate.isVariable() && patterns[i].predicate.name == var1.name){ //Task replace it with hash in the Term class
            newPatterns[i].predicate = newVar;

        }
        if(patterns[i].object.isVariable() && patterns[i].object.name == var1.name){ //Task replace it with hash in the Term class
            newPatterns[i].object = newVar;

        }
    }
}


void Engine::nested_index_loop_join(Query & query, std::vector<TriplePattern> * patternsPtr, string2term_map variablesMap, int pattern_index){
    // cout<< "-top " <<pattern_index <<std::endl; 

    if( pattern_index == (query.size() +1) ){
        for(Term  mappingVar: query.mappingVariables){
            std::cout<<variablesMap[mappingVar.name].value << "\t"; //TASK remove the last tab
        }
        std::cout<<std::endl;
    }
    else{
        auto & patterns = *patternsPtr; // vector is not copied here
        auto * newPatternsPtr = new std::vector<TriplePattern>;
        auto & newPatterns = *newPatternsPtr;

        auto newPattern = patterns[pattern_index];

        

        
        if(newPattern.isAtom()){
            nested_index_loop_join(query, patternsPtr, variablesMap, pattern_index+1);
        }
        else{
            bool notmatched = true;
            for (auto patternMappings: rdfIndex.evaluate(newPattern.subject, newPattern.predicate, newPattern.object)) { //TASK not an it yet
                notmatched=  false;

                for(auto mapping: patternMappings){
                    // cout<<(std::get<0>(mapping)).name << " -> " <<(std::get<1>(mapping)).value << " " <<std::endl; //
                    
                    
                    //substitute
                    substituteVAr(patternsPtr,newPatternsPtr, get<0>(mapping), get<1>(mapping), pattern_index);

                    auto key =std::get<0>(mapping).name;
                    auto value = std::get<1>(mapping);

                    if(variablesMap.find(key) != variablesMap.end()){

                    cout<<"Error, overwirting some mapping" <<std::endl; //
                    return;


                    }else{
                        variablesMap[key]=value;
                    }
                    

                }
                if (notmatched) return; // when a partern can't be matched from the index


                // cout<< "-->got here " ; 
                // return;



                nested_index_loop_join(query, newPatternsPtr, variablesMap, pattern_index+1);

                for(auto mapping: patternMappings){
                    auto key =std::get<0>(mapping).name;

                    if(variablesMap.find(key) == variablesMap.end()){

                    cout<<"Error, while finding added key some mapping" <<std::endl; //
                    return;


                    }else{
                        variablesMap.erase(key);
                    }
                    

                }

            
            }
        }  
    }
}

