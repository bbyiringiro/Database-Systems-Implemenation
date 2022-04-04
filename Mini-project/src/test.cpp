#include <iostream>
#include<tuple>
#include<vector>
#include<fstream>

#include "test.h"



#include "common.h"
#include "rdf_index.h"
#include "sparql_parser.h"
#include"engine.h"
#include"query_optimiser.h"





using namespace std;

std::ostream & operator<<(std::ostream &out, const RdfIndex::Triple &t){
    return out << "| " << t.Rs << " |" << t.Rp << " |" << t.Ro << " |" << t.nextptr[0] << " |" << t.nextptr[1] << " |" << t.nextptr[2] <<endl;
}

bool test_rdf_ds_add(RdfIndex & rdfIndex){

    // paper triple table
    vector<tuple<int, int, int>> triples = {make_tuple(1,3,2),\
                                            make_tuple(2 ,1 ,4 ),
                                            make_tuple(1 ,1 ,2 ),
                                            make_tuple(1 ,3 ,4 ),
                                            make_tuple(2 ,1 ,3 ),
                                            make_tuple(1 ,1 ,1 ),
                                          };
    for(tuple<int, int, int>t: triples)
        rdfIndex.add( get<0>(t),  get<1>(t),  get<2>(t));

    int counter=0;
    for(auto & triple: rdfIndex.tripleTable){
        cout <<counter << ":" << triple << endl;
        ++counter;
    }
    
    cout<<"Index Is" << endl;

    for(auto it = rdfIndex.Is.begin(); it != rdfIndex.Is.end(); it++)
        cout<< "<"<< it->first << ">" << " | " <<it->second<< endl;

    cout<<"Index Isp" << endl;

    for(auto it = rdfIndex.Isp.begin(); it != rdfIndex.Isp.end(); it++)
        cout<< "<" << get<0>(it->first) << "," << get<1>(it->first)<< "> | "<< it->second<<endl;


    return true;
}

bool test_rdf_ds_evaluate(RdfIndex & rdfIndex){

    cout << "Query EValuations test" << endl;



    // (s,p,o)
    // Term term1 = Term(Term::TermType::LITERAL,2);
    // Term term2 = Term(Term::TermType::LITERAL,1);
    // Term term3 = Term(Term::TermType::IRI,4);

         // (s, ?, o) 
    // Term term1 = Term(Term::TermType::LITERAL,1);
    // Term term2 = Term(Term::TermType::VAR,"y");
    // Term term3 = Term(Term::TermType::LITERAL, 1);
   
      // (?, p, o) 
    // Term term1 = Term(Term::TermType::VAR,"x");
    // Term term2 = Term(Term::TermType::LITERAL,1);
    // Term term3 = Term(Term::TermType::LITERAL, 1);

    //(s, p, ?) 
    // Term term1 = Term(Term::TermType::LITERAL,1);
    // Term term2 = Term(Term::TermType::LITERAL,3);
    // Term term3 = Term(Term::TermType::VAR, "z");

    // (?, ?, o) 
    // Term term1 = Term(Term::TermType::VAR,"x");
    // Term term2 = Term(Term::TermType::VAR,"y");
    // Term term3 = Term(Term::TermType::LITERAL, 4);




     // (?, ?, o) 
    // Term term1 = Term(Term::TermType::VAR,"x");
    // Term term2 = Term(Term::TermType::VAR,"y");
    // Term term3 = Term(Term::TermType::LITERAL, 2);

    // (s, ?, ?) 
    // Term term1 = Term(Term::TermType::LITERAL,1);
    // Term term2 = Term(Term::TermType::VAR,"y");
    // Term term3 = Term(Term::TermType::VAR, "x");

    // (?, p, ?) 
    // Term term1 = Term(Term::TermType::VAR,"x");
    // Term term2 = Term(Term::TermType::LITERAL,3);
    // Term term3 = Term(Term::TermType::VAR, "z");


        // (?, ?, ?)
    // Term term1 = Term(Term::TermType::VAR, "x");
    // Term term2 = Term(Term::TermType::VAR, "y");
    // Term term3 = Term(Term::TermType::VAR, "z");



    // vector<RdfIndex::Triple> results= rdfIndex.evaluate(term1, term2, term3);

    // for (auto & it: rdfIndex.evaluate(term1, term2, term3)) { //TASK not it yet
    //     for(auto & mapping: it){
    //         cout<<(std::get<0>(mapping)).name << " -> " <<(std::get<1>(mapping)).value << " " ; //
    //     }
    //     cout<<endl;
    // }

    // for (auto & it = rdfIndex.evaluate(term1, term2, term3); it!=-1; it++) { //TASK not it yet
        // for(auto & mapping: it*){
        //     cout<<(std::get<0>(mapping)).name << " -> " <<(std::get<1>(mapping)).value << " " ; //
        // }
        // cout<<(rdfIndex.evaluate2(term1, term2, term3).curTriplePtr)<<endl;
    //     RdfIndex::Iterator resultingIterator = rdfIndex.evaluate2(term1, term2, term3);
    
    // subtempIteratorType  mappingVector;
    
    // cout<<"Index search case: "<<resultingIterator.CASE<<endl;
    // for(RdfIndex::Iterator & it=rdfIndex.evaluate2(term1, term2, term3); it!=rdfIndex.end(); ++it ){
        
    //     mappingVector = *(it);
    //     for(auto & mapping: mappingVector){
    //         cout<<(std::get<0>(mapping)).name << " -> " <<(std::get<1>(mapping)).value << " " ; //
    //     }
    //     cout<<endl;
    // }
        


        // cout<<(std::get<0>((mapping)).name << " :-> " <<(std::get<1>(mapping)).value << " " <<std::endl; //
    // }


    return true;
}

