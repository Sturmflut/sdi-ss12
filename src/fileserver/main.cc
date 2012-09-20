#include <sdi/sdi.h>
#include <sdi/constants.h>
#include <l4io.h>

#include <if/iflogging.h>

#include "fileserver.h"

L4_ThreadId_t loggerid;
CORBA_Environment env(idl4_default_environment);

int main(void)
{
         while (L4_IsNilThread(loggerid))
                loggerid = nameserver_lookup("/server/logger");

        IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[FILE] Registering", &env);

        nameserver_register("/file");

        IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[FILE] Registered...", &env);

	
	fileserver_server();

	while(42)
	{
	}
}

