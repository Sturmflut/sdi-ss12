#include <l4io.h>
#include <elf.h>
#include <sdi/sdi.h>
#include <if/iffileserver.h>
#include <if/ifmemoryserver.h>

#include "taskserver.h" 
#include "root.h"


L4_Word_t last_task_id;

L4_ThreadId_t last_thread_id = L4_nilthread;


L4_ThreadId_t memoryserverid = L4_nilthread;
L4_ThreadId_t fileserverid = L4_nilthread;

void taskserver_init() {
    /* Announce task service */
    log_printf(loggerid, "[TASK] Registering");
    nameserver_register("/task");
    log_printf(loggerid, "[TASK] Registered...");
    
    // arbitrary first task_id (should be high enough to not collide)
    last_task_id = 1;
    
    while (L4_IsNilThread(memoryserverid)) {
        memoryserverid = nameserver_lookup("/server/memory");
    }
    while (L4_IsNilThread(fileserverid)) {
        fileserverid = nameserver_lookup("/file");
    }
    
    log_printf(loggerid,"[TASK] Found memoryserver = %x, fileserver = %x", memoryserverid.raw, fileserverid.raw);
}

L4_ThreadId_t taskserver_create_task_real(CORBA_Object  _caller, const path_t  path, const path_t  cmdline, idl4_server_environment * _env) {
    L4_ThreadId_t threadid = (create_thread_id(last_task_id++, 0));
    
    /* First ThreadControl to setup initial thread */
    if (!L4_ThreadControl (threadid, threadid, L4_Myself (), L4_nilthread, (void*)-1UL)) {
        panic ("ThreadControl failed\n");
    }

    L4_Word_t dummy;
    if (!L4_SpaceControl (threadid, 0, L4_FpageLog2 (0xB0000000,14), 
                utcbarea, L4_anythread, &dummy)) {
        panic ("SpaceControl failed\n");
    }

    /* Second ThreadControl, activate thread */
    if (!L4_ThreadControl (threadid, threadid, L4_nilthread, memoryserverid, 
                (void*)L4_Address (utcbarea))) {
        panic ("ThreadControl failed\n");
    }

    // TODO: proper exception handling! (file not found, file invalid)
    L4_Word_t file_id = IF_FILESERVER_get_file_id(fileserverid, path, &env);
    if (file_id == -1) {
        panic("ELF file does not exist");
    }
    
	buf_t buff;
	char tbuff[1024];
	buff._buffer = (CORBA_char*)&tbuff;
	buff._maximum = 1024;

    log_printf(loggerid, "[TASK] Loading file: %s\n", path);
    

    L4_Word_t res_read = IF_FILESERVER_read(fileserverid, file_id, 0, sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr), &buff, &env);
    
    Elf32_Ehdr *hdr = (Elf32_Ehdr *)tbuff;
    Elf32_Phdr *phdr = get_elf_phdr(hdr);
    if (phdr == NULL) {
        panic("Invalid ELF file");
    }

    log_printf(loggerid, "[TASK] ELF decoded. Number of PHs: %d", hdr->e_phnum);

    for (int i = 0; i < hdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            log_printf(loggerid, "[TASK] Sending section: path = %s, offset = %p\nvaddr = %p, size = %d, realsize = %d",
                    path, phdr[i].p_offset, phdr[i].p_vaddr, phdr[i].p_filesz + phdr[i].p_memsz, phdr[i].p_filesz);

            IF_MEMORYSERVER_map_file_pages(
                    (CORBA_Object)memoryserverid,
                    &threadid,
                    0,
                    path,
                    phdr[i].p_offset,
                    phdr[i].p_vaddr,
                    phdr[i].p_filesz + phdr[i].p_memsz,
                    phdr[i].p_filesz,
                    &env);
        }
    }

    // TODO: we have to define a stack (use anon mapping) instead of using the stack in ia32-crt.S
    IF_MEMORYSERVER_startup((CORBA_Object)memoryserverid, &threadid, (L4_Word_t)hdr->e_entry, 0, &env);
    
    return threadid;
}
