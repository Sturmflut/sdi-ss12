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

#include <if/iflocator.h>
#include <if/iflogging.h>
#include <if/ifnameserver.h>

#include <nameserver.h>


//extern "C" int printf(const char * format, ...);

L4_ThreadId_t locatorid; 
L4_ThreadId_t nameserverid; 

int main () {
    L4_Msg_t msg;
    L4_MsgTag_t tag;

	printf("a");

    /* Guess locatorid */
    locatorid = L4_GlobalId (L4_ThreadIdUserBase (L4_KernelInterface ()) + 3, 1);

    CORBA_Environment env (idl4_default_environment);    
    L4_ThreadId_t loggerid = L4_nilthread;

    while (L4_IsNilThread (loggerid)) {
        IF_LOCATOR_Locate ((CORBA_Object)locatorid, IF_LOGGING_ID, &loggerid, &env);
    }


    /* Guess nameserverid */
    L4_ThreadId_t nameserverid = L4_GlobalId (SDI_NAMESERVER_DEFAULT_THREADID, 1);

    IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[SIMPLETHREAD1] Registering", &env);

    /* Register */
    IF_NAMESERVER_register ((CORBA_Object)nameserverid, "/clients/simplethread1", &env);

    IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[SIMPLETHREAD1] Registered", &env);


    /* Lookup existing query */
    if(nameserver_lookup("/clients/simplethread1") != L4_Myself ())
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[SIMPLETHREAD1] Lookup for /clients/simplethread1 failed", &env);
    else
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[SIMPLETHREAD1] Lookup for /clients/simplethread1 okay", &env);


    /* Lookup non-existant query */
    if(nameserver_lookup("/clients/bernd") != L4_nilthread)
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[SIMPLETHREAD1] Lookup for /clients/bernd did not return L4_nilthread", &env);
    else
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[SIMPLETHREAD1] Lookup for /clients/bernd failed as expected", &env);



    /* Spin forever */
    while (42);
    
    return 0;
}
