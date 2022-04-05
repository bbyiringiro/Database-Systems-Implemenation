#ifndef SPARQL_PARSER_H_
#define SPARQL_PARSER_H_
#include<string>
#include<istream>
#include<sstream>
#include<vector>

#include "parser_abstract.h"
#include "common.h"




class SparqlParser: public Parser
{
    char END_OF_QUERY_CHAR ='}';



    id_2_resource_type & res_2_id_map;

    private:
        const static size_t QUERY_READ_BUFFER_SIZE = 1024;
        char queryBuffer[QUERY_READ_BUFFER_SIZE];


    public:
        SparqlParser(id_2_resource_type &  res_2_id_map);

        SparqlParser(id_2_resource_type &  res_2_id_map, std::ifstream & _filestream );
        Query parseQuery();
        Query parseStringQuery(std::string queryString);


    
        bool processBody(std::string  extraToken, Query&);
        bool parseTerm(Term & t, char & lastreadchar, Query&);
        bool parsePattern(Term & s, Term & p, Term & o, char & lastreadchar, Query&);
};


#endif /* SPARQL_PARSER_H_ */

