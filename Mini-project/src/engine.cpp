
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
    results_counter = 0;
    std::cout<<"query.command "<< query.command << endl;
    nested_index_loop_join( query, &query.triplePatterns, variablesMap, 0);

    // if(query.command == Query::COMMAND::COUNT){
        std::cout<<"Number of results is : "<<results_counter<<std::endl;
    // }
         

}



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




void substituteVAr(std::vector<TriplePattern> * patternsPtr, std::vector<TriplePattern> * newPatternsPtr, Term & var1, Term & newVar, int start_idx, bool isFirstVAr){

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
        if(patterns[i].subject.isVariable() && patterns[i].subject.name == var1.name){ //Task replace it with hash in the Term class
            newPatterns[i].subject = newVar;
            //  cout<<newPatterns[i].subject.isVariable() << "index=" <<i<<endl;
        }
        if(patterns[i].predicate.isVariable() && patterns[i].predicate.name == var1.name){ //Task replace it with hash in the Term class
            newPatterns[i].predicate = newVar;

        }
        if(patterns[i].object.isVariable() && patterns[i].object.name == var1.name){ //Task replace it with hash in the Term class
            newPatterns[i].object = newVar;

        }

    }


    // for(size_t i = start_idx+1; i<newPatterns.size(); i++){
    //                     if(newPatterns[i].subject.isVariable() && newPatterns[i].subject.name==var1.name){
                            
    //                         cout<<newPatterns[i].subject.isVariable()<<"Source in function Error for sub" <<newPatterns[i].subject.name<<"at index i="<<i<<"while the pattern idx is"<<start_idx<<endl;
    //                         throw;
    //                     }
    //                 }
}


void Engine::nested_index_loop_join(Query & query, std::vector<TriplePattern> * patternsPtr, string2term_map variablesMap, int pattern_index){ //TASK pass query by reference
    // cout<< "-top " <<pattern_index <<std::endl; 

    if( pattern_index == query.size() ){
        if(query.command == Query::COMMAND::SELECT){
            for(Term  mappingVar: query.mappingVariables){
                

                std::cout<<id2VariableConverter(variablesMap[mappingVar.name].value) << "\t"; //TASK remove the last tab
            }
            std::cout<<std::endl;
        }
        // std::cout<<"query.command "<< query.command << endl;



        
        results_counter++;
    }
    else{
        auto & patterns = *patternsPtr; // vector is not copied here

        auto newPattern = patterns[pattern_index];

        

        
        if(newPattern.isAtom()){
            nested_index_loop_join(query, patternsPtr, variablesMap, pattern_index+1);
        }
        else{
            bool notmatched = true;
            for (auto patternMappings: rdfIndex.evaluate(newPattern.subject, newPattern.predicate, newPattern.object)) { //TASK not an it yet
                notmatched=  false;

                auto * newPatternsPtr = new std::vector<TriplePattern>;
                auto & newPatterns = *newPatternsPtr;


                // cout<<"1---------"<<std::endl; //

                bool isFirstVAr = true;
                for(auto mapping: patternMappings){
                    // cout<<(std::get<0>(mapping)).name << " :-> " <<(std::get<1>(mapping)).value << " " <<std::endl; //
                    
                    
                    //substitute
                    substituteVAr(patternsPtr,newPatternsPtr, get<0>(mapping), get<1>(mapping), pattern_index, isFirstVAr);
                    isFirstVAr = false;
                    auto key =std::get<0>(mapping).name;
                    auto value = std::get<1>(mapping);

                    if(variablesMap.find(key) != variablesMap.end()){

                        cout<<"Error, overwriting some mapping" <<std::endl; //
                        throw;
                    

                    }else{
                        variablesMap[key]=value;
                    }


                }

                // break;
                if (notmatched) return; // when a partern can't be matched from the index


                // cout<< "-->got here " ; 
                // return;



                nested_index_loop_join(query, newPatternsPtr, variablesMap, pattern_index+1);
                // cout<<"---------"<<std::endl; //
                // cout <<std::endl; 

                for(auto mapping: patternMappings){
                    auto key =std::get<0>(mapping).name;

                    if(variablesMap.find(key) == variablesMap.end()){

                    cout<<"Error, while finding added key some mapping" <<std::endl; //
                    return;


                    }else{
                        variablesMap.erase(key);
                    }
                    

                }

                delete newPatternsPtr;

            
            }
        }  
    }
}

