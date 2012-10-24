#include <l4io.h>
#include <elf.h>
#include <sdi/sdi.h>

#include <if/iffileserver.h>
#include <if/ifmemoryserver.h>
#include <if/ifconsoleserver.h>

#include "taskserver.h" 
#include "root.h"


L4_ThreadId_t memoryserverid = L4_nilthread;
L4_ThreadId_t fileserverid = L4_nilthread;
L4_ThreadId_t consoleserverid = L4_nilthread;

Task_entry_t taskList[NUM_T_ENTRY];

void taskserver_init() {
    /* Announce task service */
    log_printf(loggerid, "[TASK] Registering");
    nameserver_register("/task");
    log_printf(loggerid, "[TASK] Registered...");
    
    while (L4_IsNilThread(memoryserverid)) {
        memoryserverid = nameserver_lookup("/server/memory");
    }
    while (L4_IsNilThread(fileserverid)) {
        fileserverid = nameserver_lookup("/file");
    }
    while (L4_IsNilThread(consoleserverid)) {
        consoleserverid = nameserver_lookup("/server/console");
    }
    
    for (L4_Word_t i = 0; i < NUM_T_ENTRY; i++) {
        clear_tasklist_entry(i);
    }

    start_init_tasks();
}

void clear_tasklist_entry(L4_Word_t task_id) {
    taskList[task_id].task_exists = false;
    for (int j = 0; j < MAX_THREADS; j++) {
        taskList[task_id].has_thread[j] = false;
    }
}

void start_init_tasks() {
    L4_Word_t tasks_config_file_id = IF_FILESERVER_get_file_id(fileserverid, "tasks.conf", &env);
    if (tasks_config_file_id == -1) {
        log_printf(loggerid, "[TASK] Could not open tasks.conf. Not starting init tasks.");
    }
	
    buf_t buff;
	char tbuff[1024];
	buff._buffer = (CORBA_char*)tbuff;
	buff._maximum = 1024;

    L4_Word_t res_read;
    L4_Word_t read_bytes = 0;

    char line[256];
    
    do {
        res_read = IF_FILESERVER_read(fileserverid, tasks_config_file_id, read_bytes, buff._maximum, &buff, &env);
        read_bytes += res_read;

        int line_index = 0;
        for (int i = 0; i < buff._maximum; i++) {
            line[line_index++] = buff._buffer[i];
            
            if (buff._buffer[i] == '\n') {
                line[line_index - 1] = '\0';
                unsigned int line_length = line_index;
                line_index = 0;

                char task_path[64];
                int task_path_index = 0;
                bool task_found = false;
                int console_nr;
                
                // line contains a full line now (without '\n')
                for (int j = 0; j < line_length; j++) {
                    if (line[j] == ' ' && !task_found) {
                        task_found = true;
                        task_path[task_path_index] = '\0';
                        continue;
                    }

                    if (!task_found) {
                        task_path[task_path_index++] = line[j];
                    } else {
                        console_nr = atoi(line + j);
                        break;
                    }
                }
                
                // empty line / comment
                if (!task_found || line[0] == '#') {
                    continue;
                }
                
                log_printf(loggerid, "[TASKS.CONF] path=%s, console_nr=%d", task_path, console_nr);
                
                L4_ThreadId_t new_threadid = get_next_thread_id();
                IF_CONSOLESERVER_setactivethread(consoleserverid, console_nr, &new_threadid, &env);

                // XXX: passing NULL might be a problem when we actually use "_env"
                taskserver_create_task_real(L4_Myself(), task_path, "", NULL);
            }

            if (buff._buffer[i] == '\r') {
                continue;
            }
        }
    } while (res_read == buff._maximum);
}

// get thread id for the next task
L4_ThreadId_t get_next_thread_id() {
    // task_id 0 is never used, so start search at 1
    for (L4_Word_t i = 1; i < NUM_T_ENTRY; i++) {
        if (!taskList[i].task_exists) {
            return create_thread_id(i, 0);
        }
    }

    panic("could not find free task_id!");
}

