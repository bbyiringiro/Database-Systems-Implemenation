#include "../include/replacer.h"
#include <sys/time.h>

Replacer::Replacer(){}
Replacer::~Replacer(){}

int Clock::PickVictim( void)
{
	int victim = INVALID_FRAME;

	// int tempStart=(((50)%this->numOfBuf)+this->numOfBuf)%this->numOfBuf;
	
	// while(tempStart !=this->current){

	// 	if((!this->hashTable[hashTable] &&  this->hashTable[current]->NotPinned()) || hashTable[current]->GetPageID() == INVALID_PAGE){
	// 		victim = current;
	// 		this->current = (current+1)%this->numOfBuf;
	// 		return victim;

	// 	}else if(this->hashTable[current] = 1 ){
	// 		this->hashTable[current] =false;
	// 	}

	// 	this->current = (current+1)%this->numOfBuf;
	// }


	return 0;
}
Clock::Clock( int bufSize, Frame **frames, HashTable *hashTable){
	this->hashTable = hashTable;
	this->numOfBuf =bufSize;

}
Clock::~Clock(){}



int LRU::PickVictim( void)
{
	timespec timestamp;
	clock_gettime(CLOCK_REALTIME, &timestamp);
	long lowest = timestamp.tv_nsec;

	int victim = INVALID_FRAME;

	for(int i = 0; i < numOfBuf; i++)
	{
		if( frames[i]->GetPageID() == INVALID_PAGE) return i;

		if( frames[i]->timestamp.tv_nsec < lowest && frames[i]->NotPinned()){
			lowest = frames[i]->timestamp.tv_nsec;
			victim = i;
		}
	}

	return victim;
}

LRU::LRU(int bufSize, Frame** frames)
{
	this->numOfBuf = bufSize;
	this->frames = frames;
}

LRU::~LRU()
{

}