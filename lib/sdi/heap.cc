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
extern char __heap_end[];
extern void* __heap_ptr;

void* alloc (L4_Word_t size) 
{
    if (__heap_ptr==NULL)
	bailout ("__heap_ptr not initialized check *-crt0.S , halted");
    void* ret = __heap_ptr;
    __heap_ptr = (void*)((L4_Word_t)__heap_ptr + size);
    if ((L4_Word_t)__heap_ptr > (L4_Word_t)&__heap_end)
	bailout ("no more heap left, halted");
    return ret;
}

void free (void* freeptr) {
    panic ((char *) "free () is not implemented. Put your code in here.");
}
