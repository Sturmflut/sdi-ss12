//
// File: include/sdi/sdi.h
//
// Description: Header for libsdi
//

#if !defined(__INCLUDE_SDI_SDI_H__)
#define __INCLUDE_SDI_SDI_H__

#include <l4/kdebug.h>
#include <sdi/types.h>


// Heap managment
extern void* alloc (L4_Word_t size);
extern void free (void* freeptr);

// Logging
extern void LogMessage (const char* message);

// Panic managment 
extern void panic (const char* message);

#define bailout(S) L4_KDB_Enter( S )
#define assert(X) if (!(X)) bailout( #X )



#endif /* !__INCLUDE_SDI_SDI_H__ */
