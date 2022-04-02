#include <iostream>
#include<vector>
#include<unordered_map>
#include<map>
#include<string>
#include<algorithm>
#include<fstream>






#include "util.h"
#include "turtle_parser.h"
#include "sparql_parser.h"
#include "rdf_index.h"
#include "sparql_parser.h"


#include "test.h"



using std::vector;
using std::unordered_map;

// Resources  dictionary integer encoding:
unordered_map<std::string, int> res_2_id_map;
vector<std::string> id_2_res_v;
RdfIndex rdfIndex;


enum COMMAND {Unknown, LOAD, SELECT, COUNT, QUIT};
std::map<std::string, COMMAND> COMMANDS;
void register_commands()
{
    COMMANDS["Unknown Command"] = Unknown; // avoid confusion of null and 0 index returned when a key is not in the map
    COMMANDS["LOAD"] = LOAD;
    COMMANDS["SELECT"] = SELECT;
    COMMANDS["COUNT"] = COUNT;
    COMMANDS["QUIT"] = QUIT;
}


//TASK
// - handle LOAD filename1 filename2 'file name'3


// The component implementing the command line interface outlined above
int main(int argc, char const *argv[])
{


    register_commands(); // registers allowed command for the CLI
    std::string input;

    // std::ifstream _fin("/users/ms21jcbb/Practicals/DSI/data/q01.txt");

    // SparqlParser sparqlParser(res_2_id_map, _fin);
    // Query query =sparqlParser.parseQuery();
    SparqlParser sparqlParser(res_2_id_map);



    // std::string temps = "SELECT ?Xm WHERE {?X  <http://www.w3.org/1999/02/22-rdf-syntax-ns#type>             <http://swat.cse.lehigh.edu/onto/univ-bench.owl#GraduateStudent>  . ?X <http://swat.cse.lehigh.edu/onto/univ-bench.owl#takesCourse> <http://www.Department0.University0.edu/GraduateCourse0>          .}";
    


    



    //tests
    // test_rdf_ds_add(rdfIndex);
    // test_rdf_ds_evaluate(rdfIndex);
     bool statustmep = TurtleParser::parseFile("/users/ms21jcbb/Practicals/DSI/data/LUBM-001-mat.ttl", id_2_res_v, res_2_id_map, rdfIndex);
    test_query(res_2_id_map, rdfIndex);
    return 0;
    


    do{
        std::cout<<">";
        std::getline(std::cin >> std::ws, input); // get input and remove leading whitespace and 
        trim(input);

        int command_idx=input.find_first_of(WHITESPACE);
        std::string input_action = input.substr(0, command_idx);
        std::transform(input_action.begin(), input_action.end(), input_action.begin(), ::toupper); // converts the command to uppercase 
    



        switch (COMMANDS[input_action])
        {
        case LOAD:
            {
            std::cout<<"LOAD" << std::endl;
            if(command_idx ==-1){
                std::cerr<<"Error: Pleae provide a filename " << std::endl;
                usageHelpMessage();
                break;
            }
            std::string filename = trim_copy(input.substr(command_idx));
            
            std::cout<<filename << std::endl;
            bool status = TurtleParser::parseFile(filename, id_2_res_v, res_2_id_map, rdfIndex);
            // print(status); //TO BE CHANGED
            
            break;
        }
        case SELECT:
            std::cout<<"SELECT" << std::endl;
            sparqlParser.parseStringQuery(input);
            break;
        case COUNT:
            std::cout<<"COUNT " << std::endl;
            sparqlParser.parseStringQuery(input);
            break;
        case QUIT:
            std::cout<<"Quitting ..." << std::endl;
            return 0;
        default:
            std::cerr<<"Unkown Command: " <<input<<"check your input" << std::endl;
            usageHelpMessage();
            break;
        }


    }while(true);

    return 0;
}



