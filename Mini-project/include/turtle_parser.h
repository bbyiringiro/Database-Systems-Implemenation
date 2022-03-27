#ifndef TURTLE_PARSER_H_
#define TURTLE_PARSER_H_
#include<string>
#include<vector>
#include<unordered_map>
#include "rdf_index.h"



using std::vector;
using std::unordered_map;

class TurtleParser
{
    public:
        static bool parseFile(std::string filepath, vector<std::string> & id_2_res_v, unordered_map<std::string, int> & res_2_id_m, RdfIndex & tripleTable);
};


#endif /* TURTLE_PARSER_H_ */

