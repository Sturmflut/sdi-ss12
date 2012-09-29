#ifndef _TASKSERVER_H_
#define _TASKSERVER_H_

#include <sdi/sdi.h>

L4_ThreadId_t taskserver_create_task_real(CORBA_Object  _caller, const path_t  path, const path_t  cmdline, idl4_server_environment * _env);

#endif
