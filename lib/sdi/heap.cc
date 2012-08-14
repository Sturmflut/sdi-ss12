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

extern char __heap_start[];
extern char  __heap_end[];
extern void* __heap_ptr;

static const int	BLOCK_SIZE = 512;
static int		NUM_SLOTS;

static int addr_to_slot(void* addr)
{
	return ((char*)addr - (char*)__heap_start) / BLOCK_SIZE;
}

static bool slot_istaken(int i)
{
	return __heap_start[i] & 1;
}

static void slot_settaken(int i)
{
	__heap_start[i] |= 1;
}

static void slot_cleartaken(int i)
{
	__heap_start[i] &= ~(1);
}

static bool slot_nexttaken(int i)
{
	return __heap_start[i] & 2;
}

static void slot_setnexttaken(int i)
{
	__heap_start[i] |= 2;
}

static void slot_clearnexttaken(int i)
{
	__heap_start[i] &= ~(2);
}


void* alloc (L4_Word_t size) 
{
	if (__heap_ptr==NULL)
		bailout ("__heap_ptr not initialized check *-crt0.S , halted");

	// Initialize heap and bitmap
	//printf("Thread %lx __heap_start %p __heap_end %p __heap_ptr %p\n", &__heap_start, &__heap_end, __heap_ptr);
	if(&__heap_start == __heap_ptr)
	{
		memset(&__heap_start, 0, (char*)&__heap_end - (char*)&__heap_start);

		__heap_ptr = (void*)(__heap_ptr + (((char*)&__heap_end - (char*)&__heap_start) / BLOCK_SIZE));
		NUM_SLOTS = ((char*)&__heap_end - (char*)__heap_ptr) / BLOCK_SIZE;
		//printf("Thread %lx zeroing heap, %i slots, __heap_start %p, __heap_ptr %p\n", L4_Myself().raw, NUM_SLOTS, &__heap_start, __heap_ptr);
	}

	// Scan for a big enough piece
	for(char* p = (char*)__heap_ptr; p < ((char*) &__heap_end); p += BLOCK_SIZE)
	{
		int slot = addr_to_slot(p);
		bool free = true;
		
		for(int i = slot; free &&
			( (i - slot) < ((size / BLOCK_SIZE) + 1)) &&
			( i < (((char*)&__heap_end - (char*)__heap_ptr) / BLOCK_SIZE)); i++)
			if(slot_istaken(i))
				free = false;

		if(free)
		{
			int j;
			for(j = slot; (j - slot) < ((size / BLOCK_SIZE) + 1); j++)
			{
				slot_settaken(j);
				slot_setnexttaken(j);
				printf("Thread %lx, slot %i value %i\n", L4_Myself().raw, j, *((char*)__heap_start + j));
			}

			slot_settaken(++j);
			//printf("Thread %lx, slot %i value %i\n", L4_Myself().raw, j, *((char*)__heap_start + j));

			//printf("Thread %lx handing out %p for request size %i, slot %i\n", L4_Myself().raw, p, size, slot);
			return p;
		}
	}

	bailout ("No heap left");
}

void free (void* freeptr) {
	int i = addr_to_slot(freeptr);

	if(!slot_istaken(i))
	{
		//printf("Thread %lx couldn't free %p, slot value %i\n", L4_Myself().raw, freeptr, *((char*)__heap_start + i));
		bailout("free() called on untaken slot");
	}

	for(; slot_istaken(i) && slot_nexttaken(i); i++)
	{
		slot_cleartaken(i);
		slot_clearnexttaken(i);
	}

	slot_cleartaken(++i);
	
	//printf("Thread %lx freed %i slots after %p\n", L4_Myself().raw, i - addr_to_slot(freeptr), freeptr);	
}

