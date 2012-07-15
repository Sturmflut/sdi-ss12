//
// File:  src/test/main.cc
//
// Description: Testclient
//

#include <l4/thread.h>

#include <l4io.h>

#include <sdi/types.h>
#include <sdi/sdi.h>

#include <idl4glue.h>
#include <if/iflogging.h>

#include <nameserver.h>

int main () {
    L4_Msg_t msg;
    L4_MsgTag_t tag;

    CORBA_Environment env (idl4_default_environment);

    L4_ThreadId_t loggerid = L4_nilthread;

    printf(" ");

    while (L4_IsNilThread (loggerid)) {
        loggerid = nameserver_lookup("/server/logger");
    }

    IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[DRIVERSERVER] Active", &env);

    /* Spin forever */
    while (42);
    
    return 0;
}
