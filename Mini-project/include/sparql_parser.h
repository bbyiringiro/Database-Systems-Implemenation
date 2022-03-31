#ifndef SPARQL_PARSER_H_
#define SPARQL_PARSER_H_
#include<string>
#include<istream>
#include<sstream>
#include<vector>

#include"common.h"




class SparqlParser
{
    const static long READ_BUFFER_SIZE = 1024;
    char END_OF_QUERY_CHAR ='}';
    long loadBufferPos=0;
    long readPos=0;

    std::istream & fin;
    std::stringstream ss;

    unordered_map<std::string, int> & res_2_id_map;



    enum COMMAND{
        SELECT, COUNT
    };

    COMMAND command;
    std::vector<Term> mappingVariables;
    std::vector<TriplePattern> triplePatterns;
    std::vector<std::string> allVariables;
    


    public:
    char queryBuffer[READ_BUFFER_SIZE];
        SparqlParser(unordered_map<std::string, int> &  res_2_id_map);
        

        SparqlParser(unordered_map<std::string, int> &  res_2_id_map, std::ifstream & _filestream );
        void parseQuery();
        void parseStringQuery(std::string queryString);


        
        bool loadBuffer();
        bool next(char & ch);
        bool readToken(std::string & token);
        bool readUntilNoSpace(char & ch);
        bool readUntil(char stopChar, std::string & token);
        bool processBody(std::string  extraToken);
        bool parseTerm(Term & t, char & lastreadchar);
        bool parsePattern(Term & s, Term & p, Term & o, char & lastreadchar);
};


#endif /* SPARQL_PARSER_H_ */

