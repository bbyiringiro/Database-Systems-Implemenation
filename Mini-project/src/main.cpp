#include <iostream>
#include<vector>
#include<unordered_map>
#include<map>
#include<string>
#include<algorithm>
#include<fstream>
#include <time.h>
#include <iomanip>




#include "util.h"
#include "turtle_parser.h"
#include "sparql_parser.h"
#include "rdf_index.h"
#include "sparql_parser.h"
#include"engine.h"



#include "test.h"



using std::vector;
using std::tuple;
using std::unordered_map;

// Resources  dictionary integer encoding:
// Resource -> ID
id_2_resource_type res_2_id_map;
//ID -> (resource, resourceType)
vector<tuple<std::string, bool>> id_2_res_v; // bool incates whether a resource is variable or literall
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

int parseTurtleFile(std::string file_name, vector<tuple<std::string, bool>>  & id_2_res_v, id_2_resource_type & res_2_id_map, RdfIndex & rdfIndex);
// The component implementing the command line interface outlined above
int main(int argc, char const *argv[])
{


    register_commands(); // registers allowed command for the CLI
    SparqlParser sparqlParser(res_2_id_map); // start query parser instance task
    std::string input;


    // ---------------------------
    // std::ifstream _fin("/users/ms21jcbb/Practicals/DSI/data/q01.txt");

    // SparqlParser sparqlParser(res_2_id_map, _fin);
    // Query query =sparqlParser.parseQuery()

    // std::string temps = "SELECT ?Xm WHERE {?X  <http://www.w3.org/1999/02/22-rdf-syntax-ns#type>             <http://swat.cse.lehigh.edu/onto/univ-bench.owl#GraduateStudent>  . ?X <http://swat.cse.lehigh.edu/onto/univ-bench.owl#takesCourse> <http://www.Department0.University0.edu/GraduateCourse0>          .}";
    



    //tests
    // test_rdf_ds_add(rdfIndex);
    // test_rdf_ds_evaluate(rdfIndex);
    // std::ifstream turtleFile("/users/ms21jcbb/Practicals/DSI/data/test.ttl"); //TASK putting in the file stream
    // TurtleParser turtleParser(turtleFile);
    // bool statustmep = turtleParser.parseFile(id_2_res_v, res_2_id_map, rdfIndex);//TASK
    // test_query(res_2_id_map,id_2_res_v, rdfIndex);
    // // test_query_optimiser();
    // return 0;
    // -----------------------

    
    double duration =0.;

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
                std::cerr<<"Error: Pleae provide a filename after LOAD command" << std::endl;
                usageHelpMessage();
                break;
            }
            std::string filename = trim_copy(input.substr(command_idx));
            clock_t begin_time = clock();
            int n=parseTurtleFile(filename, id_2_res_v, res_2_id_map, rdfIndex);
            clock_t end_time = clock();
            if (n!=-1){
                duration = double(end_time - begin_time)/(CLOCKS_PER_SEC/1000);
                std::cout<<setiosflags(ios::fixed)<<std::setprecision(0)<<n<<" triples loaded in "<<duration<<" ms.\n";
            }
            break;
        }
        case SELECT:
        {
            std::cout<<"SELECT" << std::endl;
            std::cout<<input<<endl;
            SparqlParser sparqlParser(res_2_id_map);
            Query query = sparqlParser.parseStringQuery(input);
            //Query is empty... TASK handle or return handle the of query failure
            Engine queryEngine(rdfIndex, id_2_res_v);
            clock_t begin_time = clock();
            int n = queryEngine.print_query_answers(query);
            clock_t end_time = clock();
            if (n!=-1){
                duration = double(end_time - begin_time)/(CLOCKS_PER_SEC/1000);
                std::cout<<setiosflags(ios::fixed)<<std::setprecision(0)<<n<<" results returned in "<<duration<<" ms.\n";
            }
            break;

        }
        case COUNT:{
            std::cout<<"COUNT " << std::endl;
            SparqlParser sparqlParser(res_2_id_map);
            Query query = sparqlParser.parseStringQuery(input);
            Engine queryEngine(rdfIndex, id_2_res_v);
            clock_t begin_time = clock();
            int n = queryEngine.print_query_answers(query);
            clock_t end_time = clock();
            if (n!=-1){
                duration = double(end_time - begin_time)/(CLOCKS_PER_SEC/1000);
                std::cout<<setiosflags(ios::fixed)<<std::setprecision(0)<<n<<" results returned in "<<duration<<" ms.\n";
            }
            break;
        }
        case QUIT:
            std::cout<<"Quitting ..." << std::endl;
            return 0;
        default:
            std::cerr<<"Unkown Command: " <<input<<"check your input" << std::endl;
            usageHelpMessage();
            break;
        }


    }while(1);

    return 0;
}


int parseTurtleFile(std::string file_name, vector<tuple<std::string, bool>> & id_2_res_v, id_2_resource_type & res_2_id_map, RdfIndex & rdfIndex){

    std::ifstream turtleFileStream(file_name);
    if(turtleFileStream.is_open()){

        TurtleParser turtleParser(turtleFileStream);
        return turtleParser.parseFile(id_2_res_v, res_2_id_map, rdfIndex);
        // if(!loadStatus){
        //     std::cout<<"Failed to parse the give turtle file, refer to the messge above to see why parsing failed"<<std::endl;
        // }


    }else{
        std::cout<<"Error: Could not open the file at: "<<file_name<<std::endl;
        return -1;
    }

}


// Query parseQuery(std::string stringQuery,vector<tuple<std::string, bool>> & id_2_res_v, id_2_resource_type & res_2_id_map, RdfIndex & rdfIndex){

//     if(stringQuery.size()<6){
//         SparqlParser sparqlParser(res_2_id_map, _fin);
//         Query query =sparqlParser.parseQuery();
//         return query;
//     }else{
//         std::cout<<"Error: Please check your query "<<file_name<<std::endl;
//     }

// }


