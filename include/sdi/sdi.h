//
// File: include/sdi/sdi.h
//
// Description: Header for libsdi
//

#if !defined(__INCLUDE_SDI_SDI_H__)
#define __INCLUDE_SDI_SDI_H__

#include <l4/kdebug.h>
#include <sdi/types.h>
#include <elf.h>

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

// Nameserver convenience API
void nameserver_register(path_t path);
void nameserver_deregister();
L4_ThreadId_t nameserver_lookup(path_t path);


// Consoleserver convenience API
int console_printf(const char *format, ...);

#define bailout(S) L4_KDB_Enter( S )
#define assert(X) if (!(X)) bailout( #X )

// Taskserver
L4_Word_t get_task_id(L4_ThreadId_t threadid);
L4_Word_t get_thread_count(L4_ThreadId_t threadid);
L4_ThreadId_t create_thread_id(L4_Word_t task_id, L4_Word_t thread_count);

// ELF
Elf32_Phdr* valid_elf_header(Elf32_Ehdr *hdr);

#endif /* !__INCLUDE_SDI_SDI_H__ */
