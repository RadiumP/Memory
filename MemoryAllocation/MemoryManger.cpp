#include "MemoryManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

//Reference: http://www.inf.udec.cl/~leo/Malloc_tutorial.pdf


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

	// Initialize any data needed to manage the memory pool
	void initializeMemoryManager(void)
	{
		//init a large chunk of memory
		block *initBlock = (block*)MM_POOL;
		initBlock->size = MM_POOL_SIZE - sizeof(block);
		initBlock->isFree = 1;
		initBlock->next = NULL;
	}


	// return a pointer inside the memory pool
	// If no chunk can accommodate aSize call OnAllocFail()
	//DIY  malloc() 
	void* allocate(int aSize)
	{
		block *cur, *prev;
		void* pData = nullptr;

		cur = 

	}


	// Free up a chunk previously allocated
	//free()
	void deallocate(void* aPointer)
	{

	}
};