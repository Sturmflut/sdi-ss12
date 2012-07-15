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

    IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[CONSOLESERVER] Active", &env);

    /* Spin forever */
    while (42);
    
    return 0;
}
