#include <sdi/sdi.h>
#include <sdi/constants.h>

#include <l4io.h>

#include "shell.h"


static void print_usage()
{
	console_printf(consoleid, "  Lookup the given path through naming\n");
	console_printf(consoleid, "  Usage: lookup <path>\n");
}


static L4_ThreadId_t lookup(path_t path)
{
	CORBA_Environment env (idl4_default_environment);

        L4_ThreadId_t next = L4_nilthread;
        L4_ThreadId_t cur = L4_nilthread;
        CORBA_char in[SDI_NAMESERVER_MAX_ENTRY_LEN + 1];
        CORBA_char rem[SDI_NAMESERVER_MAX_ENTRY_LEN + 1];
        CORBA_char * remp = (CORBA_char*) &rem;

        L4_ThreadId_t nameserverid = L4_GlobalId (SDI_NAMESERVER_DEFAULT_THREADID, 1);

        /* Initialize */
        strncpy(in, path, SDI_NAMESERVER_MAX_ENTRY_LEN);

        next = nameserverid;
        do
        {
                cur = next;
		printf( "Lookup next: %x\n", next.raw);
                next = IF_NAMING_Lookup((CORBA_Object) next, in, &remp, &env);
		printf( "Lookup result: %x\n", next.raw);

                if(!L4_IsNilThread(next))
                        strncpy(in, remp, SDI_NAMESERVER_MAX_ENTRY_LEN);

	        CORBA_free(remp);

        } while(cur != next && !L4_IsNilThread(next) && strlen(in) > 0);

        return next;
}



void builtin_lookup(char* cmdline)
{
	if(strlen(cmdline) == 0)
	{
		print_usage();
		return;
	}

	// Perform lookup
	L4_ThreadId_t result = nameserver_lookup(cmdline);

	if(L4_IsNilThread(result))
	{
		console_printf((CORBA_Object)consoleid, "Could not lookup %s\n", cmdline);
		return;
	}

	console_printf((CORBA_Object)consoleid, "Positive response for %s from thread %x\n", cmdline, result.raw);
}


	
