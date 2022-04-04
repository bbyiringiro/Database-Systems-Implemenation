// RDF indexing data structure that implements Add and Evaluate functions
#include <iostream>
#include "rdf_index.h"
#include<stdint.h>


// using namespace std::vector;
// using namespace std::tuple;



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


// TASK helper

subtempIteratorType RdfIndex::mapVariables(RdfIndex::Triple & row, Term & s_term, Term & p_term, Term & o_term){
    subtempIteratorType mappings;
    if(s_term.isVariable()){
       mappingType t = make_tuple( s_term, Term(Term::TermType::LITERAL,row.Rs, s_term.name) );  // TASK not necessary literal ... .. and the third arugment is for debugging purposesimplement a way to find type of terms from their intiger encodings..
       mappings.push_back(t);  
    }
    // else{
    //     mappingType t =  make_tuple(s_term,s_term);
    //     mappings.push_back(t);
          
    // }

    if(p_term.isVariable()){
        mappingType t = make_tuple(p_term, Term(Term::TermType::LITERAL,row.Rp, p_term.name));
        mappings.push_back(t);  
    }
    // else{
    //      mappingType t = make_tuple(p_term,p_term);
    //     mappings.push_back(t);  
    // }

    if(o_term.isVariable()){
        mappingType t = make_tuple(o_term,Term(Term::TermType::LITERAL,row.Ro, o_term.name));
        mappings.push_back(t);  
    }
    // else{
    //      mappingType t = make_tuple(o_term,o_term);
    //     mappings.push_back(t);  

    // }

    return mappings;
    
}


RdfIndex::Iterator& RdfIndex::evaluate(Term & s_term, Term & p_term, Term & o_term){ //TASK put appropriate type

//     tempIterator result;
//     //bindings

    //(s, p, o) 
    if( !s_term.isVariable() && !p_term.isVariable() && !o_term.isVariable() ){
        tuple<id_t, id_t, id_t> tempT = make_tuple(s_term.value, p_term.value, o_term.value);
        if(Ispo.find(tempT) != Ispo.end()){
            triple_pointer_t T = Ispo[tempT];
            currIterator.update(T, &s_term, &p_term, &o_term, 0);
            return ++currIterator;// ++take the iterator to the match
        }

    }
    //(s, ?, o) 
    else if( !s_term.isVariable() && p_term.isVariable() && !o_term.isVariable() ){

        if(sp_linked_list_size < op_linked_list_size){ // if sp-list is shorter
            if(Is.find(s_term.value) != Is.end()){
                triple_pointer_t T = Is[s_term.value];

                currIterator.update(T, &s_term, &p_term, &o_term, 1);
                return ++currIterator; //++take the iterator to the first matching element
            }
            
            
        }else{ // else if op list is shorter

            if(Io.find(o_term.value) != Io.end()){
                triple_pointer_t T = Io[o_term.value];
                currIterator.update(T, &s_term, &p_term, &o_term, 2);
                return ++currIterator;
            }
        }

    }
    //(?, p, o) 
    else if(s_term.isVariable() && !p_term.isVariable() && !o_term.isVariable() ){
        tuple<id_t, id_t> op_key_index=make_tuple(o_term.value,p_term.value);
        
        if(Iop.find(op_key_index) != Iop.end()) {
            triple_pointer_t T = Iop[op_key_index];
            auto row = tripleTable[T];
            currIterator.update(T, &s_term, &p_term, &o_term, 3);
            return ++currIterator;
        }

    }
    //(s, p, ?) --
    else if(!s_term.isVariable() && !p_term.isVariable()&& o_term.isVariable()){
        
        tuple<id_t, id_t> sp_key_index=make_tuple(s_term.value,p_term.value);
        
        if(Isp.find(sp_key_index) != Isp.end()) {
            triple_pointer_t T = Isp[sp_key_index];
            auto row = tripleTable[T];
            currIterator.update(T, &s_term, &p_term, &o_term, 4);
            return ++currIterator;

        }

    }
    //(?, ?, o) --
    else if(s_term.isVariable() && p_term.isVariable() && !o_term.isVariable() ){

        if(Io.find(o_term.value) != Io.end()){
            triple_pointer_t T = Io[o_term.value];
            currIterator.update(T, &s_term, &p_term, &o_term, 5);
            return ++currIterator;
        }
        

    }
    //(s, ?, ?)
    else if(!s_term.isVariable() && p_term.isVariable() && o_term.isVariable()){
        if(Is.find(s_term.value) != Is.end()){
            triple_pointer_t T = Is[s_term.value];
            currIterator.update(T, &s_term, &p_term, &o_term, 6);
            return ++currIterator;
        }

    }
    // //(?, p, ?)
    else if(s_term.isVariable() && !p_term.isVariable() && o_term.isVariable()){

        if(Ip.find(p_term.value) != Ip.end()){
            triple_pointer_t T = Ip[p_term.value];
            currIterator.update(T, &s_term, &p_term, &o_term, 7);
            return ++currIterator;

        }

    }
    //(?, ?, ?)
    else if(s_term.isVariable() && p_term.isVariable() && o_term.isVariable()){
        currIterator.update(0, &s_term, &p_term, &o_term, 8);
        return ++currIterator;

    }
    // points no the end
    currIterator.update(INDEX_END_PTR, &s_term, &p_term, &o_term, -1); //case -1 // the default
    return currIterator;
}



