// RDF indexing data structure that implements Add and Evaluate functions
#include <iostream>

#include "rdf_index.h"
#include<stdint.h>







bool RdfIndex::add(id_t s, id_t p, id_t o){

    tuple<id_t, id_t, id_t> triple_key=make_tuple(s,p,o);
    
    if(Ispo.find(triple_key)==Ispo.end()){  //check if it does not already exists then add it
        
        
        triple_pointer_t Tnew = getTablesize();

        Ispo[triple_key] = Tnew; // update the index

        tuple<id_t, id_t> sp_key_index=make_tuple(s,p);

        Triple Rnew={s,p,o, -1,-1,-1};
        
        // build sp-linked list
        if(Isp.find(sp_key_index) != Isp.end()) {
            triple_pointer_t T = Isp[sp_key_index];
            insert_xy_list(Tnew, T, Rnew, Nsp);
            sp_linked_list_size++;

        }else{
            Isp[sp_key_index] = Tnew;

            if(Is.find(s) != Is.end()){
                triple_pointer_t T = Is[s];
                insert_xy_list(Tnew, T, Rnew, Nsp);
                sp_linked_list_size++;
            }else{
                Is[s]=Tnew;
            }
        }


        //build p linked list


        if(Ip.find(p) != Ip.end()) {
            triple_pointer_t T = Ip[p];
            insert_xy_list(Tnew, T, Rnew, Np);

        }else{
            Ip[p]=Tnew;
        }

        // build op linked list
        tuple<id_t, id_t> op_key_index=make_tuple(o,p);
        if(Iop.find(op_key_index) != Iop.end()) {
            triple_pointer_t T = Iop[op_key_index];
            insert_xy_list(Tnew, T, Rnew, Nop);
            op_linked_list_size++;


        }else{
            Iop[op_key_index] = Tnew;

            if(Io.find(o) != Io.end()){
                triple_pointer_t T = Io[o];
                insert_xy_list(Tnew, T, Rnew, Nop);
                op_linked_list_size++;

            }else{
                Io[o]=Tnew;
            }
        }


        tripleTable.push_back(Rnew);

        

        
        return true;
    } // endif  x in Ispo
    // else do nothing: don't add a duplicate




    return false;
}

vector<RdfIndex::Triple> RdfIndex::evaluate(Term & s_term, Term & p_term, Term & o_term){ //TASK put appropriate type
    vector<Triple> results;
    //bindings

    //(s, p, o) 
    if( !s_term.isVariable() && !p_term.isVariable() && !o_term.isVariable() ){
        tuple<id_t, id_t, id_t> tempT = make_tuple(s_term.value, p_term.value, o_term.value);
        if(Ispo.find(tempT) != Ispo.end()){
            triple_pointer_t T = Ispo[tempT];
             results.push_back(tripleTable[T]);
        }

    }
    //(s, ?, o) 
    else if( !s_term.isVariable() && p_term.isVariable() && !o_term.isVariable() ){

        if(sp_linked_list_size < op_linked_list_size){ // if sp-list is shorter
            if(Is.find(s_term.value) != Is.end()){
                triple_pointer_t T = Is[s_term.value];

                while(T != -1){ //traverslse ip list
                    auto row = tripleTable[T];
                    T = tripleTable[T].nextptr[Nsp];
                    if(row.Ro != o_term.value) continue;
                    results.push_back(row);
                }
            }
            

        }else{ // else if op list is shorter

            if(Io.find(o_term.value) != Io.end()){
                triple_pointer_t T = Io[o_term.value];

                while(T != -1){ //traverslse ip list
                    auto row = tripleTable[T];
                    T = tripleTable[T].nextptr[Nop];
                    if(row.Rs != s_term.value) continue;
                    results.push_back(row);
                }
            }


        }


    }
    //(?, p, o) 
    else if(s_term.isVariable() && !p_term.isVariable() && !o_term.isVariable() ){
        tuple<id_t, id_t> op_key_index=make_tuple(o_term.value,p_term.value);
        
        if(Iop.find(op_key_index) != Iop.end()) {
            triple_pointer_t T = Iop[op_key_index];
            auto row = tripleTable[T];

            while(row.Rp == p_term.value){ //traverslse ip list until Rp!=p
                results.push_back(row);
                T = tripleTable[T].nextptr[Nop];
                if (T == -1) break;
                row = tripleTable[T];
            }
        }

    }
    //(s, p, ?) 
    else if(!s_term.isVariable() && !p_term.isVariable()&& o_term.isVariable()){
        
        tuple<id_t, id_t> sp_key_index=make_tuple(s_term.value,p_term.value);
        
        if(Isp.find(sp_key_index) != Isp.end()) {
            triple_pointer_t T = Isp[sp_key_index];
            auto row = tripleTable[T];

            while(row.Rp ==p_term.value){ //traverslse sp list until Rp!=p or T=-1
                results.push_back(row);
                T = tripleTable[T].nextptr[Nsp];
                if (T == -1) break;
                row = tripleTable[T];
            }
        }

    }
    // //(?, ?, o)
    else if(s_term.isVariable() && p_term.isVariable() && !o_term.isVariable() ){

        if(Io.find(o_term.value) != Io.end()){
            triple_pointer_t T = Io[o_term.value];

            while(T != -1){ //traverslse ip list
                auto row = tripleTable[T];
                T = tripleTable[T].nextptr[Nop];
                if(s_term.name == p_term.name)
                    if(row.Rs != row.Rp) continue;
                results.push_back(row);
            }
        }
        

    }
    //(s, ?, ?)
    else if(!s_term.isVariable() && p_term.isVariable() && o_term.isVariable()){
        if(Is.find(s_term.value) != Is.end()){
            triple_pointer_t T = Is[s_term.value];

            while(T != -1){ //traverslse ip list
                auto row = tripleTable[T];
                T = tripleTable[T].nextptr[Nsp];
                if(p_term.name == o_term.name)
                    if(row.Rp != row.Ro) continue;
                results.push_back(row);
            }
        }

    }
    //(?, p, ?)
    else if(s_term.isVariable() && !p_term.isVariable() && o_term.isVariable()){

        if(Ip.find(p_term.value) != Ip.end()){
            triple_pointer_t T = Ip[p_term.value];

            while(T != -1){ //traverslse p list
                auto row = tripleTable[T];
                T = tripleTable[T].nextptr[Np];
                if(s_term.name == o_term.name)
                    if(row.Rs != row.Ro) continue;
                results.push_back(row);
            }
        }

    }
    //(?, ?, ?)
    else if(s_term.isVariable() && p_term.isVariable() && o_term.isVariable()){
        for(auto & row: tripleTable){
            if(s_term.name == p_term.name)
                if(row.Rs != row.Rp) continue;
            
            if(p_term.name == o_term.name)
                if(row.Rp != row.Ro) continue;

            if(s_term.name == o_term.name)
                if(row.Rs != row.Ro) continue;
            
            results.push_back(row);

        }

    }
    return results;
}



triple_pointer_t RdfIndex::getTablesize(){
    return (triple_pointer_t) tripleTable.size();
}

void RdfIndex::insert_xy_list(triple_pointer_t & Tnew, triple_pointer_t & T, Triple & Rnew, unsigned short Nidx){
    triple_pointer_t Tnext = tripleTable[T].nextptr[Nidx]; 
    Rnew.nextptr[Nidx]=Tnext;
    tripleTable[T].nextptr[Nidx]=Tnew;
}