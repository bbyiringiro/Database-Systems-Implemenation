
#include "../include/bufmgr.h"
#include "../include/frame.h"


// TASK
// RLU and clock

// --------------------------------------------------------------------
// Constructor for BufMgr

// Input   : bufSize  - number of pages in the this buffer manager
// Output  : None
// PostCond: All frames are empty.
// --------------------------------------------------------------------

BufMgr::BufMgr( int bufSize )
{
	// allocate the frames in the heap memory

	this->numOfBuf = bufSize;
	this->frames = new Frame*[bufSize];
	for (int i=0; i<this->numOfBuf; ++i) {
		this->frames[i] = new Frame();
		this->frames[i]->EmptyIt();
	}

	this->replacer = new LRU(this->numOfBuf,this->frames);


	this->ResetStat();

	
}


//--------------------------------------------------------------------
// Destructor for BufMgr
//
// Input   : None
// Output  : None
//--------------------------------------------------------------------

BufMgr::~BufMgr()
{   

	for(int i = 0; i < this->numOfBuf; i++){
		if(frames[i]->IsDirty()) FlushPage(frames[i]->GetPageID());
		delete frames[i];
	}


	delete[] this->frames;
	delete this->replacer;

}

//--------------------------------------------------------------------
// BufMgr::PinPage
//
// Input    : pid     - page id of a particular page 
//            isEmpty - (optional, default to false) if true indicate
//                      that the page to be pinned is an empty page.
// Output   : page - a pointer to a page in the buffer pool. (NULL
//            if fail)
// Purpose  : Pin the page with page id = pid to the buffer.  
//            Read the page from disk unless isEmpty is true or unless
//            the page is already in the buffer.
// Condition: Either the page is already in the buffer, or there is at
//            least one frame available in the buffer pool for the 
//            page.
// PostCond : The page with page id = pid resides in the buffer and 
//            is pinned. The number of pin on the page increase by
//            one.
// Return   : OK if operation is successful.  FAIL otherwise.
//--------------------------------------------------------------------


Status BufMgr::PinPage(PageID pid, Page*& page, bool isEmpty)
{

	++totalCall;

	int frameNumber = FindFrame(pid);
	if(frameNumber != INVALID_FRAME){ // when buffer pool contains the Pageno
		++totalHit; // page hit
		this->frames[frameNumber]->Pin();
		page = this->frames[frameNumber]->GetPage();
		return OK;
	}
	else{

		// select a victim frame
		frameNumber = this->replacer->PickVictim();
		if(frameNumber != INVALID_FRAME){
			
			//if dirty write to disk 
			if(frames[frameNumber]->GetPageID() != INVALID_PAGE)
				if(FlushPage(frames[frameNumber]->GetPageID()) == FAIL) {
					return FAIL; 
				}

			frames[frameNumber]->EmptyIt();
			if(isEmpty) frames[frameNumber]->SetPageID( pid); // just only update the ID
			else if(frames[frameNumber]->Read(pid) != OK) return FAIL; // read the page to buffer


			frames[frameNumber]->Pin();
			// return the address of the page
			page = frames[frameNumber]->GetPage(); 
			return OK;
		}
			// couldn't evict a page
			return FAIL; 
		
	}

} 



//--------------------------------------------------------------------
// BufMgr::UnpinPage
//
// Input    : pid     - page id of a particular page 
//            dirty   - indicate whether the page with page id = pid
//                      is dirty or not. (Optional, default to false)
// Output   : None
// Purpose  : Unpin the page with page id = pid in the buffer. Mark 
//            the page dirty if dirty is true.  
// Condition: The page is already in the buffer and is pinned.
// PostCond : The page is unpinned and the number of pin on the
//            page decrease by one. 
// Return   : OK if operation is successful.  FAIL otherwise.
//--------------------------------------------------------------------


Status BufMgr::UnpinPage(PageID pid, bool dirty)
{
	int frameNumber = FindFrame(pid);

	if(frameNumber == INVALID_FRAME || frames[frameNumber]->NotPinned()) return FAIL;

	frames[frameNumber]->Unpin();

	// if using Clock
	// if(frames[frameNumber]->NotPinned()){
	// 	hashTable[frameNumber]=true;
	// }
	

	if(dirty) frames[frameNumber]->DirtyIt();
	return OK;
}

//--------------------------------------------------------------------
// BufMgr::NewPage
//
// Input    : howMany - (optional, default to 1) how many pages to 
//                      allocate.
// Output   : firstPid  - the page id of the first page (as output by
//                   DB::AllocatePage) allocated.
//            firstPage - a pointer to the page in memory.
// Purpose  : Allocate howMany number of pages, and pin the first page
//            into the buffer. 
// Condition: howMany > 0 and there is at least one free buffer space
//            to hold a page.
// PostCond : The page with page id = pid is pinned into the buffer.
// Return   : OK if operation is successful.  FAIL otherwise.
// Note     : You can call DB::AllocatePage() to allocate a page.  
//            You should call DB:DeallocatePage() to deallocate the
//            pages you allocate if you failed to pin the page in the
//            buffer.
//--------------------------------------------------------------------


