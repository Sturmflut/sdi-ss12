//
// File:  src/test/main.cc
//
// Description: Testclient
//

#include <l4/thread.h>
#include <l4/space.h>
#include <l4/message.h>
#include <l4/ipc.h>
#include <l4/sigma0.h>
#include <l4/bootinfo.h>
#include <l4/kdebug.h>

#include <l4io.h>
#include <macros.h>

#include <stdlib.h>

#include <sdi/types.h>
#include <sdi/sdi.h>

#include <idl4glue.h>

#include <sdi/constants.h>

#include <if/iflogging.h>
#include <if/ifnameserver.h>

#include <nameserver.h>


//extern "C" int printf(const char * format, ...);


int main () {
    L4_Msg_t msg;
    L4_MsgTag_t tag;

    char buf[256];

	printf("a");

    CORBA_Environment env (idl4_default_environment);

    L4_ThreadId_t loggerid = L4_nilthread;

    while (L4_IsNilThread (loggerid)) {
        loggerid = nameserver_lookup("/server/logger");
    }

    IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[SIMPLETHREAD1] Registering", &env);

    nameserver_register("/clients/simplethread1");

    IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[SIMPLETHREAD1] Registered", &env);


    /* Lookup existing query */
    L4_ThreadId_t result = L4_nilthread;

    if((result = nameserver_lookup("/clients/simplethread1")) != L4_Myself ())
    {
	snprintf(buf, 256, "[SIMPLETHREAD1] Lookup for /clients/simplethread1 failed, got %lx instead od %lx", &env, result.raw, L4_Myself().raw);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, buf, &env);
    }
    else
    {
	snprintf(buf, 256, "[SIMPLETHREAD1] Lookup for /clients/simplethread1 okay, got %lx when expecting %lx", result.raw, L4_Myself().raw);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, buf, &env);

    }
    /* Lookup non-existant query */
    if((result = nameserver_lookup("/clients/bernd")) != L4_nilthread)
    {
	snprintf(buf, 256, "[SIMPLETHREAD1] Lookup for /clients/bernd did not return L4_nilthread, but %lx", result);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, buf, &env);
    }
    else
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[SIMPLETHREAD1] Lookup for /clients/bernd failed as expected", &env);



    /* Spin forever */
    while (42);
    
    return 0;
}
