#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "include/minirel.h"
#include "include/bufmgr.h"
#include "include/heapfile.h"
#include "include/join.h"
#include "include/relation.h"

#include <fstream>
#include <vector>

int MINIBASE_RESTART_FLAG = 0;// used in minibase part

#define NUM_OF_DB_PAGES  2000 // define # of DB pages
#define NUM_OF_BUF_PAGES 50 // define Buf manager size.You will need to change this for the analysis

// TASKs
/*  Compare the relative performance of the three join algorithms. Record the times
    taken for each algorithm to run, and the number of page misses. */
/*  • Study the effect of the buffer pool size on the algorithms by changing the buffer
    pool size (NUM_OF_BUF_PAGES in main.cpp).*/
/*  • Study the effect of the relation size on the algorithms by changing the number of
    records in the given Employee and Project relations (NUM_OF_REC_IN_R and 
	NUM_OF_REC_IN_S in join.h). */
/* • Submit a report containing documentation, tables and graphs of these statistics,
    together with an analysis of the results. Note that your analysis must be thorough,
    and cover a wide range of buffer pool sizes and relation sizes.  */



int main()
{
	

	std::ofstream outFile1("TupleNestedLoopJoin.txt");
	std::ofstream outFile2("BlockNestedLoopJoin.txt");
	std::ofstream outFile3("IndexNestedLoopJoin.txt");


	outFile1.clear();
	outFile2.clear();
	outFile3.clear();

	Status s;
	int reps = 10;


	for (int rRecSize = 10000; rRecSize <= NUM_OF_REC_IN_R*10; rRecSize+=1000)
	for (int sRecSize = 2500; (sRecSize <= NUM_OF_REC_IN_S*20); sRecSize+=250)
	for (int buffSize = 10; buffSize <= 512; buffSize +=50){


	




	



	//
	// Initialize Minibase's Global Variables
	//

	minibase_globals = new SystemDefs(s, 
		"MINIBASE.DB",
		"MINIBASE.LOG",
		NUM_OF_DB_PAGES,   // Number of pages allocated for database
		500,
		buffSize,  // Number of frames in buffer pool
		NULL);

		
	
	//
	// Initialize random seed
	//

	srand(1);

	//
	// Create Random Relations R(outer relation) and S for joining. The definition is in relation.h, 
	// # of tuples: NUM_OF_REC_IN_R, NUM_OF_REC_IN_S in join.h
	//  
	//

	// cerr << "Creating random records for relation R\n";
	CreateR(rRecSize);
	// cerr << "Creating random records for relation S\n";
	CreateS(sRecSize);

	//
	// Initialize the specification for joins
	//

	JoinSpec specOfS, specOfR;

	CreateSpecForR(specOfR);
	CreateSpecForS(specOfS);

	// 
	// Do your joining here.
	//

	long pinRequests;
	long pinMisses;
	double duration;
	long totalPinrequests=0;
	long totalPinMisses=0;
	double totalDuration=0;
	
	//Tuple join
	for (int i = 0; i < reps ; i++){

		MINIBASE_BM->ResetStat();
		
		TupleNestedLoopJoin(specOfR,specOfS, pinRequests, pinMisses, duration);
		totalPinrequests += pinRequests; 
		totalPinMisses += pinMisses;
		totalDuration += duration;
	}
	// cout << "TupleNestedLoopJoin" << endl << endl;
     cout <<  totalPinrequests/reps << " " << totalPinMisses/reps << " AVG-Time " << totalDuration / reps << endl << endl;
	outFile1 <<rRecSize <<"," <<sRecSize <<"," <<buffSize <<","<< totalPinrequests/reps <<","<< totalPinMisses/reps << "," << totalDuration / reps << endl;


	//Block join
	totalPinrequests = 0; totalPinMisses = 0;totalDuration = 0;
	for (int i = 0; i < reps; i++){
		MINIBASE_BM->ResetStat();
		int B = (MINIBASE_BM->GetNumOfBuffers()-3*3)*MINIBASE_PAGESIZE;
		BlockNestedLoopJoin(specOfR,specOfS, B, pinRequests, pinMisses, duration);	
		totalPinrequests += pinRequests; 
		totalPinMisses += pinMisses;
		totalDuration += duration;
	}
	// cout << "BlockNestedLoopJoin" << endl << endl;
     cout <<  totalPinrequests/reps << " " << totalPinMisses/reps << " AVG-Time " << totalDuration / reps << endl << endl;
	outFile2 <<rRecSize <<"," <<sRecSize <<"," <<buffSize <<","<< totalPinrequests/reps <<","<< totalPinMisses/reps << "," << totalDuration / reps << endl;



	//Index Join

	totalPinrequests = 0; totalPinMisses = 0;totalDuration = 0;
	for (int i = 0; i < reps; i++){
		MINIBASE_BM->ResetStat();
		
		IndexNestedLoopJoin(specOfR,specOfS, pinRequests, pinMisses, duration);	
		totalPinrequests += pinRequests; 
		totalPinMisses += pinMisses;
		totalDuration += duration;
	}

	// cout << "IndexNestedLoopJoin" << endl << endl;
    cout  <<  totalPinrequests/reps << " " << totalPinMisses/reps << " AVG-Time " << totalDuration / reps << endl << endl;
	outFile3 <<rRecSize <<"," <<sRecSize <<"," <<buffSize <<","<< totalPinrequests/reps <<","<< totalPinMisses/reps << "," << totalDuration / reps << endl;


	//delete the created database
    remove("MINIBASE.DB");

	}

	 

	


	return 1;
}
