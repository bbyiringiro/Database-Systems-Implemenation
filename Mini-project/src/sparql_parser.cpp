#include <iostream>
#include<string>
#include <sstream>
#include <istream>
#include<fstream>
#include <locale>
#include <algorithm>

#include "rdf_index.h"

#include "sparql_parser.h"
void toUpperStr(std::string & str);



SparqlParser::SparqlParser(unordered_map<std::string, int> &  _res_2_id_map):fin(ss), res_2_id_map(_res_2_id_map){
}

SparqlParser::SparqlParser(unordered_map<std::string, int> & _res_2_id_map, std::ifstream  &_filestream):fin(_filestream),res_2_id_map(_res_2_id_map){

}

void SparqlParser::parseStringQuery(std::string queryString){
    ss.clear();
    ss<<queryString;
     
     

    parseQuery();
    
}

//TASK Check if variables can be devided by commas or others



void SparqlParser::parseQuery(){
    //TASK check if it exist and throw expection

    mappingVariables.clear();
    triplePatterns.clear();
    allVariables.clear();
    std::string cmd ="";
    if(readToken(cmd)){
        toUpperStr(cmd);
        if(cmd !="SELECT" && cmd !="COUNT"){
            std::cout<<"SYNTAX ERROR: Command "<<cmd<< " is unknow. The systme only allows COUNT and SELECT, revise your query" << std::endl;
            return;
        }

        if (cmd=="SELECT")
            command = SELECT;
        else if(cmd == "COUNT")
            command = COUNT;
    }
    //TEST
    std::cout<< "COMMAND ID: "<< command<<std::endl;
    //Parse Mapping Variables
    std::string variable ="";
    std::string extraCharactersAfterWhere ="";

    while(readToken(variable)){
        if(variable[0] =='?'){
            mappingVariables.push_back(Term( Term::TermType::VAR, variable.substr(1, variable.size()) ));
            variable ="";
        }else{
            if(variable.size()< 5){
                std::cout<<"SYNTAX ERROR in the query at ..."<<variable<<"... You should have WHERE"<<std::endl;;
                return;
            }
            std::string maybeWHERE =variable.substr(0,5);
            toUpperStr(maybeWHERE);
            if(maybeWHERE != "WHERE"){
                std::cout<<"SYNTAX ERROR in the query at ..."<<variable<<"... You should have WHERE"<<std::endl;
                return;
            }
            if(variable.size() > 5){// in case no space between where and the the next, like WHERE{?....
            //TASK
            extraCharactersAfterWhere =variable.substr(5, variable.size());
            }
            break;
        }
    }
    //TEST print all mapping variables
    std::cout<< "VARIABLES: " ;
    for(Term & t: mappingVariables) std::cout<< t.name <<", ";
    std::cout<< std::endl;

    processBody(extraCharactersAfterWhere);

    //TEST all variables in the query

    std::cout<< "Variables in Body: " ;
    for(auto & t: allVariables) std::cout<< t <<", ";
    std::cout<< std::endl;

    // Validate
    for(Term & varTerm: mappingVariables){
        bool isInBody = false;
        for (std::string & varB: allVariables){
            if(varTerm.name == varB ) {
                isInBody = true;
                break;
            }
        }
        if(!isInBody){
            std::cout<< " All variable in the mapping have to appear in the body, ?"<<varTerm.name<<" is not in the body" <<std::endl;
            return;
        }
    }

    //TEST


        // if(==""){
        //     std::cout<<"No query found "<<std::endl;;
        //     return;
        // }
    

    

}


bool SparqlParser::loadBuffer(){
  char ch;

  loadBufferPos = 0;
  while(loadBufferPos < READ_BUFFER_SIZE){
    if (fin >> std::noskipws >> ch){
        // if (ch == '\n') continue;

        queryBuffer[loadBufferPos]=ch;
        ++loadBufferPos;
    }
    else{ //EOF
        queryBuffer[loadBufferPos]='\0';
        break;
    }
  }

  return loadBufferPos>0;
}


