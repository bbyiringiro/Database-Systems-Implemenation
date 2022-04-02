#pragma once
#include<string>
#include <sstream>
#include<fstream>
#include<istream>
#include<sstream>
#include<vector>


class Parser{
    public:

        Parser(char * buffer, size_t buffer_size);

        Parser(std::ifstream & _filestream, char * buffer, size_t buffer_size);


        virtual bool loadBuffer();
        virtual bool next(char & ch);
        virtual bool readToken(std::string & token);
        virtual bool readUntilNoSpace(char & ch);
        virtual bool readUntil(char stopChar, std::string & token);



        
        uint loadBufferPos=0;
        uint readPos=0;

        size_t READ_BUFFER_SIZE;

        char * BUFFER;


        std::istream & in_stream;
        std::stringstream ss;


};