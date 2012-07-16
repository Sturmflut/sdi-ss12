#ifndef _DRIVERSERVER_H
#define _DRIVERSERVER_H 1

#include <sdi/constants.h>

#include <if/ifdriverserver.h>

#include <nameserver.h>

/**
 Register the calling thread for the given path at the default driverserver
*/
void driverserver_register(path_t path)
{
        CORBA_Environment env (idl4_default_environment);
        L4_ThreadId_t driverserverid = nameserver_lookup("/driver");
	IF_DRIVERSERVER_register((CORBA_Object) driverserverid, path, &env);
}


/**
 Deregister the calling thread from the given path at the default driverserver
*/
void driverserver_deregister()
{
        CORBA_Environment env (idl4_default_environment);
        L4_ThreadId_t driverserverid = nameserver_lookup("/driver");
	IF_DRIVERSERVER_deregister((CORBA_Object) driverserverid, &env);
}

#endif

