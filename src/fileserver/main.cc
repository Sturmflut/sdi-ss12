#include <l4/bootinfo.h>
#include <l4/kip.h>

#include <sdi/sdi.h>
#include <sdi/constants.h>
#include <l4io.h>

#include <if/iflogging.h>
#include "fileserver-server.h"
#include "fileserver.h"

L4_ThreadId_t loggerid;
CORBA_Environment env(idl4_default_environment);

L4_BootInfo_t* bootinfo;

L4_BootInfo_t* get_boot_info()
{
	if (!L4_BootInfo_Valid((void*)L4_BootInfo (L4_KernelInterface ())))
	        return NULL;

	return (L4_BootInfo_t*) L4_BootInfo(L4_KernelInterface());
}


int main(void)
{
	char logbuf[80];

         while (L4_IsNilThread(loggerid))
                loggerid = nameserver_lookup("/server/logger");

        IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[FILE] Registering", &env);

        nameserver_register("/file");

        IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[FILE] Registered...", &env);

	/* Get bootinfo */
	bootinfo = get_boot_info();
	
	if(bootinfo == NULL)
		panic("Fileserver cannot access bootinfo");

	snprintf(logbuf, sizeof(logbuf), "[FILE] %li files emulated", L4_BootInfo_Entries (bootinfo));
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuf, &env);

	fileserver_server();

	while(42)
	{
	}
}

