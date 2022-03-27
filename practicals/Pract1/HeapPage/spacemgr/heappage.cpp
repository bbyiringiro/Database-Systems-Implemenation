#include <iostream>
#include <stdlib.h>
#include <memory.h>

#include "../include/heappage.h"
#include "../include/heapfile.h"
#include "../include/bufmgr.h"
#include "../include/db.h"

using namespace std;

//------------------------------------------------------------------
// Constructor of HeapPage
//
// Input     : Page ID
// Output    : None
//------------------------------------------------------------------

void HeapPage::Init(PageID pageNo)
{
	this->nextPage = INVALID_PAGE;
	this->prevPage = INVALID_PAGE;
	this->numOfSlots = 0;
	this->pid = pageNo;
	this->fillPtr = HEAPPAGE_DATA_SIZE;
	this->freeSpace = HEAPPAGE_DATA_SIZE;
	SLOT_SET_EMPTY(slots[0]);
}

void HeapPage::SetNextPage(PageID pageNo)
{
	this->nextPage = pageNo;
}

void HeapPage::SetPrevPage(PageID pageNo)
{
	this->prevPage = pageNo;
}

PageID HeapPage::GetNextPage()
{
	return this->nextPage;
}

PageID HeapPage::GetPrevPage()
{
	return this->prevPage;
}


//------------------------------------------------------------------
// HeapPage::InsertRecord
//
// Input     : Pointer to the record and the record's length 
// Output    : Record ID of the record inserted.
// Purpose   : Insert a record into the page
// Return    : OK if everything went OK, DONE if sufficient space 
//             does not exist
//------------------------------------------------------------------

Status HeapPage::InsertRecord(char *recPtr, int recLen, RecordID& rid)
{
  if(this->AvailableSpace()<recLen) return DONE; // optmisation
  
  bool freeSlotAvail = false;

  int slotIdx =0;
  // find free slot
  for(slotIdx = 0; slotIdx < this->numOfSlots; ++slotIdx)
    if(SLOT_IS_EMPTY(slots[slotIdx])){
      freeSlotAvail = true; break;
    }

//case when we need to allocate for new slot space
  if(!freeSlotAvail){
    freeSpace -= sizeof(Slot);
    ++numOfSlots;
  }

  short offset = fillPtr - recLen;
  // update the page
  SLOT_FILL(slots[slotIdx], offset, recLen);
  memcpy(&(data[offset]), recPtr, recLen);
  this->freeSpace -= recLen; 
  this->fillPtr -= recLen;
  // return the page rid
  rid.pageNo = pid;
  rid.slotNo = slotIdx;

  return OK;
}


//------------------------------------------------------------------
// HeapPage::DeleteRecord 
//
// Input    : Record ID
// Output   : None
// Purpose  : Delete a record from the page
// Return   : OK if successful, FAIL otherwise  
//------------------------------------------------------------------ 

Status HeapPage::DeleteRecord(const RecordID& rid)
{
  if(rid.slotNo >= numOfSlots || SLOT_IS_EMPTY(slots[rid.slotNo])) return FAIL;

  // improvement part----
  short lengthToDel = slots[rid.slotNo].length;
  short offsetToDel = slots[rid.slotNo].offset;
  memmove(&(data[fillPtr + lengthToDel]), &(data[fillPtr]), offsetToDel - fillPtr);
  for(int slotIdx = 0; slotIdx < numOfSlots; ++slotIdx){
    if(!SLOT_IS_EMPTY(slots[slotIdx]) && slots[slotIdx].offset < offsetToDel){
      slots[slotIdx].offset += lengthToDel;
    }
  }
  this->fillPtr +=lengthToDel;
  this->freeSpace +=lengthToDel;

  
  
  SLOT_SET_EMPTY(slots[rid.slotNo]);
  // free slot if it's the last one
  if(rid.slotNo == numOfSlots - 1){
    this->freeSpace += sizeof(Slot);
    this->numOfSlots--;
  }

	return OK;
}






//------------------------------------------------------------------
// HeapPage::FirstRecord
//
// Input    : None
// Output   : record id of the first record on a page
// Purpose  : To find the first record on a page
// Return   : OK if successful, DONE otherwise
//------------------------------------------------------------------