L4_ThreadId_t taskserver_create_task_real(CORBA_Object  _caller, const path_t  path, const path_t  cmdline, idl4_server_environment * _env) 
{
    L4_ThreadId_t threadid = get_next_thread_id();
    taskList[get_task_id(threadid)].task_exists = true;
    
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
        return L4_nilthread;
    }
    
	buf_t buff;
	char tbuff[1024];
	buff._buffer = (CORBA_char*)tbuff;
	buff._maximum = 1024;

    log_printf(loggerid, "[TASK] Loading file: %s", path);
    

    L4_Word_t res_read = IF_FILESERVER_read(fileserverid, file_id, 0, sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr), &buff, &env);
    
    Elf32_Ehdr *hdr = (Elf32_Ehdr *)tbuff;
    Elf32_Phdr *phdr = get_elf_phdr(hdr);
    if (phdr == NULL) {
        panic("Invalid ELF file");
    }

    log_printf(loggerid, "[TASK] ELF decoded. Number of PHs: %d", hdr->e_phnum);

    for (int i = 0; i < hdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            IF_MEMORYSERVER_map_file_pages(
                    (CORBA_Object)memoryserverid,
                    &threadid,
                    0,
                    path,
                    phdr[i].p_offset,
                    phdr[i].p_vaddr,
                    phdr[i].p_memsz,
                    phdr[i].p_filesz,
                    &env);
        }
    }

    IF_MEMORYSERVER_map_anon_pages(
            memoryserverid, &threadid, 0, 
            (L4_Word_t)STACK_START - STACK_SIZE, 
            STACK_SIZE, &env);

    IF_MEMORYSERVER_startup((CORBA_Object)memoryserverid, &threadid, (L4_Word_t)hdr->e_entry, (L4_Word_t)STACK_START, &env);
    
    //make an entry in task list
    taskList[get_task_id(threadid)].has_thread[0] = true; 
    for(int i=1; i<MAX_THREADS; i++) {
        taskList[get_task_id(threadid)].has_thread[i] = false;
    }

    return threadid;
}

L4_ThreadId_t  taskserver_create_thread_real(CORBA_Object  _caller, const L4_Word_t  ip, const L4_Word_t  sp, idl4_server_environment * _env)
{
    L4_Word_t threadNo = -1;

    //find next empty thread slot in task list
    for(int i=0; i<MAX_THREADS; i++) {
        if(!taskList[get_task_id(_caller)].has_thread[i]) {
            threadNo = i;
            break;
        }
    }

    if(threadNo == -1) {
        panic("No free thread slot available");
    }

    L4_ThreadId_t threadid = create_thread_id(get_task_id(_caller), threadNo);

    /* do the ThreadControl call */
    if (!L4_ThreadControl (threadid, _caller, L4_Myself(), memoryserverid,
		      (void*)((L4_Address (utcbarea) + utcbsize * threadNo)  & ~(utcbsize - 1)) )) {
        panic ("ThreadControl failed");
    }

    IF_MEMORYSERVER_startup((CORBA_Object) memoryserverid, &threadid, ip, sp, &env);
    taskList[get_task_id(_caller)].has_thread[threadNo] = true;

    return threadid;

}

void taskserver_kill_task_implementation_real(CORBA_Object  _caller, const L4_ThreadId_t * threadid, idl4_server_environment * _env)
{
    L4_Word_t task_id = get_task_id(*threadid);

    log_printf(loggerid, "[TASK] killing task_id: %d", task_id);
    
    // iterate over all threads in the given task
    for (int i = 0; i < MAX_THREADS; i++) {
        if (taskList[task_id].has_thread[i]) {
            L4_ThreadId_t kill_thread_id = create_thread_id(task_id, i);
            if (!L4_ThreadControl(kill_thread_id, L4_nilthread, L4_nilthread, L4_nilthread, (void *)-1)) {
                panic("Could not kill thread");
            }
        }
    }
    
    clear_tasklist_entry(task_id);
    
    IF_MEMORYSERVER_destroyed(memoryserverid, threadid, &env);
}

CORBA_boolean  taskserver_is_running_impl(CORBA_Object  _caller, const L4_ThreadId_t * task_id, idl4_server_environment * _env)
{
    return taskList[get_task_id(*task_id)].task_exists;
}

