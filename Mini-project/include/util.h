#ifndef UTIL_H
#define UTIL_H

#include<iostream>
#include<vector>
#include<list>
#include<string>
#include<algorithm>

// #include <cctype>
// #include <locale>

using namespace std;




const std::string WHITESPACE = " \n\t\r\f\v";

// make them inline static
void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);
std::string trim_copy(std::string s);
void usageHelpMessage();



// TASK
template <typename T>
void printWrapper(const T& c) {
    cout << "[";
    bool flag = true;
    for (const auto& e : c) {
        if (flag) flag = false;
        else cout << ",";
        print(e);
    }
    cout << "]";
}

template <typename T>
void print(const std::vector<T>& v) { printWrapper(v); }

template <typename T>
void print(const std::list<T>& l) { printWrapper(l); }

template <typename T>
void print(const T& e) { cout << e; }

template <typename T>
void printl(const T& e) { cout << e << endl; }
//




#endif /* UTIL_H */
