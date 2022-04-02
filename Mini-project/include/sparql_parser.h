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

    


    public:
    char queryBuffer[READ_BUFFER_SIZE];
        SparqlParser(unordered_map<std::string, int> &  res_2_id_map);
        

        SparqlParser(unordered_map<std::string, int> &  res_2_id_map, std::ifstream & _filestream );
        Query parseQuery();
        Query parseStringQuery(std::string queryString);


        
        bool loadBuffer();
        bool next(char & ch);
        bool readToken(std::string & token);
        bool readUntilNoSpace(char & ch);
        bool readUntil(char stopChar, std::string & token);
        bool processBody(std::string  extraToken, Query&);
        bool parseTerm(Term & t, char & lastreadchar, Query&);
        bool parsePattern(Term & s, Term & p, Term & o, char & lastreadchar, Query&);
};


#endif /* SPARQL_PARSER_H_ */

