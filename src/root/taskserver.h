#ifndef _TASKSERVER_H_
#define _TASKSERVER_H_

#include <sdi/sdi.h>
#include <if/iflogging.h>

#define NUM_T_ENTRY 30
#define MAX_THREADS 10

// TODO: Datenstruktur um Threads zu speichern
typedef struct{
    bool has_thread[MAX_THREADS]; //index is thread cnt
} Task_entry_t;

extern CORBA_Environment env;
extern Task_entry_t taskList[NUM_T_ENTRY]; //index is task number

void taskserver_init();

L4_ThreadId_t taskserver_create_task_real(CORBA_Object  _caller, const path_t  path, const path_t  cmdline, idl4_server_environment * _env);

L4_ThreadId_t  taskserver_create_thread_real(CORBA_Object  _caller, const L4_Word_t  ip, const L4_Word_t  sp, idl4_server_environment * _env);

#endif
