
#include<iostream>
#include<vector>
#include<list>
#include<string>
#include<algorithm>

#include "util.h"

using namespace std;

//String Manipulations Util

void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

 void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

 void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

 std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

 std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

 std::string trim_copy(std::string s) {
    trim(s);
    return s;
}


// Printinting Utils
void usageHelpMessage()
{
    std::cout << "Accepted commands: LOAD/SELECT/COUNT/QUIT" << std::endl;
    std::cout << "Usage: " << std::endl;
    std::cout << " - LOAD <file_name>" << std::endl;
    std::cout << " - SELECT <rest-of-query>"<< std::endl;
    std::cout << " - COUNT <rest-of-query>"<< std::endl;
    std::cout << " - QUIT: terminates the application"<< std::endl;

}
