#ifndef COMMON_H_
#define COMMON_H_
#include <ostream>
#include<vector>
#include<unordered_map>
#include <tuple>

typedef unsigned int id_t;
typedef signed int triple_pointer_t; // TASK the papers' pointer is 6 bytes  and problably unsigned with special case for null or -1


class Term{
    public:
        //variable place holders
        const short int VAR_PLACEHOLDER = -1;
        const std::string NON_VAR_NAME_PLACEHOLDER = "";

        enum TermType{
          VAR,
          IRI,
          LITERAL
        };
        TermType type;
        id_t value;
        std::string name;
        
        Term(){}
        // constructuor for literatal and IRI
        Term(const TermType _type, const id_t _value):type(_type),value(_value), name(NON_VAR_NAME_PLACEHOLDER){}

        //constructor for Variables with value holding placeholder
        Term(const TermType _type, const std::string _name):type(_type),value((id_t) VAR_PLACEHOLDER), name(_name){}

        

        inline bool isVariable(){
            return type == VAR;
        }
};



class TriplePattern{
  public:
    Term subject, predicate, object;

    TriplePattern(Term s, Term p, Term o):subject(s), predicate(p), object(o){}

};


class Exception {
  public:
    std::string msg;
    Exception(const std::string& msg):msg(msg){}
    ~Exception(){}
};







// function has to live in the std namespace 
// so that it is picked up by argument-dependent name lookup (ADL).
namespace std{
    namespace
    {

        // Code from boost
        // Reciprocal of the golden ratio helps spread entropy
        //     and handles duplicates.
        // See Mike Seymour in magic-numbers-in-boosthash-combine:
        //     https://stackoverflow.com/questions/4948780

        template <class T>
        inline void hash_combine(std::size_t& seed, T const& v)
        {
            seed ^= hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }

        // Recursive template code derived from Matthieu M.
        template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
        struct HashValueImpl
        {
          static void apply(size_t& seed, Tuple const& tuple)
          {
            HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
            hash_combine(seed, get<Index>(tuple));
          }
        };

        template <class Tuple>
        struct HashValueImpl<Tuple,0>
        {
          static void apply(size_t& seed, Tuple const& tuple)
          {
            hash_combine(seed, get<0>(tuple));
          }
        };
    }

    template <typename ... TT>
    struct hash<std::tuple<TT...>> 
    {
        size_t
        operator()(std::tuple<TT...> const& tt) const
        {                                              
            size_t seed = 0;                             
            HashValueImpl<std::tuple<TT...> >::apply(seed, tt);    
            return seed;                                 
        }                                              

    };
};









#endif //COMMON_H_