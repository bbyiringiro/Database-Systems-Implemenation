#include <iostream>
#include<string>
#include <sstream>
#include <istream>
#include<fstream>
#include <locale>
#include <algorithm>

#include "rdf_index.h"

#include "sparql_parser.h"
#include "common.h"
void toUpperStr(std::string & str);



SparqlParser::SparqlParser(unordered_map<std::string, int> &  _res_2_id_map):Parser(queryBuffer, QUERY_READ_BUFFER_SIZE), res_2_id_map(_res_2_id_map){
}

SparqlParser::SparqlParser(unordered_map<std::string, int> & _res_2_id_map, std::ifstream  &_filestream):Parser(_filestream, queryBuffer, QUERY_READ_BUFFER_SIZE),res_2_id_map(_res_2_id_map){

}

Query SparqlParser::parseStringQuery(std::string queryString){
    ss.clear();
    ss<<queryString;
     
     

    return parseQuery();
    
}

//TASK Check if variables can be devided by commas or others



Query SparqlParser::parseQuery(){
    //TASK check if it exist and throw expection

    Query query =Query();
    
    std::string cmd ="";
    if(readToken(cmd)){
        toUpperStr(cmd);
        if(cmd !="SELECT" && cmd !="COUNT"){
            std::cout<<"SYNTAX ERROR: Command "<<cmd<< " is unknow. The systme only allows COUNT and SELECT, revise your query" << std::endl;
            return Query();
        }

        if (cmd=="SELECT")
            query.command = Query::SELECT;
        else if(cmd == "COUNT")
            query.command = Query::COUNT;
    }


    //Parse Mapping Variables
    std::string variable ="";
    std::string extraCharactersAfterWhere ="";

    while(readToken(variable)){
        if(variable[0] =='?'){
            query.mappingVariables.push_back(Term( Term::TermType::VAR, variable.substr(1, variable.size()) ));
            variable ="";
        }else{
            if(variable.size()< 5){
                std::cout<<"SYNTAX ERROR in the query at ..."<<variable<<"... You should have WHERE"<<std::endl;;
                return Query();
            }
            std::string maybeWHERE =variable.substr(0,5);
            toUpperStr(maybeWHERE);
            if(maybeWHERE != "WHERE"){
                std::cout<<"SYNTAX ERROR in the query at ..."<<variable<<"... You should have WHERE"<<std::endl;
                return Query();
            }
            if(variable.size() > 5){// in case no space between where and the the next, like WHERE{?....
            //TASK
            extraCharactersAfterWhere =variable.substr(5, variable.size());
            }
            break;
        }
    }


    // //TEST
    // std::cout<< "COMMAND ID: "<< query.command<<std::endl;
    // //Parse Mapping Variables
    
    // //TEST print all mapping variables
    // std::cout<< "Mapping VARIABLES: " ;
    // for(Term & t: query.mappingVariables) std::cout<< t.name <<", ";
    // std::cout<< std::endl;


    // //TEST all variables in the query

    // // std::cout<< "Variables in Body: " ;
    // // for(auto & t: query.allVariables) std::cout<< t <<", ";
    // // std::cout<< std::endl;

    processBody(extraCharactersAfterWhere, query);


    // Validate
    for(Term & varTerm: query.mappingVariables){
        bool isInBody = false;
        for (std::string & varB: query.allVariables){
            if(varTerm.name == varB ) {
                isInBody = true;
                break;
            }
        }
        if(!isInBody){
            std::cout<< " All variable in the mapping have to appear in the body, ?"<<varTerm.name<<" is not in the body" <<std::endl;
            return Query();
        }
    }

    //TEST


        // if(==""){
        //     std::cout<<"No query found "<<std::endl;;
        //     return;
        // }

    return query;
    

    

}