Status BufMgr::NewPage (PageID& firstPid, Page*& firstPage, int howMany)
{
	if(howMany<=0 || MINIBASE_DB->AllocatePage(firstPid, howMany) != OK) return FAIL;

	if( PinPage(firstPid, firstPage, true) != OK) // check if there is a free buffer otherwise deallocae
	{
		MINIBASE_DB->DeallocatePage(firstPid, howMany);
		return FAIL;
	}

	return OK;
}

//--------------------------------------------------------------------
// BufMgr::FreePage
//
// Input    : pid     - page id of a particular page 
// Output   : None
// Purpose  : Free the memory allocated for the page with 
//            page id = pid  
// Condition: Either the page is already in the buffer and is pinned
//            no more than once, or the page is not in the buffer.
// PostCond : The page is unpinned, and the frame where it resides in
//            the buffer pool is freed.  Also the page is deallocated
//            from the database. 
// Return   : OK if operation is successful.  FAIL otherwise.
// Note     : You can call MINIBASE_DB->DeallocatePage(pid) to
//            deallocate a page.
//--------------------------------------------------------------------


Status BufMgr::FreePage(PageID pid)
{

	int frameNumber = FindFrame(pid);

	if( frameNumber == INVALID_FRAME || frames[frameNumber]->GetPinCount() <= 1)
	{
		if(frameNumber != INVALID_FRAME ) frames[frameNumber]->EmptyIt();

		MINIBASE_DB->DeallocatePage(pid);
		return OK;
	}

	return FAIL;
}


//--------------------------------------------------------------------
// BufMgr::FlushPage
//
// Input    : pid  - page id of a particular page 
// Output   : None
// Purpose  : Flush the page with the given pid to disk.
// Condition: The page with page id = pid must be in the buffer,
//            and is not pinned. pid cannot be INVALID_PAGE.
// PostCond : The page with page id = pid is written to disk if it's dirty. 
//            The frame where the page resides is empty.
// Return   : OK if operation is successful.  FAIL otherwise.
//--------------------------------------------------------------------


Status BufMgr::FlushPage(PageID pid)
{

	int frameNumber = FindFrame(pid);

	if( pid == INVALID_PAGE || frameNumber == INVALID_FRAME) return FAIL;
	if(!frames[frameNumber]->NotPinned()) return FAIL;

	Status flushSuccess = OK;

	if(frames[frameNumber]->IsDirty())
	{
		flushSuccess = MINIBASE_DB->WritePage(frames[frameNumber]->GetPageID(), frames[frameNumber]->GetPage());
		++numDirtyPageWrites;
	}
	if (flushSuccess)
		frames[frameNumber]->EmptyIt();

	return flushSuccess;
}

//--------------------------------------------------------------------
// BufMgr::FlushAllPages
//
// Input    : None
// Output   : None
// Purpose  : Flush all pages in this buffer pool to disk.
// Condition: All pages in the buffer pool must not be pinned.
// PostCond : All dirty pages in the buffer pool are written to 
//            disk (even if some pages are pinned). All frames are empty.
// Return   : OK if operation is successful.  FAIL otherwise.
//--------------------------------------------------------------------

Status BufMgr::FlushAllPages()
{

	Status status = OK;
	for(int i = 0; i < numOfBuf; ++i)
	{
		if(!frames[i]->NotPinned()) status = FAIL;
		// flashes them to the disk if they are pinned
		if( frames[i]->IsDirty())
			if(MINIBASE_DB->WritePage(frames[i]->GetPageID(), frames[i]->GetPage()) == FAIL) status = FAIL;

		frames[i]->EmptyIt();
	}

	return status;
}


//--------------------------------------------------------------------
// BufMgr::GetNumOfUnpinnedFrames
//
// Input    : None
// Output   : None
// Purpose  : Find out how many unpinned locations are in the buffer
//            pool.
// Condition: None
// PostCond : None
// Return   : The number of unpinned buffers in the buffer pool.
//--------------------------------------------------------------------

unsigned int BufMgr::GetNumOfUnpinnedFrames()
{

	unsigned int count = 0;

	for(int i = 0; i < numOfBuf; i++)
		if(frames[i]->NotPinned()) ++count;
	
	return count;
}

void  BufMgr::PrintStat() {
	cout<<"**Buffer Manager Statistics**"<<endl;
	cout<<"Number of Dirty Pages Written to Disk: "<<numDirtyPageWrites<<endl;
	cout<<"Number of Pin Page Requests: "<<totalCall<<endl;
	cout<<"Number of Pin Page Request Misses "<<totalCall-totalHit<<endl;
}

//--------------------------------------------------------------------
// BufMgr::FindFrame
//
// Input    : pid - a page id 
// Output   : None
// Purpose  : Look for the page in the buffer pool, return the frame
//            number if found.
// PreCond  : None
// PostCond : None
// Return   : the frame number if found. INVALID_FRAME otherwise.
//--------------------------------------------------------------------

int BufMgr::FindFrame( PageID pid )
{

	for(int i=0; i<this->numOfBuf; ++i){
		if(this->frames[i]->GetPageID() == pid) return i;
	}

	return INVALID_FRAME;
}

