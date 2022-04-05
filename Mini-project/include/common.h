#ifndef COMMON_H_
#define COMMON_H_
#include <ostream>
#include<vector>
#include<unordered_map>
#include <tuple>

typedef unsigned int id_t;
typedef signed int triple_pointer_t; // TASK the papers' pointer is 6 bytes  and problably unsigned with special case for null or -1
using id_2_resource_type = std::unordered_map<std::tuple<std::string, bool>, int>;






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
        // constructor that takes all
        Term(const TermType _type, const id_t _value, const std::string _name):type(_type),value(_value), name(_name){}
        // constructuor for literatal and IRI
        Term(const TermType _type, const id_t _value):type(_type),value(_value), name(NON_VAR_NAME_PLACEHOLDER){}

        //constructor for Variables with value holding placeholder
        Term(const TermType _type, const std::string _name):type(_type),value((id_t) VAR_PLACEHOLDER), name(_name){}

        

        inline bool isVariable(){
            return type == VAR;
        }

        Term& operator=(Term other) noexcept 
        {
          type = other.type;
          value = other.value;
          name = other.name;
            
            return *this;
        }
        bool operator==(Term other) noexcept 
        {
          return (type == other.type) && (value == other.value) && (name == other.name);
        }
};



      // hash function for Term class

            // hash function for Term class

      //  template <>
      //   struct hash<Term>
      //   {
      //     std::size_t operator()(const Term& k) const
      //     {
      //       using std::size_t;
      //       using std::hash;
      //       using std::string;

      //       // Compute individual hash values for first,
      //       // second and third and combine them using XOR
      //       // and bit shifting:

      //       return ((hash<string>()(k.name)
      //               ^ (hash<Term::TermType>()(k.type) << 1)) >> 1)
      //               ^ (hash<int>()(k.value) << 1);
      //     }
      //   };






class TriplePattern{
  public:
    Term subject, predicate, object;
    std::string name; //TASK DEBUG remove
    TriplePattern(Term s, Term p, Term o, std::string n):name(n), subject(s), predicate(p), object(o){}
    TriplePattern(){}
    TriplePattern(Term s, Term p, Term o):subject(s), predicate(p), object(o){}


    bool isAtom(){
      return !subject.isVariable() && !predicate.isVariable() && !object.isVariable();
    }

    bool operator==(TriplePattern other) noexcept 
      {
        return (subject == other.subject) && (predicate == other.predicate) && (object == other.object);
      }

};

class Query{
  public:
    enum COMMAND{
        SELECT, COUNT
    };

    COMMAND command;
    std::vector<Term> mappingVariables; //TASK could be just a string??
    std::vector<TriplePattern> triplePatterns;
    std::vector<std::string> allVariables;

    int size(){
      return (int) triplePatterns.size();
    }

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