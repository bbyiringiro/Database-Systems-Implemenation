#ifndef TURTLE_PARSER_H_
#define TURTLE_PARSER_H_
#include<string>
#include<vector>
#include<unordered_map>
#include "rdf_index.h"
#include "parser_abstract.h"



using std::vector;
using std::unordered_map;

class TurtleParser: public Parser
{
    private:
        bool parseResource(std::string & resource_name, char & ch);
        id_t encode_resource(std::string & resource_name, 
            id_t current_res_id, 
            id_t * triple_encoding_results, 
            short res_idx,  
            vector<std::string> & id_2_res_v, 
            unordered_map<std::string, 
            int> & res_2_id_map, RdfIndex & rdfIndex);



        const static size_t TRIPLE_READ_BUFFER_SIZE = 1024*10;
        char tripleBuffer[TRIPLE_READ_BUFFER_SIZE];
        int current_res_id = 0;

        
    public:
        TurtleParser();
        TurtleParser(std::ifstream  &_filestream );
        bool parseFile(std::string filepath, vector<std::string> & id_2_res_v, unordered_map<std::string, int> & res_2_id_m, RdfIndex & tripleTable);//TASK static 


    
};


#endif /* TURTLE_PARSER_H_ */