bool test_query(unordered_map<std::string, int> &  res_2_id_map, vector<tuple<std::string, bool>> & id_2_res_v,  RdfIndex & rdfIndex){
    std::ifstream _fin("/users/ms21jcbb/Practicals/DSI/data/test.txt");
    cout<<"\n\nstarting query parsing" << endl;
    SparqlParser sparqlParser(res_2_id_map, _fin);
    Query query =sparqlParser.parseQuery();
    if(query.allVariables.size()==0){
        std::cout<<"Something went wrong while processing the query" << std::endl;
        return false;
    }
    cout<<"Done query parsing sucessfully" << endl;




    //TEST
    std::cout<< "COMMAND ID: "<< query.command<<std::endl;
    //Parse Mapping Variables

    //TEST
    std::cout<< "Lenght: "<< query.size()<<std::endl;
    //Parse Mapping Variables
    
    //TEST print all mapping variables
    std::cout<< "Mapping VARIABLES: " ;
    for(Term & t: query.mappingVariables) std::cout<< t.name <<", ";
    std::cout<< std::endl;


    //TEST all variables in the query

    std::cout<< "Variables in Body: " ;
    for(auto & t: query.allVariables) std::cout<< t <<", ";
    std::cout<< std::endl;



    Engine queryEngine(rdfIndex, id_2_res_v);
    queryEngine.print_query_answers(query);


    return true;


}

bool test_query_optimiser(){

    vector<TriplePattern> unproccessed;

     // (?, ?, ?)
    Term term1 = Term(Term::TermType::VAR, "x");
    Term term2 = Term(Term::TermType::VAR, "y");
    Term term3 = Term(Term::TermType::VAR, "z");
    unproccessed.push_back(TriplePattern(term1, term2, term3, " (?, ?, ?) "));

    // (s,p,o)
    Term term11 = Term(Term::TermType::LITERAL,2);
    Term term21 = Term(Term::TermType::LITERAL,1);
    Term term31 = Term(Term::TermType::IRI,4);
    unproccessed.push_back(TriplePattern(term11, term21, term31, " (s,p,o) "));


    //(s, p, ?) 
    Term term12 = Term(Term::TermType::LITERAL,2);
    Term term22 = Term(Term::TermType::LITERAL,1);
    Term term32 = Term(Term::TermType::VAR, "z");
    unproccessed.push_back(TriplePattern(term12, term22, term32, " (s, p, ?)  "));


    // (s, ?, ?) 
    Term term13 = Term(Term::TermType::LITERAL,1);
    Term term23 = Term(Term::TermType::VAR,"y");
    Term term33 = Term(Term::TermType::VAR, "z");
    unproccessed.push_back(TriplePattern(term13, term23, term33, " (s, ?, ?) "));



      // (?, p, o) 
    Term term14 = Term(Term::TermType::VAR,"x");
    Term term24 = Term(Term::TermType::LITERAL,3);
    Term term34 = Term(Term::TermType::LITERAL, 4);
    unproccessed.push_back(TriplePattern(term14, term24, term34, " (?, p, o)  "));


   



    // (?, ?, o) 
    Term term16 = Term(Term::TermType::VAR,"x");
    Term term26 = Term(Term::TermType::VAR,"y");
    Term term36 = Term(Term::TermType::LITERAL, 2);
    unproccessed.push_back(TriplePattern(term16, term26, term36, " (?, ?, o)  "));



     // (s, ?, o) 
    Term term17 = Term(Term::TermType::LITERAL,1);
    Term term27 = Term(Term::TermType::VAR,"y");
    Term term37 = Term(Term::TermType::LITERAL, 4);
    unproccessed.push_back(TriplePattern(term17, term27, term37, " (s, ?, o)  "));

    
    
    // (?, p, ?) 
    Term term18 = Term(Term::TermType::VAR,"x");
    Term term28 = Term(Term::TermType::LITERAL,1);
    Term term38 = Term(Term::TermType::VAR, "x");
    unproccessed.push_back(TriplePattern(term18, term28, term38, " (?, p, ?) "));


    // auto patter = TriplePattern(term1, term2, term3);
    // cout<<getHeuristicScore(term1, term2, term3) << endl;

    auto orderRes=planQuery(unproccessed);
    for (auto & pattern: orderRes){
        cout<<pattern.name<<" < ";
    }


}
