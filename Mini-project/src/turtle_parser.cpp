// The component for parsing and importing Turtle files
#include <iostream>
#include<fstream>
#include <string>
#include<vector>
#include<unordered_map>
#include <tuple>

#include "common.h"
#include "turtle_parser.h"
#include "rdf_index.h"


// #include "util.h"

//TASK update according to official specifications and make as like sparql parser

using std::vector;
using std::unordered_map;



std::vector<std::string> split(std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}



bool TurtleParser::parseFile(std::string filepath, vector<std::string> & id_2_res_v, unordered_map<std::string, int> & res_2_id_map, RdfIndex & rdfIndex){
    std::ifstream turtleFile(filepath);
    std::string line;

    int triples_num=0;
    int current_res_id = 0;
    if ( turtleFile.is_open() ) {
        while (turtleFile.good())
        {
            getline(turtleFile, line);
            std::vector<std::string> tokens=split(line, " "); 
            // std::cout << line << std::endl;
            if(turtleFile.eof()) break;
            if(tokens.size() ==0) continue; // make it skip empty lines

            if(tokens.size()==4){
                id_t spo_ids[3];
                // spo parse 
                for(int i=0; i<3; ++i){

                    // resources -> ID
                    if(res_2_id_map.find(tokens[i]) == res_2_id_map.end()){ // should be transactions??

                        spo_ids[i]=current_res_id;
                        res_2_id_map[tokens[i]] = current_res_id++; // if a resrouce is not already in map give it id and increment
                        //ID -> resources
                        id_2_res_v.push_back(tokens[i]); // implicitely taken the id according to the order..
                    }else{
                        // get stored id so we can create a new tuple
                        spo_ids[i]=res_2_id_map[tokens[i]];

                    }

                    
                }

                // create the six column index

                if(rdfIndex.add(spo_ids[0], spo_ids[1],spo_ids[2])){
                    ++triples_num;
                } //else duplicate tuple 

            }else{
                //
                std::cerr <<"ERROR: Found non-triple line, possible error in the .ttl";
            }
            



            
        }
        
        std::cout <<"resource lenght: "<< id_2_res_v.size() <<"=="<<res_2_id_map.size() << std::endl;
        std::cout <<"RdfIndex size: "<< rdfIndex.getTablesize() << std::endl;
        std::cout<< "Done Parsing";
        turtleFile.close();
        return true;
    }else{
        std::cout << "Couldn't open file at : "<< filepath << std::endl;
        return false;
    }
}