triple_pointer_t RdfIndex::getTablesize(){
    return (triple_pointer_t) tripleTable.size();
}

void RdfIndex::insert_xy_list(triple_pointer_t & Tnew, triple_pointer_t & T, Triple & Rnew, unsigned short Nidx){
    triple_pointer_t Tnext = tripleTable[T].nextptr[Nidx]; 
    Rnew.nextptr[Nidx]=Tnext;
    tripleTable[T].nextptr[Nidx]=Tnew;
}


RdfIndex::IndexIterator& RdfIndex::IndexIterator::operator++(){
                    switch(CASE){
                        case 0: //(s, p, o) 
                        {
                            if(nextPtr != INDEX_END_PTR){  
                                curTriplePtr=nextPtr;
                                nextPtr=INDEX_END_PTR;
                            }else{
                                curTriplePtr= nextPtr;
                            }
                            break;
                        }
                        case 1: //(s, ?, o) 
                        {
                            if(nextPtr != INDEX_END_PTR){  //sp-list
                                auto & row = rdfIndex.tripleTable[nextPtr];
                                
                                while(nextPtr !=INDEX_END_PTR && row.Ro != (*o_term).value) {
                                    nextPtr = row.nextptr[rdfIndex.Nsp];
                                    row = rdfIndex.tripleTable[nextPtr];
                                }
                                if(row.Ro == (*o_term).value){
                                    curTriplePtr = nextPtr;
                                    row = rdfIndex.tripleTable[nextPtr];
                                    if(nextPtr!=INDEX_END_PTR)
                                        nextPtr = row.nextptr[rdfIndex.Nsp];
                                }else{
                                    curTriplePtr= nextPtr;
                                }
                            }else{ // if nxt INDEX_END_PTR
                                curTriplePtr= nextPtr;
                            }
                            break;
                        }
                        case 2:
                        {
                            //symetric to case 1

                            if(nextPtr != INDEX_END_PTR){  //op-list

                                auto & row = rdfIndex.tripleTable[nextPtr];
                                
                                while(nextPtr !=INDEX_END_PTR && row.Rs != (*s_term).value) {
                                    nextPtr = row.nextptr[rdfIndex.Nop];
                                    row = rdfIndex.tripleTable[nextPtr];
                                }
                                if(row.Rs == (*s_term).value){
                                    curTriplePtr = nextPtr;
                                    row = rdfIndex.tripleTable[nextPtr];
                                    if(nextPtr!=INDEX_END_PTR)
                                        nextPtr = row.nextptr[rdfIndex.Nop];
                                }else{
                                    curTriplePtr= nextPtr;
                                }
                            }else{
                                curTriplePtr= nextPtr;
                            }
                            break;
                        }
                        case 3: //(?, p, o) 
                        {
                            if(nextPtr != INDEX_END_PTR){
                                auto & row = rdfIndex.tripleTable[nextPtr];
                                if(row.Rp == (*p_term).value){
                                   curTriplePtr = nextPtr;
                                   nextPtr=row.nextptr[rdfIndex.Nop];
                                }else{
                                    curTriplePtr = INDEX_END_PTR;
                                    nextPtr=INDEX_END_PTR;
                                }  
                            }else{
                                curTriplePtr= nextPtr;
                            }
                            break;
                        }
                        case 4:  // symmetric to case 3
                        {
                            if(nextPtr != INDEX_END_PTR){
                                auto & row = rdfIndex.tripleTable[nextPtr];
                                if(row.Rp == (*p_term).value){
                                   curTriplePtr = nextPtr;
                                   nextPtr=row.nextptr[rdfIndex.Nsp];
                                }else{
                                    curTriplePtr = INDEX_END_PTR;
                                    nextPtr=INDEX_END_PTR;
                                }  
                            }else{
                                curTriplePtr= nextPtr;
                            }
                            
                            break;
                        }
                        case 5://(?, ?, o) similar to case 1
                        {
                            if(nextPtr != INDEX_END_PTR){  //op-list

                                auto & row = rdfIndex.tripleTable[nextPtr];
                                
                                while(nextPtr !=INDEX_END_PTR &&
                                    (*s_term).name == (*p_term).name&&
                                    row.Rs != row.Rp) {
                                    nextPtr = row.nextptr[rdfIndex.Nop];
                                    row = rdfIndex.tripleTable[nextPtr];
                                }
                                
                                curTriplePtr = nextPtr;
                                nextPtr = row.nextptr[rdfIndex.Nop];
                                // if(nextPtr!=INDEX_END_PTR)
                                //     nextPtr = row.nextptr[rdfIndex.Nop];
                                
                            }else{
                                curTriplePtr= nextPtr;
                            }
                            break;
                        }
                        case 6: // //(s, ?, ?) symmetric to case 5
                        {
                            if(nextPtr != INDEX_END_PTR){  //sp-list

                                auto & row = rdfIndex.tripleTable[nextPtr];
                                
                                while(nextPtr !=INDEX_END_PTR &&
                                    (*p_term).name == (*o_term).name&&
                                    row.Rp != row.Ro) {
                                    nextPtr = row.nextptr[rdfIndex.Nsp];
                                    row = rdfIndex.tripleTable[nextPtr];
                                }
                                
                                curTriplePtr = nextPtr;
                                nextPtr = row.nextptr[rdfIndex.Nsp];
                            }else{
                                curTriplePtr= nextPtr;
                            }
                            break;
                        }
                        case 7: //(?, p, ?) // symetric to case 6
                        {
                            if(nextPtr != INDEX_END_PTR){ 
                                auto & row = rdfIndex.tripleTable[nextPtr];
                                
                                while(nextPtr !=INDEX_END_PTR &&
                                    (*s_term).name == (*o_term).name&&
                                    row.Rs != row.Ro) {
                                    nextPtr = row.nextptr[rdfIndex.Np];
                                    row = rdfIndex.tripleTable[nextPtr];
                                }
                                
                                curTriplePtr = nextPtr;
                                nextPtr = row.nextptr[rdfIndex.Np]; 

                        
                            }else{
                                curTriplePtr= nextPtr;
                            }
                            break;
                        }
                        case 8:
                        {
                            if(nextPtr != INDEX_END_PTR){
                                auto index_size=rdfIndex.getTablesize();
                                auto & row = rdfIndex.tripleTable[nextPtr];
                                while(index_size >nextPtr &&(
                                        ((*s_term).name == (*p_term).name && row.Rs != row.Rp ) || // (x,x,y)
                                        ((*p_term).name == (*o_term).name && row.Rp != row.Ro ) ||    //(x, y, y)
                                        ((*s_term).name == (*o_term).name && row.Rs != row.Ro ) //(x, y, x)
                                        )
                                    ){ 
                                    row = rdfIndex.tripleTable[++nextPtr];   
                                }
                                
                                if(nextPtr>=rdfIndex.getTablesize()){
                                    nextPtr = INDEX_END_PTR;
                                    curTriplePtr=INDEX_END_PTR;
                                }else{
                                    curTriplePtr = nextPtr;
                                    ++nextPtr;
                                }
                        
                            }else{
                                curTriplePtr= nextPtr;
                            }
                            break;
                        }
                        default:
                        {   
                            curTriplePtr = -1;
                            nextPtr = -1;
                            //current pointing to non index...
                            break;
                        }

                        
                    }
                    
                    return *this;
                }






