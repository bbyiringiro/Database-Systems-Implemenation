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



void TupleNestedLoopJoin(JoinSpec specOfR, JoinSpec specOfS, long& pinRequests, long& pinMisses, double& duration)
{

	clock_t begin = clock();
	Status s;
	Scan *R;
	Scan *S;

	HeapFile *outputRelation;
	
	
	char *recPtrR = new char[specOfR.recLen];
	char *recPtrS = new char[specOfS.recLen];
	char * recOut = new char[specOfR.recLen + specOfS.recLen];
	
	int recLenR = specOfR.recLen;
	int recLenS = specOfS.recLen;
	int recLenOut = specOfR.recLen + specOfS.recLen;

	RecordID ridR,
			 ridS,
			 ridOut;

	R = specOfR.file->OpenScan(s);
	if (s != OK) cerr << "ERROR : Failed to open heapfile of R for scan.\n";


	outputRelation = new HeapFile(NULL, s);
	if (s != OK) cerr << "ERROR : Failed to  create new file\n";



	
	while (R->GetNext(ridR, recPtrR,recLenR) == OK){

		S = specOfS.file->OpenScan(s);
		if (s != OK) cerr << "ERROR : Failed to open heapfile of S for scan.\n";

		while (S->GetNext(ridS, recPtrS, recLenS) == OK){
			
			int* joinKeyR = (int*)&recPtrR[specOfR.offset];			   		             	
			int* joinAKeyS = (int*)&recPtrS[specOfS.offset];
			if (*joinKeyR == *joinAKeyS){
				MakeNewRecord(recOut,recPtrR,recPtrS,recLenR,recLenS);
				outputRelation->InsertRecord (recOut,recLenOut, ridOut); 
			}
		}
		delete S;

	}	
	delete [] recPtrR,
		   recPtrS,
		   recOut;
	delete R;

	MINIBASE_BM->GetStat(pinRequests,pinMisses);
	clock_t end = clock();
	duration =float (end - begin)/CLOCKS_PER_SEC;
  


	delete outputRelation;

}
