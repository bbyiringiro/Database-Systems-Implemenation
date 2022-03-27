#ifndef RDF_INDEX_H_
#define RDF_INDEX_H_
#include <ostream>
#include<vector>
#include<unordered_map>
#include <tuple>
#include "common.h"



using namespace std;




class RdfIndex{


    public:
        struct Triple{
            id_t Rs, Rp, Ro;
            triple_pointer_t nextptr[3]; // [Nsp, Np, Nop]
        };




        //Indices Ispo, Isp, Iop, Is, Ip,Io
        unordered_map<tuple<id_t, id_t,id_t>, triple_pointer_t>Ispo;
        unordered_map<tuple<id_t, id_t>, triple_pointer_t> Isp;
        unordered_map<tuple<id_t, id_t>, triple_pointer_t> Iop;

        unordered_map<id_t, triple_pointer_t> Is;
        unordered_map<id_t, triple_pointer_t> Ip;
        unordered_map<id_t, triple_pointer_t> Io;






        std::vector<Triple> tripleTable;

        bool add(id_t s, id_t p, id_t o);

        vector<Triple> evaluate(Term & s_term, Term & p_term, Term & o_term);
        

        triple_pointer_t getTablesize();

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