//version 1 evaluate without iterator 
// tempIterator RdfIndex::evaluate2(Term & s_term, Term & p_term, Term & o_term){ //TASK put appropriate type

//     tempIterator result;
//     //bindings

//     //(s, p, o) 
//     if( !s_term.isVariable() && !p_term.isVariable() && !o_term.isVariable() ){
//         tuple<id_t, id_t, id_t> tempT = make_tuple(s_term.value, p_term.value, o_term.value);
//         if(Ispo.find(tempT) != Ispo.end()){
//             triple_pointer_t T = Ispo[tempT];
//             auto row = tripleTable[T];
//             result.push_back(RdfIndex::mapVariables(row, s_term, p_term, o_term));
//         }

//     }
//     //(s, ?, o) 
//     else if( !s_term.isVariable() && p_term.isVariable() && !o_term.isVariable() ){

//         if(sp_linked_list_size < op_linked_list_size){ // if sp-list is shorter
//             if(Is.find(s_term.value) != Is.end()){
//                 triple_pointer_t T = Is[s_term.value];

//                 while(T != -1){ //traverslse ip list
//                     auto row = tripleTable[T];
//                     T = tripleTable[T].nextptr[Nsp];
//                     if(row.Ro != o_term.value) continue;
//                     result.push_back(RdfIndex::mapVariables(row, s_term, p_term, o_term));
//                 }
//             }
            

