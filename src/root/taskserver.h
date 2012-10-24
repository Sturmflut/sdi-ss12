#ifndef _TASKSERVER_H_
#define _TASKSERVER_H_

#include <sdi/sdi.h>
#include <if/iflogging.h>

#define MAX_THREADS 10

// TODO: Datenstruktur um Threads zu speichern
typedef struct{
    bool task_exists;
    bool has_thread[MAX_THREADS]; //index is thread cnt
} Task_entry_t;

extern CORBA_Environment env;

// XXX: might be confusing - data structure has same name as in
// memoryserver.h
extern Task_entry_t taskList[NUM_T_ENTRY]; //index is task number

void taskserver_init();

void clear_tasklist_entry(L4_Word_t task_id);

void start_init_tasks();

L4_ThreadId_t get_next_thread_id();

L4_ThreadId_t taskserver_create_task_real(CORBA_Object  _caller, const path_t  path, const path_t  cmdline, idl4_server_environment * _env);

L4_ThreadId_t  taskserver_create_thread_real(CORBA_Object  _caller, const L4_Word_t  ip, const L4_Word_t  sp, idl4_server_environment * _env);

void taskserver_kill_task_implementation_real(CORBA_Object  _caller, const L4_ThreadId_t * task_id, idl4_server_environment * _env);

CORBA_boolean taskserver_is_running_impl(CORBA_Object _caller, const L4_ThreadId_t * task_id, idl4_server_environment * _env);


#endif
