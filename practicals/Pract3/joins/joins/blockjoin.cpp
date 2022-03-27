#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/minirel.h"
#include "../include/heapfile.h"
#include "../include/scan.h"
#include "../include/join.h"
#include "../include/relation.h"
#include "../include/bufmgr.h"

//---------------------------------------------------------------
// Each join method takes in at least two parameters :
// - specOfS
// - specOfR
//
// They specify which relations we are going to join, which 
// attributes we are going to join on, the offsets of the 
// attributes etc.  specOfS specifies the inner relation while
// specOfR specifies the outer one.
//
//You can use MakeNewRecord() to create the new result record.
//
// Remember to clean up before exiting by "delete"ing any pointers
// that you "new"ed.  This includes any Scan/BTreeFileScan that 
// you have opened.
//---------------------------------------------------------------

void BlockNestedLoopJoin(JoinSpec specOfR, JoinSpec specOfS, int B, long& pinRequests, long& pinMisses, double& duration)
{

    clock_t begin = clock();
	Status s = OK;
        HeapFile* outputRelation = new HeapFile(NULL,s);
        if (s != OK) cerr << "ERROR:Failed to create a file for the joined relations.\n";


        int recLenR = specOfR.recLen;
        int recLenS = specOfS.recLen;
        int recLenOut = specOfR.recLen + specOfS.recLen;

        char * recRBlock = new char [B];
        char * recR = new char[recLenR];
        char * recS = new char[recLenS];
        char * recOut = new char[recLenOut];
        int recsNumInB = B / recLenR;
        bool EOF_R = false;

        RecordID ridR,
                 ridS,
		 ridOut;
        Scan * S;
        Scan * R = specOfR.file->OpenScan(s);
        if (s != OK) cerr << "ERROR:Failed to open a R relation for scan .\n";
        while(!EOF_R){
                char * recRBlock = new char [B];
                for (int i = 0; i < recsNumInB; i++){
                        if (R->GetNext(ridR, recRBlock + i*recLenR, recLenR) !=OK){
                                EOF_R = true; break;
                        }
                }
                S = specOfS.file->OpenScan(s);
                if (s != OK)cerr << "ERROR:Failed to open S relation for scan .\n";
                while(S->GetNext(ridS, recS, recLenS) == OK){
                        int * joinKeyS = (int*)&recS[specOfS.offset];

                        for (int j = 0; j < B; j += recLenR){
                                int * joinKeyR = (int*)&recRBlock[j+specOfR.offset];
                                if (*joinKeyS == *joinKeyR){
                                        MakeNewRecord(recOut, recRBlock+j, recS, recLenR, recLenS);
                                        outputRelation->InsertRecord(recOut, recLenOut, ridOut);
                                }
                        }
                }


        }
        delete [] recR, 
                recS,
                recOut,
                recRBlock;
	delete R, S;
        	

	MINIBASE_BM->GetStat(pinRequests,pinMisses);
	clock_t end = clock();
	duration = float(end - begin)/CLOCKS_PER_SEC;

	delete outputRelation;
}
