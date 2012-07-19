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

// Sleeping
extern void sleep (int msec);

#include <idl4glue.h>

#include <if/ifnameserver.h>
#include <if/ifdriverserver.h>

// Nameserver convenience API
extern void nameserver_register(path_t path);
extern void nameserver_deregister();
extern L4_ThreadId_t nameserver_lookup(path_t path);

// Driver server convenience API
extern void driverserver_register(path_t path);
extern void driverserver_deregister();


#define bailout(S) L4_KDB_Enter( S )
#define assert(X) if (!(X)) bailout( #X )



#endif /* !__INCLUDE_SDI_SDI_H__ */
