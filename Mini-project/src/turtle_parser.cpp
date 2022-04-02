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
using std::cout;



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

TurtleParser::TurtleParser():Parser(tripleBuffer, TRIPLE_READ_BUFFER_SIZE){
}

TurtleParser::TurtleParser(std::ifstream  &_filestream ):Parser(_filestream, tripleBuffer, TRIPLE_READ_BUFFER_SIZE){

}


id_t TurtleParser::encode_resource(std::string & resource_name, 
            id_t  current_res_id, 
            id_t * triple_encoding_results, 
            short res_idx,  
            vector<std::string> & id_2_res_v, 
            unordered_map<std::string, 
            int> & res_2_id_map, RdfIndex & rdfIndex){

    if(res_2_id_map.find(resource_name) == res_2_id_map.end()){
        triple_encoding_results[res_idx]=current_res_id;
        res_2_id_map[resource_name] = current_res_id++; // if a resrouce is not already in map give it id and increment
        //ID -> resources
        id_2_res_v.push_back(resource_name); // implicitely taken the id according to the order..
    }
    else{
        triple_encoding_results[res_idx]=res_2_id_map[resource_name];
    }
        // std::cout <<current_res_id<< std::endl;

    return current_res_id;

}


bool TurtleParser::parseResource(std::string & resource_name, char & ch){
    // char ch;
    
    if(readUntilNoSpace(ch)){
        cout<<ch<< resource_name<< endl;

        
        if(ch =='<'){
            if (!readUntil('>',resource_name)){
                std::cout<< " Error reading term in a triple, could not find closing bracket >" <<std::endl;
                return false;
            }
                    cout<<!in_stream.eof() << resource_name<< endl;

        }

        else if(ch =='\"'){
            if (!readUntil('\"',resource_name)){
                std::cout<< "Error reading term in a trile, could not find closing the quote \" " <<std::endl;
                return false;
            }
            // if(next(ch)){
            //     // lastChar=ch;
            //     if(ch=='^'){
            //         cout<<("here..."+ch) <<endl;
            //         std::string tempStr;
            //         if(ch =='<'){
            //             if (!readUntil('>',tempStr)){
            //                 std::cout<< " Error reading type in a literal, could not find closing bracket >" <<std::endl;
            //                 return false;
            //             }
            //         }else return false;
            //     }


            // }
            
        }
        else{
            std::cout<< "Uknown start of the triple, check your turtle file for a triple that starts with: "<<ch  <<std::endl;
            return false;
        }
    }else{
        return false;
    }

     return true;

}

bool TurtleParser::parseFile(std::string filepath, vector<std::string> & id_2_res_v, unordered_map<std::string, int> & res_2_id_map, RdfIndex & rdfIndex){

    int triples_num=0;
    

// if ( fin.is_open() ) {// TASK check file open...
    char ch;
    while(next(ch)){
        // read subject term
        std::string subject;
        // read predicate ter
        std::string predicate;
        // read object
        std::string object;


        if(parseResource(subject, ch)){
            if(parseResource(predicate, ch)){
                if(parseResource(object, ch)){

                    std::string temp="";
                    if(!readUntil('.',temp)){
                        std::cout<< "One Triple is not closed with a dot(.) from here ..." <<"temp"<<std::endl; //TASK get a line number 
                        return false;
                    }

                    id_t spo_ids[3];
                    current_res_id = encode_resource(subject,current_res_id, spo_ids, 0, id_2_res_v, res_2_id_map, rdfIndex);
                    current_res_id = encode_resource(predicate,current_res_id, spo_ids, 1, id_2_res_v, res_2_id_map, rdfIndex);
                    current_res_id = encode_resource(object,current_res_id, spo_ids, 2, id_2_res_v, res_2_id_map, rdfIndex);

                    std::cout <<subject<< " --" << predicate<< " --" << object << std::endl;
                    // std::cout <<spo_ids[0]<< " --" << spo_ids[1]<< " --" << spo_ids[2] << std::endl;
                    
                    // create the six column index
                    if(rdfIndex.add(spo_ids[0], spo_ids[1],spo_ids[2])){
                        ++triples_num;

                        // current_res_id++;
                    } //else duplicate tuple 
                    else{
                        cout<<"duplicate found\n"; // TASK remove
                    }


                }else{cout<<"Error in N-triple  file \n"; return false;}
            }
            else{cout<<"Error in N-triple  file \n";return false;}
        }
        else{cout<<"Error in N-triple  file \n"; return false;}

        

 
    }

    std::cout <<"resource lenght: "<< id_2_res_v.size() <<"=="<<res_2_id_map.size() << std::endl;
    std::cout <<"RdfIndex size: "<< rdfIndex.getTablesize() << std::endl;
    std::cout <<"Triple Number:  "<< triples_num << std::endl;
    std::cout<< "Done Parsing";
    return true;
    



        

        // while (turtleFile.good())
        // {
        //     getline(turtleFile, line);
        //     std::vector<std::string> tokens=split(line, " "); 
        //     // std::cout << line << std::endl;
        //     if(turtleFile.eof()) break;
        //     if(tokens.size() ==0) continue; // make it skip empty lines

        //     if(tokens.size()==4){
        //         id_t spo_ids[3];
        //         // spo parse 
        //         for(int i=0; i<3; ++i){

        //             // resources -> ID
        //             if(res_2_id_map.find(tokens[i]) == res_2_id_map.end()){ // should be transactions??

        //                 spo_ids[i]=current_res_id;
        //                 res_2_id_map[tokens[i]] = current_res_id++; // if a resrouce is not already in map give it id and increment
        //                 //ID -> resources
        //                 id_2_res_v.push_back(tokens[i]); // implicitely taken the id according to the order..
        //             }else{
        //                 // get stored id so we can create a new tuple
        //                 spo_ids[i]=res_2_id_map[tokens[i]];

        //             }

                    
        //         }

        //         // create the six column index

        //         if(rdfIndex.add(spo_ids[0], spo_ids[1],spo_ids[2])){
        //             ++triples_num;
        //         } //else duplicate tuple 

        //     }else{
        //         //
        //         std::cerr <<"ERROR: Found non-triple line, possible error in the .ttl";
        //     }
            



            
        // }
        
    //     std::cout <<"resource lenght: "<< id_2_res_v.size() <<"=="<<res_2_id_map.size() << std::endl;
    //     std::cout <<"RdfIndex size: "<< rdfIndex.getTablesize() << std::endl;
    //     std::cout<< "Done Parsing";
    //     turtleFile.close();
    //     return true;
    // }else{
    //     std::cout << "Couldn't open file at : "<< filepath << std::endl;
    //     return false;
    // }
}



