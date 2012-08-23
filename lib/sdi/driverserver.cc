#include <sdi/sdi.h>
#include <idl4glue.h>

#include <sdi/constants.h>
#include <stdlib.h>

#include <if/ifdriverserver.h>

/**
 Register the calling thread for the given path at the default driverserver
*/
void driverserver_register(string64_t path)
{
        CORBA_Environment env (idl4_default_environment);
        L4_ThreadId_t driverserverid = L4_nilthread;

	while(L4_IsNilThread(driverserverid))
		driverserverid = nameserver_lookup("/driver");

	IF_DRIVERSERVER_register((CORBA_Object) driverserverid, path, &env);
}


/**
 Deregister the calling thread from the given path at the default driverserver
*/
void driverserver_deregister()
{
        CORBA_Environment env (idl4_default_environment);
        L4_ThreadId_t driverserverid = nameserver_lookup("/driver");

	while(L4_IsNilThread(driverserverid))
		driverserverid = nameserver_lookup("/driver");

	IF_DRIVERSERVER_deregister((CORBA_Object) driverserverid, &env);
}


