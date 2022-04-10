#include "parser_abstract.h"
#include<iostream>



Parser::Parser(char * buffer, size_t buffer_size):in_stream(ss), BUFFER(buffer), READ_BUFFER_SIZE(buffer_size){}

Parser::Parser( std::ifstream  &_filestream, char * buffer, size_t buffer_size):in_stream(_filestream),  BUFFER(buffer), READ_BUFFER_SIZE(buffer_size){}


bool Parser::loadBuffer(){
  char ch;

  loadBufferPos = 0;
  while(loadBufferPos < READ_BUFFER_SIZE){
    if (in_stream >> std::noskipws >> ch){
        // if (ch == '\n') continue;
        

        BUFFER[loadBufferPos]=ch;
        ++loadBufferPos;
    }
    else{ //EOF
        BUFFER[loadBufferPos]='\0';
        break;
    }
  }

  return loadBufferPos>0;
}


bool Parser::next(char & ch){ 

    if(readPos < loadBufferPos){ // check if have not read all the buffer
        if(BUFFER[readPos] == '\0') return false; // it indicates we have the all of the file
        ch =BUFFER[readPos++]; return true;
    }
    else{
        if(loadBuffer()) { // reload  the buffer and reset the reading position
            readPos = 0; return next(ch);
        } // else return false as we have finished the whole file 
        else return false;
    }
}





bool Parser::readToken(std::string & token){
    char ch;
   if(!readUntilNoSpace(ch)) return false;
    

    do{
        token +=ch;
        if(!next(ch)) {
            token ="";
            return false;
        }
    }while(!std::isspace(ch));
    // std::cout << token << std::endl;
    return true;

}

bool Parser::readUntilNoSpace(char & ch){
    do{
        if(!next(ch)) return false;
    }while(std::isspace(ch));

    return true;

}

bool Parser::readUntil(char stopChar, std::string & token){
   char ch;
    while(ch!=stopChar){
        token +=ch;
        if(!next(ch)) return false;

    }
    return true;

}