Status HeapPage::FirstRecord(RecordID& rid)
{
	if(IsEmpty()) return DONE; // optimisation

  int slotIdx = 0;
  for(int slotIdx = 0; slotIdx < numOfSlots; ++slotIdx)
    if(!SLOT_IS_EMPTY(slots[slotIdx])){
      rid.pageNo = pid;
      rid.slotNo = slotIdx;
      return OK;
    }

  return DONE;
}


//------------------------------------------------------------------
// HeapPage::NextRecord
//
// Input    : ID of the current record
// Output   : ID of the next record
// Return   : Return DONE if no more records exist on the page; 
//            otherwise OK
//------------------------------------------------------------------

Status HeapPage::NextRecord (RecordID curRid, RecordID& nextRid)
{ 
  // uses FAQ idea
  if( curRid.slotNo+1 >= numOfSlots) return DONE; 
  for(int curSlot =curRid.slotNo+1; curSlot < numOfSlots; ++curSlot){
    if(SLOT_IS_EMPTY(slots[curSlot])) continue;
    nextRid.pageNo = this->pid;
    nextRid.slotNo = curSlot;
    return OK;
  }
  return DONE;
}


//------------------------------------------------------------------
// HeapPage::GetRecord
//
// Input    : Record ID
// Output   : Records length and a copy of the record itself
// Purpose  : To retrieve a _copy_ of a record with ID rid from a page
// Return   : OK if successful, FAIL otherwise
//------------------------------------------------------------------

Status HeapPage::GetRecord(RecordID rid, char *recPtr, int& recLen)
{
	if(rid.slotNo >= numOfSlots) return FAIL;
  if(SLOT_IS_EMPTY(slots[rid.slotNo])) return FAIL;


  recLen = slots[rid.slotNo].length; 
  memcpy(recPtr, 
         &(data[slots[rid.slotNo].offset]), 
         recLen);
  return OK;
}


//------------------------------------------------------------------
// HeapPage::ReturnRecord
//
// Input    : Record ID
// Output   : pointer to the record, record's length
// Purpose  : To output a _pointer_ to the record
// Return   : OK if successful, FAIL otherwise
//------------------------------------------------------------------

Status HeapPage::ReturnRecord(RecordID rid, char*& recPtr, int& recLen)
{
	if( rid.slotNo > numOfSlots) return FAIL;
  if(SLOT_IS_EMPTY(slots[rid.slotNo])) return FAIL;



  // update the references
  recLen = slots[rid.slotNo].length;
  recPtr = &data[slots[rid.slotNo].offset]; 
  return OK;
}


//------------------------------------------------------------------
// HeapPage::AvailableSpace
//
// Input    : None
// Output   : None
// Purpose  : To return the amount of available space
// Return   : The amount of available space on the heap file page.
//------------------------------------------------------------------

int HeapPage::AvailableSpace(void)
{
  // take into account when free slots are available
  return (numOfSlots > GetNumOfRecords()) ? freeSpace : freeSpace - sizeof(Slot);
}


//------------------------------------------------------------------
// HeapPage::IsEmpty
// 
// Input    : None
// Output   : None
// Purpose  : Check if there is any record in the page.
// Return   : true if the HeapPage is empty, and false otherwise.
//------------------------------------------------------------------

bool HeapPage::IsEmpty(void)
{
	return GetNumOfRecords() == 0;
}

int HeapPage::GetNumOfRecords()
{
	int slotCount = 0;
  for(int slotIdx =0; slotIdx < this->numOfSlots; ++slotIdx)
    if(!SLOT_IS_EMPTY(slots[slotIdx])) slotCount++;

  return slotCount;
}


void HeapPage::CompactSlotDir()
{
  // go through the list linearly making putting non empty slots contigously
  int staleSlotNum = 0;
  for(int slotIdx = 0; slotIdx < numOfSlots; slotIdx++)
    if(SLOT_IS_EMPTY(slots[slotIdx])) ++staleSlotNum; 
    else
      slots[slotIdx - staleSlotNum]= {slots[slotIdx].offset, 
                                      slots[slotIdx].length};
  freeSpace += sizeof(Slot) * staleSlotNum;
  numOfSlots -= staleSlotNum; 
}