bool SparqlParser::processBody(std::string  extraonLastToken, Query &query){

    std::cout<< extraonLastToken<<std::endl;

     bool seenClosedBrace = false;
     char ch, lastreadchar;
     if(!readUntilNoSpace(ch)) {
         std::cout<< "Syntax Error in the body :"+ch <<std::endl;
         return false;
     }
     if(extraonLastToken == "" && ch !='{'){

         return false;
     }else{
         //handle the extraonLastToken
     }

     //get tripplePatters

    while(!seenClosedBrace){
        // read subject term
        Term subject = Term();
        // read predicate ter
        Term predicate = Term();
        // read object
        Term object = Term();

        if(parsePattern(subject, predicate, object, lastreadchar, query)){
            query.triplePatterns.push_back(TriplePattern(subject, predicate, object));
            std::string temp="";
            if(!readUntil('.',temp)){
                std::cout<< "One pattern is not closed with a dot(.) from here ..." <<"temp"<<std::endl;
                return false;
            }
        }
        else{
            if(lastreadchar ==END_OF_QUERY_CHAR) seenClosedBrace= true;
            else{
                std::cout<< "One pattern is not closed with a from here ..." <<"temp"<<std::endl;
                return false;
            }
        }
    }



   


    
    return true;

}

bool SparqlParser::parsePattern(Term & subject, Term & predicate, Term & object, char & lastreadchar, Query & query){

     if(!parseTerm(subject, lastreadchar, query)) {
        if(lastreadchar !=END_OF_QUERY_CHAR)
            std::cout<< "Syntax error in the querry near ... "<< subject.name<<std::endl;
        return false;
    }
    std::cout<<subject.name<< " -- ";


    if(!parseTerm(predicate, lastreadchar, query)) {
        std::cout<< "Syntax error in the querry near ... "<< predicate.name<<std::endl;
        return false;
    }
    std::cout<<predicate.name<< " -- ";

    if(!parseTerm(object, lastreadchar, query)) {
        std::cout<< "Syntax error in the querry near ... "<< object.name<<std::endl;
        return false;
    }
    std::cout<<object.name<<std::endl;


    return true;


}

bool SparqlParser::parseTerm(Term & t, char & lastreadchar, Query & query)
{
    char ch;
    
    if(readUntilNoSpace(ch)){
        lastreadchar = ch;
        if(ch =='<'){
            t.type=Term::TermType::IRI;
            std::string name ="";

            if (!readUntil('>',name)){
                std::cout<< " Error reading term in a pattern, could not find closing bracket >" <<std::endl;
                return false;
            }
            t.name = name;
            if(res_2_id_map.find(name) == res_2_id_map.end()){
                //TASK uncomment
                std::cout<<std::endl<< "(Error: Resource name:(" << name<<") not found in memory)"<<std::endl;
                return false; // TASK is this necessary
            }
            t.value=res_2_id_map[name];
        }

        else if(ch =='\"'){
            t.type=Term::TermType::LITERAL;
            std::string name ="";

            if (!readUntil('\"',name)){
                std::cout<< "Error reading term in a pattern, could not find closing quote \" " <<std::endl;
                return false;
            }
            t.name = name;
            if(res_2_id_map.find(name) == res_2_id_map.end()){
                //TASK

                std::cout<<std::endl<< "(Error: Resource name:(" << name<<") not found in memory)"<<std::endl;
                return false;
            }
            t.value=res_2_id_map[name];
        }

        else if(ch =='?'){
            t.type=Term::TermType::VAR;
            std::string name ="";
            if(!readToken(name)){
                std::cout<< " Error reading variable in a pattern" <<std::endl;

                return false;
            }
            t.name = name;
            if (std::find(query.allVariables.begin(), query.allVariables.end(), name) == query.allVariables.end()) 
            query.allVariables.push_back(name);

        }
        else{
            if(ch !=END_OF_QUERY_CHAR)
            std::cout<< "Uknown start of the term, check your query for a term that starts with: "<<ch  <<std::endl;
            return false;
        }
    }else{
        return false;
    }

    //  }else{
    //      //check if the file is query was processed correctly
    //  }
     return true;
}





// static bool iswhitespace(char c) { return (c==' ')||(c=='\t')||(c=='\n')||(c=='\r')
// void toLowerStr(std::string & str){
//     for(auto& c : str) c = toLower(c);
// }
void toUpperStr(std::string & str){
    for(auto& c : str) c = toupper(c);
}

