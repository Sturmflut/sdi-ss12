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
extern int log_printf(L4_ThreadId_t logger_id, const char *format, ...);

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
int console_printf(L4_ThreadId_t consoleid, const char *format, ...);
char console_readchar(L4_ThreadId_t consoleid, char* modifier);
int console_readline(L4_ThreadId_t consoleid, char* buffer, L4_Word_t maxlen);
void console_setcolor(char color);

#define bailout(S) L4_KDB_Enter( S )
#define assert(X) if (!(X)) bailout( #X )

#define ELEM_COUNT(x) (sizeof((x)) / sizeof((x[0])))

// number of threads
#define NUM_T_ENTRY 30

// Taskserver
L4_Word_t get_task_id(L4_ThreadId_t threadid);
L4_Word_t get_thread_count(L4_ThreadId_t threadid);
L4_ThreadId_t create_thread_id(L4_Word_t task_id, L4_Word_t thread_count);

// ELF
Elf32_Phdr* get_elf_phdr(Elf32_Ehdr *hdr);

#endif /* !__INCLUDE_SDI_SDI_H__ */
