#ifndef _TASKSERVER_H_
#define _TASKSERVER_H_

#include <sdi/sdi.h>
#include <if/iflogging.h>

// TODO: Datenstruktur um Threads zu speichern

extern CORBA_Environment env;

void taskserver_init();

L4_ThreadId_t taskserver_create_task_real(CORBA_Object  _caller, const path_t  path, const path_t  cmdline, idl4_server_environment * _env);

#endif
