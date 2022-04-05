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
        bool parseResource(std::string & resource_name, bool & type);
        id_t encode_resource(
            std::tuple<std::string, bool> resource_name_type, 
            id_t current_res_id, 
            id_t * triple_encoding_results, 
            short res_idx,  
            vector<tuple<std::string, bool>> & id_2_res_v, 
            id_2_resource_type & res_2_id_map, 
            RdfIndex & rdfIndex,
            bool resourceType);



        const static size_t TRIPLE_READ_BUFFER_SIZE = 1024*10;
        char tripleBuffer[TRIPLE_READ_BUFFER_SIZE];
        int current_res_id = 0;

        
    public:
        TurtleParser();
        TurtleParser(std::ifstream  &_filestream );
        bool parseFile(vector<tuple<std::string, bool>> & id_2_res_v, id_2_resource_type & res_2_id_m, RdfIndex & tripleTable);//TASK static 


    
};


#endif /* TURTLE_PARSER_H_ */