bool SparqlParser::next(char & ch){ 

    if(readPos < loadBufferPos){ // check if have not read all the buffer
        if(queryBuffer[readPos] == '\0') return false; // it indicates we have the all of the file
        ch =queryBuffer[readPos++]; return true;
    }
    else{
        if(loadBuffer()) { // reload  the buffer and reset the reading position
            readPos = 0; return next(ch);
        } // else return false as we have finished the whole file 
        else return false;
    }
}



bool SparqlParser::processBody(std::string  extraonLastToken){

    std::cout<< extraonLastToken<<std::endl;

     bool seenClosedBrace = false;
     char ch, lastreadchar;
     if(!readUntilNoSpace(ch)) {
         std::cout<< "Syntax Error in the body" <<std::endl;
         return false;
     }
     if(extraonLastToken == "" && ch !='{'){

         return false;
     }else{
         //handle the extraonLastToken
     }

     //get tripplePatters
    
    // while(true){

    // }

    while(!seenClosedBrace){
        // read subject term
        Term subject = Term();
        // read predicate ter
        Term predicate = Term();
        // read object
        Term object = Term();

        if(parsePattern(subject, predicate, object, lastreadchar)){
            triplePatterns.push_back(TriplePattern(subject, predicate, object));
            std::string temp="";
            if(!readUntil('.',temp)){
                std::cout<< "One pattern is not closed with a from here ..." <<"temp"<<std::endl;
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

bool SparqlParser::parsePattern(Term & subject, Term & predicate, Term & object, char & lastreadchar){

     if(!parseTerm(subject, lastreadchar)) {
        if(lastreadchar !=END_OF_QUERY_CHAR)
            std::cout<< "Syntax error in the querry near ... "<< subject.name<<std::endl;
        return false;
    }
    std::cout<<subject.name<< " -- ";


    if(!parseTerm(predicate, lastreadchar)) {
        std::cout<< "Syntax error in the querry near ... "<< predicate.name<<std::endl;
        return false;
    }
    std::cout<<predicate.name<< " -- ";

    if(!parseTerm(object, lastreadchar)) {
        std::cout<< "Syntax error in the querry near ... "<< object.name<<std::endl;
        return false;
    }
    std::cout<<object.name<<std::endl;


    return true;


}

bool SparqlParser::readToken(std::string & token){
    char ch;
   if(!readUntilNoSpace(ch)) return false;
    

    do{
        token +=ch;
        if(!next(ch)) return false;
    }while(!std::isspace(ch));
    return true;

}

bool SparqlParser::readUntilNoSpace(char & ch){
    do{
        if(!next(ch)) return false;
    }while(std::isspace(ch));

    return true;

}

bool SparqlParser::readUntil(char stopChar, std::string & token){
   char ch;
    while(ch!=stopChar){
        token +=ch;
        if(!next(ch)) return false;

    }
    return true;

}

bool SparqlParser::parseTerm(Term & t, char & lastreadchar)
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
                // std::cout<<std::endl<< "(Error: Resource name:(" << name<<") not found in memory)"<<std::endl;
                // return false;
            }
            t.value=res_2_id_map[name];
        }

        else if(ch =='\"'){
            t.type=Term::TermType::LITERAL;
            std::string name ="";

            if (!readUntil('\"',name)){
                std::cout<< "Error reading term in a pattern, could not find closing bracket >" <<std::endl;
                return false;
            }
            t.name = name;
            if(res_2_id_map.find(name) == res_2_id_map.end()){
                //TASK

                // std::cout<<std::endl<< "(Error: Resource name:(" << name<<") not found in memory)"<<std::endl;
                // return false;
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
            if (std::find(allVariables.begin(), allVariables.end(), name) == allVariables.end()) 
            allVariables.push_back(name);

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

