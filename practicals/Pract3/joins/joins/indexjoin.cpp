#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/minirel.h"
#include "../include/heapfile.h"
#include "../include/scan.h"
#include "../include/join.h"
#include "../include/btfile.h"
#include "../include/btfilescan.h"
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


void IndexNestedLoopJoin(JoinSpec specOfR, JoinSpec specOfS, long& pinRequests, long& pinMisses, double& duration)
{
    clock_t begin = clock();

	Status s = OK;
    HeapFile* outputRelation = new HeapFile(NULL,s);
    if (s != OK) cerr << "ERROR:Failed to create a file for the joined relations.\n";


    Scan * R = specOfR.file->OpenScan(s);
    if (s != OK) cerr << "ERROR:Failed to open R \n";
            

    Scan * S = specOfS.file->OpenScan(s);
    if (s != OK) cerr << "ERROR:Failed to open S.\n";

    int recLenR = specOfR.recLen;
    int recLenS = specOfS.recLen;
    int recLenOut = specOfR.recLen + specOfS.recLen;


    char * recR = new char[recLenR];
    char * recS = new char[recLenS];
    char * recOut = new char[recLenOut];

    RecordID ridR,
             ridS,
             ridOut;

    // create an index on inner relation
	BTreeFile *btree;
	btree = new BTreeFile (s, "BTree", ATTR_INT, sizeof(int));
	while(OK == S->GetNext(ridS, recS, recLenS)){
		btree->Insert(recS + specOfS.offset, ridS);            
    }


    BTreeFileScan  *btreeScan;

	while(R->GetNext(ridR, recR, recLenR)==OK){
		int * joinKeyR = (int*)&recR[specOfR.offset]; 

		btreeScan = (BTreeFileScan *)btree->OpenScan(joinKeyR, joinKeyR);

		int key;

		while (btreeScan->GetNext(ridS, &key) == OK)
		{
            specOfS.file->GetRecord(ridS, recS, recLenS);
			MakeNewRecord(recOut, recR, recS, recLenR, recLenS);
	   		outputRelation->InsertRecord (recOut, recLenOut, ridOut);
        }


    }
	
	btree->DestroyFile();
	delete btree, 
            btreeScan,
            R, S;
    delete [] recR, recS, recOut;

	MINIBASE_BM->GetStat(pinRequests,pinMisses);

	clock_t end = clock();
	duration = float(end - begin)/CLOCKS_PER_SEC;

	delete outputRelation;
}
