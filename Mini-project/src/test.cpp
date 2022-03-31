#include <iostream>
#include<tuple>
#include<vector>

#include "test.h"



#include "common.h"
#include "rdf_index.h"




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

    int counter;
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

    // (?, ?, ?)
    // Term term1 = Term(Term::TermType::VAR, "x");
    // Term term2 = Term(Term::TermType::VAR, "y");
    // Term term3 = Term(Term::TermType::VAR, "z");

    // (s,p,o)
    // Term term1 = Term(Term::TermType::LITERAL,2);
    // Term term2 = Term(Term::TermType::LITERAL,1);
    // Term term3 = Term(Term::TermType::IRI,4);

    //(s, p, ?) 
    // Term term1 = Term(Term::TermType::LITERAL,2);
    // Term term2 = Term(Term::TermType::LITERAL,1);
    // Term term3 = Term(Term::TermType::VAR, "z");

    // (s, ?, ?) 
    // Term term1 = Term(Term::TermType::LITERAL,1);
    // Term term2 = Term(Term::TermType::VAR,"y");
    // Term term3 = Term(Term::TermType::VAR, "z");


      // (?, p, o) 
    // Term term1 = Term(Term::TermType::VAR,"x");
    // Term term2 = Term(Term::TermType::LITERAL,3);
    // Term term3 = Term(Term::TermType::LITERAL, 4);

     // (?, ?, o) 
    // Term term1 = Term(Term::TermType::VAR,"x");
    // Term term2 = Term(Term::TermType::VAR,"y");
    // Term term3 = Term(Term::TermType::LITERAL, 2);



    // (?, ?, o) 
    // Term term1 = Term(Term::TermType::VAR,"x");
    // Term term2 = Term(Term::TermType::VAR,"y");
    // Term term3 = Term(Term::TermType::LITERAL, 2);


     // (s, ?, o) 
    // Term term1 = Term(Term::TermType::LITERAL,1);
    // Term term2 = Term(Term::TermType::VAR,"y");
    // Term term3 = Term(Term::TermType::LITERAL, 4);
    
    
    // (?, p, ?) 
    // Term term1 = Term(Term::TermType::VAR,"x");
    // Term term2 = Term(Term::TermType::LITERAL,1);
    // Term term3 = Term(Term::TermType::VAR, "x");



    // vector<RdfIndex::Triple> results= rdfIndex.evaluate(term1, term2, term3);

    // for (auto & r: results) cout<<r <<endl;

    

    return true;
}