#include "MemoryManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
//clean cout later
#include <iostream>

//Reference: http://www.inf.udec.cl/~leo/Malloc_tutorial.pdf
//https://www.ibm.com/developerworks/aix/tutorials/au-memorymanager/
//

/*
Cons in this implementation: 

block has a size of 12, not efficient enough
deallocate() does not 'delete' the aPointer, after deallocate aPointer can still output *aPointer


*/

namespace MemoryManager
{
	//size is from the note
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
		initBlock->next = NULL;
		
		
		//std::cout << sizeof(struct block)<< std::endl; same as sizeof(block)
	}


	// return a pointer inside the memory pool
	// If no chunk can accommodate aSize call OnAllocFail()
	//DIY  new() 
	void* allocate(int aSize)
	{
		block *cur, *prev;
		void* pData = nullptr;

		cur = (block*)MM_POOL;
		std::cout << (cur)->isFree << std::endl;
		while ((cur->size < aSize || cur->isFree == 0) && cur->next != NULL)
		{
			//prev = cur;
			std::cout << "block" << cur << "not available" << std::endl;
			cur = cur->next;
			
		}
		if (cur->size == aSize)
		{
			cur->isFree = 0;
			pData = (void*)(++cur);
			std::cout << "block" << cur << "fit" << std::endl;
			return pData;
		}
		else if (cur->size > aSize + sizeof(block))
		{

			std::cout << "block" << cur << "too big need to split" << std::endl;
			std::cout << cur->size << std::endl;
			split(cur, aSize);
			std::cout << cur->next->size << std::endl;
			pData = (void*)(++cur);
			
			return pData;
		}
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
			cur->isFree = 1;
			merge();
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
		int remain = MM_POOL_SIZE - sizeof(block);
		block *cur;
		cur = (block*)MM_POOL;
		while (cur)
		{
			if(!cur->isFree)remain -= cur->size + sizeof(block);
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
				if (cur->size > max)max = cur->size;
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
				if (cur->size < min)min = cur->size;
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