//         }else{ // else if op list is shorter

//             if(Io.find(o_term.value) != Io.end()){
//                 triple_pointer_t T = Io[o_term.value];

//                 while(T != -1){ //traverslse ip list
//                     auto row = tripleTable[T];
//                     T = tripleTable[T].nextptr[Nop];
//                     if(row.Rs != s_term.value) continue;
//                     result.push_back(RdfIndex::mapVariables(row, s_term, p_term, o_term));
//                 }
//             }


//         }


//     }
//     //(?, p, o) 
//     else if(s_term.isVariable() && !p_term.isVariable() && !o_term.isVariable() ){
//         tuple<id_t, id_t> op_key_index=make_tuple(o_term.value,p_term.value);
        
//         if(Iop.find(op_key_index) != Iop.end()) {
//             triple_pointer_t T = Iop[op_key_index];
//             auto row = tripleTable[T];

//             while(row.Rp == p_term.value){ //traverslse ip list until Rp!=p
//                 result.push_back(RdfIndex::mapVariables(row, s_term, p_term, o_term));
//                 T = tripleTable[T].nextptr[Nop];
//                 if (T == -1) break;
//                 row = tripleTable[T];
//             }
//         }

//     }
//     //(s, p, ?) 
//     else if(!s_term.isVariable() && !p_term.isVariable()&& o_term.isVariable()){
        
//         tuple<id_t, id_t> sp_key_index=make_tuple(s_term.value,p_term.value);
        
//         if(Isp.find(sp_key_index) != Isp.end()) {
//             triple_pointer_t T = Isp[sp_key_index];
//             auto row = tripleTable[T];

//             while(row.Rp ==p_term.value){ //traverslse sp list until Rp!=p or T=-1
//                 result.push_back(RdfIndex::mapVariables(row, s_term, p_term, o_term));
//                 T = tripleTable[T].nextptr[Nsp];
//                 if (T == -1) break;
//                 row = tripleTable[T];
//             }
//         }

//     }
//     // //(?, ?, o)
//     else if(s_term.isVariable() && p_term.isVariable() && !o_term.isVariable() ){

//         if(Io.find(o_term.value) != Io.end()){
//             triple_pointer_t T = Io[o_term.value];

//             while(T != -1){ //traverslse ip list
//                 auto row = tripleTable[T];
//                 T = tripleTable[T].nextptr[Nop];
//                 if(s_term.name == p_term.name)
//                     if(row.Rs != row.Rp) continue;
//                 result.push_back(RdfIndex::mapVariables(row, s_term, p_term, o_term));
//             }
//         }
        

//     }
//     //(s, ?, ?)
//     else if(!s_term.isVariable() && p_term.isVariable() && o_term.isVariable()){
//         if(Is.find(s_term.value) != Is.end()){
//             triple_pointer_t T = Is[s_term.value];

//             while(T != -1){ //traverslse ip list
//                 auto row = tripleTable[T];
//                 T = tripleTable[T].nextptr[Nsp];
//                 if(p_term.name == o_term.name)
//                     if(row.Rp != row.Ro) continue;
//                 result.push_back(RdfIndex::mapVariables(row, s_term, p_term, o_term));
//             }
//         }

//     }
//     //(?, p, ?)
//     else if(s_term.isVariable() && !p_term.isVariable() && o_term.isVariable()){

//         if(Ip.find(p_term.value) != Ip.end()){
//             triple_pointer_t T = Ip[p_term.value];

//             while(T != -1){ //traverslse p list
//                 auto row = tripleTable[T];
//                 T = tripleTable[T].nextptr[Np];
//                 if(s_term.name == o_term.name)
//                     if(row.Rs != row.Ro) continue;
//                 result.push_back(RdfIndex::mapVariables(row, s_term, p_term, o_term));
//             }
//         }

//     }
//     //(?, ?, ?)
//     else if(s_term.isVariable() && p_term.isVariable() && o_term.isVariable()){
//         for(auto & row: tripleTable){
//             if(s_term.name == p_term.name)
//                 if(row.Rs != row.Rp) continue;
            
//             if(p_term.name == o_term.name)
//                 if(row.Rp != row.Ro) continue;

//             if(s_term.name == o_term.name)
//                 if(row.Rs != row.Ro) continue;
            
//             result.push_back(RdfIndex::mapVariables(row, s_term, p_term, o_term));

//         }

//     }
//     return result;
// }

