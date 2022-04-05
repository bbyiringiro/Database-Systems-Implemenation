#ifndef RDF_INDEX_H_
#define RDF_INDEX_H_
#include <ostream>
#include<vector>
#include<unordered_map>
#include <tuple>
#include <array>
#include "common.h"


using namespace std;
// using namespace std::vector;
// using namespace std::tuple;

using mappingType =tuple<Term, Term>;
using subtempIteratorType = vector<mappingType>;
using tempIterator = vector<subtempIteratorType>;




class RdfIndex{


    public:
        static const triple_pointer_t INDEX_END_PTR = -1;

        struct Triple{
            id_t Rs, Rp, Ro;
            triple_pointer_t nextptr[3]; // [Nsp, Np, Nop]
        };

        class IndexIterator {
            public:
                // class RdfIndex;

                IndexIterator(triple_pointer_t t_ptr, RdfIndex & _rdfIndex):curTriplePtr(t_ptr), rdfIndex(_rdfIndex){
                    CASE = 8; //for (?, ?, ?)  so the iterator traverse the whole index starting form curTriplePtr where the default should 0
                }

                void update(triple_pointer_t curT, Term * _s_term, Term * _p_term, Term * _o_term, short _case){
                        curTriplePtr=curT;
                        nextPtr = curT;
                        s_term=_s_term;
                        p_term=_p_term;
                        o_term=_o_term;
                        CASE =_case;

                }

                subtempIteratorType operator*() {
                    if(curTriplePtr ==-1){
                        throw; // should not allow access a null pointer
                    }
                    RdfIndex::Triple row = rdfIndex.tripleTable[curTriplePtr];
                    return RdfIndex::mapVariables(row, *s_term, *p_term, *o_term);
                }

                IndexIterator& operator++();

                IndexIterator operator++(int){
                    IndexIterator iter = *this;
                    ++(*this);
                    return iter;
                }

                bool operator==(IndexIterator other) { // TASK change it to a reference
                    return curTriplePtr == other.curTriplePtr;
                }

                bool operator!=(IndexIterator other) {
                    return !(*this == other);
                }

                triple_pointer_t curTriplePtr;
                triple_pointer_t nextPtr;
                RdfIndex & rdfIndex;


            
                Term * s_term;
                Term * p_term;
                Term * o_term;
                short CASE;
        };

        using Iterator = IndexIterator;
       



        //Indices Ispo, Isp, Iop, Is, Ip,Io
        unordered_map<tuple<id_t, id_t,id_t>, triple_pointer_t>Ispo;
        unordered_map<tuple<id_t, id_t>, triple_pointer_t> Isp;
        unordered_map<tuple<id_t, id_t>, triple_pointer_t> Iop;

        unordered_map<id_t, triple_pointer_t> Is;
        unordered_map<id_t, triple_pointer_t> Ip;
        unordered_map<id_t, triple_pointer_t> Io;

        
        Iterator currIterator;
        Iterator endIterator; // this avoid creating the object everytime we are traversing the index






        std::vector<Triple> tripleTable;
        // IndexIterator iterator;
        RdfIndex():currIterator(Iterator(-1, *this)), endIterator(-1, *this){
            // triple_pointer_t t=0;
            // currIterator = Iterator(0);
        }
        bool add(id_t s, id_t p, id_t o);

        Iterator & evaluate(Term & s_term, Term & p_term, Term & o_term);
        tempIterator evaluate2(Term & s_term, Term & p_term, Term & o_term);
        static subtempIteratorType mapVariables(RdfIndex::Triple & row, Term & s_term, Term & p_term, Term & o_term);

        triple_pointer_t getTablesize();



        Iterator end(){
            return endIterator;//  this avoid creating the object everytime
            // Iterator(-1, *this);
        }

        

    private:
        void insert_xy_list(triple_pointer_t & Tnew, triple_pointer_t & T, Triple & t, unsigned short Nidx);

        unsigned short Nsp =0;
        unsigned short Np = 1;
        unsigned short Nop = 2;

        // linkedList sizes needed for evaluation 
        size_t sp_linked_list_size =0;
        size_t op_linked_list_size =0;

        
};









// bool operator==(const Triple &a, const Triple &b){
//     return (a.s == b.s && a.p == b.p && a.o == b.o);
// }


#endif /* RDF_INDEX_H_ */
