#ifndef _NAMESERVER_H
#define _NAMESERVER_H 1

#include <sdi/constants.h>

#include <if/ifnameserver.h>

/**
 Register the calling thread for the given path at the default nameserver
*/
void nameserver_register(path_t path)
{
        CORBA_Environment env (idl4_default_environment);
        L4_ThreadId_t nameserverid = L4_GlobalId (SDI_NAMESERVER_DEFAULT_THREADID, 1);
	IF_NAMESERVER_register((CORBA_Object) nameserverid, path, &env);
}


/**
 Deregister the calling thread from the given path at the default nameserver
*/
void nameserver_deregister()
{
        CORBA_Environment env (idl4_default_environment);
        L4_ThreadId_t nameserverid = L4_GlobalId (SDI_NAMESERVER_DEFAULT_THREADID, 1);
	IF_NAMESERVER_deregister((CORBA_Object) nameserverid, &env);
}

/**
 Recursive lookup of path path on the default nameserver (port 5353)
*/
L4_ThreadId_t nameserver_lookup(path_t path)
{
	CORBA_Environment env (idl4_default_environment);

	L4_ThreadId_t next = L4_nilthread;
	CORBA_char in[SDI_NAMESERVER_MAX_ENTRY_LEN + 1];
	CORBA_char rem[SDI_NAMESERVER_MAX_ENTRY_LEN + 1];
	CORBA_char * remp = (CORBA_char*) &rem;

	L4_ThreadId_t nameserverid = L4_GlobalId (SDI_NAMESERVER_DEFAULT_THREADID, 1);

	/* Initialize */
	strncpy(in, path, SDI_NAMESERVER_MAX_ENTRY_LEN);

	do
	{
		next = IF_NAMING_Lookup((CORBA_Object) nameserverid, in, &remp, &env);

		if(next != L4_nilthread)
			strncpy(in, remp, SDI_NAMESERVER_MAX_ENTRY_LEN);

		//CORBA_string_free(*rem);

	} while(next != L4_nilthread && strlen(remp) > 0);

	//CORBA_string_free(in);

	return next;
}

#endif

