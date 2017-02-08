#include "MemoryManager.h"


//Reference: http://www.inf.udec.cl/~leo/Malloc_tutorial.pdf
//https://www.ibm.com/developerworks/aix/tutorials/au-memorymanager/

//Author: Chenyang Li
//2/7/2017
/*
--Based on the idea in the reference, I created a block structure to store size, a 'isFree' flag and a pointer to next.

0)Initialization: link the address with a block and init the variables
1)Allocation: Find a free block with a larger or same size. If the block size is larger, split the block into smaller ones and do the allocation. 
Set the size, flag to 0 and pointer to next
2)Deallocation: Find the target block with the address and set the flag to 1. If multiple blocks are adjacent and free, merge them
3)Check total free space, smallest block, largest block: Traverse the memory pool, check the size and flag to decide.
4)Split, merge: Helper function to manipulate the blocks in the pool

--Cons I think can be improved in this implementation: 

block's size is 12, not efficient enough
deallocate() does not 'delete' the aPointer, after deallocate aPointer can still output *aPointer
my test cases might not cover all the situations.

This is surely not a perfect implementation, there might be more cons and space for improvement.  

--Other notes/thought:
I took a long time on figuring out the ideas than the coding. This is the 1st time I ever tried to code something like this.
After reading and googling, I learnt a lot and found this is a very important and useful in Game Development. However, I did not realize it until now.
This is definitely something I need to improve from now on.


Thanks for the test!
*/

namespace MemoryManager
{
	
	const int MM_POOL_SIZE = 65536;
	char MM_POOL[MM_POOL_SIZE];

	struct block {
		int size;
		int isFree;
		block *next;
	};

	void split(block * slot, int aSize);
	void merge();

	// Initialize any data needed to manage the memory pool
	void initializeMemoryManager(void)
	{
		//init a large chunk of memory
		block *initBlock = (block*)MM_POOL;
		initBlock->size = MM_POOL_SIZE - sizeof(block);
		initBlock->isFree = 1;
		initBlock->next = nullptr;
	}


	// return a pointer inside the memory pool
	// If no chunk can accommodate aSize call OnAllocFail()
	//DIY  new() 
	void* allocate(int aSize)
	{
		block *cur;
		void* pData = nullptr;

		cur = (block*)MM_POOL;
		//traverse
		while ((cur->size < aSize || cur->isFree == 0) && cur->next != nullptr)
		{
			cur = cur->next;			
		}
		//fit
		if (cur->size == aSize)
		{
			cur->isFree = 0;
			pData = (void*)(++cur);			
			return pData;
		}
		//large need to split
		else if (cur->size > aSize + sizeof(block))
		{
			split(cur, aSize);			
			pData = (void*)(++cur);
			
			return pData;
		}
		//pool size is too small
		else
		{
			onOutOfMemory();
			return pData;
		}

	}

	

	// Free up a chunk previously allocated
	//DIY delete(): in fact it's not exactly same. deallocate() flags the block as isFree in MM_POOL, but aPointer still exists.
	void deallocate(void* aPointer)
	{
		if ((MM_POOL <= aPointer) && (aPointer <= MM_POOL + MM_POOL_SIZE))
		{
			block* cur = (block*)aPointer;
			cur--;
			if(cur->isFree == 0)
			{
				cur->isFree = 1;
				//merge multiple adjacant blocks
				merge();
			}
			//deallocate more than once
			else
			{
				onIllegalOperation("Deallocate more than once");
			}
			
		}
		else
		{
			onIllegalOperation("Invalid pointer for deallocation.");
		}
	}


	// Will scan the memory pool and return the total free space remaining
	//free = total - used chunk
	int freeRemaining(void)
	{
		//double check here!!!
		int remain = MM_POOL_SIZE;
		block *cur;
		cur = (block*)MM_POOL;
		while (cur)
		{
			if(!cur->isFree)remain -= (cur->size + sizeof(block));
			cur = cur->next;
		}

		return remain;
	}

	// Will scan the memory pool and return the largest free space remaining

	int largestFree(void) 
	{
		int max = 0;
		block *cur;
		cur = (block*)MM_POOL;
		while (cur)
		{
			if (cur->isFree)
			{
				if (cur->size > max)max = cur->size + sizeof(block);
			}
			cur = cur->next;
		}
		return max;
	}

	// will scan the memory pool and return the smallest free space remaining
	int smallestFree(void)
	{
		int min = MM_POOL_SIZE;
		block *cur;
		cur = (block*)MM_POOL;
		
		while (cur)
		{
			if (cur->isFree)
			{
				if (cur->size < min)min = cur->size + sizeof(block);
			}
			cur = cur->next;
		}
		return min;
	}


	//If an avaible block is larger than the target size, split the block into smaller block. 	
	void split(block * slot, int aSize)
	{
		block* nBlock = (slot + aSize + sizeof(block));
		nBlock->size = slot->size - aSize - sizeof(block);
		nBlock->isFree = 1;
		nBlock->next = slot->next;
		slot->size = aSize;
		slot->isFree = 0;
		slot->next = nBlock;
	}
	
	//When deallocate, multiple adjacent blocks might be free. In this case, merge them into a large block
	void merge()
	{
		block* cur;
		cur = (block*)MM_POOL;
		while (cur && cur->next)
		{
			
				if (cur->isFree && cur->next->isFree)
				{				
					do
					{
						cur->size += cur->next->size + sizeof(block);
						cur->next = cur->next->next;
					} while (cur->next && cur->next->isFree);
				}
				

				cur = cur->next;
			
			
			
		}
	}
	
};

