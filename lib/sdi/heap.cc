//
// File: lib/sdi/heap.cc
//
// Description: VERY simple heap implemenetation 
//

#include <stdlib.h>
#include <l4io.h>

#include <sdi/types.h>
#include <sdi/sdi.h>

/* Heap Managment */

// import symbols from crt0.S

extern void* __heap_start[];
extern void* __heap_end[];
extern void* __heap_ptr;

static const int BLOCK_SIZE = 512;

static int addr_to_slot(void* addr)
{
	return ((char*)addr - (char*)__heap_start) / BLOCK_SIZE;
}

#define	SLOT_ISTAKEN(x)		( (*((char*)__heap_start + x) & 1) == 1 )
#define	SLOT_SETTAKEN(x)	( *((char*)__heap_start + x) |= 1 )
#define	SLOT_CLEARTAKEN(x)	( *((char*)__heap_start + x) &= ~(1) )

#define	SLOT_NEXTTAKEN(x)	( (*((char*)__heap_start + x) & 2) == 2 )
#define	SLOT_SETNEXTTAKEN(x)	( *((char*)__heap_start + x) |= 2 )
#define	SLOT_CLEARNEXTTAKEN(x)	( *((char*)__heap_start + x) &= ~(2) )



void* alloc (L4_Word_t size) 
{
	if (__heap_ptr==NULL)
		bailout ("__heap_ptr not initialized check *-crt0.S , halted");

	// Initialize heap and bitmap
	if(__heap_start == __heap_ptr)
	{
		//printf("Thread %lx zeroing heap\n", L4_Myself().raw);
		for(char* i = (char*) __heap_start; i < (char*) __heap_end; i++)
			*i = 0;

		__heap_ptr = (void*)((L4_Word_t)__heap_ptr + ((__heap_end - __heap_start) / BLOCK_SIZE));
	}

	// Scan for a big enough piece
	for(char* p = (char*)__heap_ptr; p < ((char*) &__heap_end); p += BLOCK_SIZE)
	{
		int slot = addr_to_slot(p);
		bool free = true;
		
		for(int i = slot; free &&
			( (i - slot) < ((size / BLOCK_SIZE) + 1)) &&
			( i < (((char*)__heap_end - (char*)__heap_ptr) / BLOCK_SIZE)); i++)
			if(SLOT_ISTAKEN(i))
				free = false;

		if(free)
		{
			int j;
			for(j = slot; (j - slot) < ((size / BLOCK_SIZE) + 1); j++)
			{
				SLOT_SETTAKEN(j);
				SLOT_SETNEXTTAKEN(j);
				printf("Thread %lx, slot %i value %i\n", L4_Myself().raw, j, *((char*)__heap_start + j));
			}

			SLOT_SETTAKEN(++j);
			//printf("Thread %lx, slot %i value %i\n", L4_Myself().raw, j, *((char*)__heap_start + j));

			//printf("Thread %lx handing out %p for request size %i, slot %i\n", L4_Myself().raw, p, size, slot);
			return p;
		}
	}

	bailout ("No heap left");
}

void free (void* freeptr) {
	int i = addr_to_slot(freeptr);

	if(!SLOT_ISTAKEN(i))
	{
		//printf("Thread %lx couldn't free %p, slot value %i\n", L4_Myself().raw, freeptr, *((char*)__heap_start + i));
		bailout("free() called on untaken slot");
	}

	for(; SLOT_ISTAKEN(i) && SLOT_NEXTTAKEN(i); i++)
	{
		SLOT_CLEARTAKEN(i);
		SLOT_CLEARNEXTTAKEN(i);
	}

	SLOT_CLEARTAKEN(++i);
	
	//printf("Thread %lx freed %i slots after %p\n", L4_Myself().raw, i - addr_to_slot(freeptr), freeptr);	
}

