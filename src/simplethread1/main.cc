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
#include <if/iflocator.h>
#include <if/iflogging.h>
#include <if/ifnameserver.h>

L4_ThreadId_t locatorid; 
L4_ThreadId_t nameserverid; 

int main () {
    L4_Msg_t msg;
    L4_MsgTag_t tag;

    /* Guess locatorid */
    locatorid = L4_GlobalId (L4_ThreadIdUserBase (L4_KernelInterface ()) + 3, 1);

    CORBA_Environment env (idl4_default_environment);    
    L4_ThreadId_t loggerid = L4_nilthread;

    printf ("Resolve logger ...\n");
    while (L4_IsNilThread (loggerid)) {
        IF_LOCATOR_Locate ((CORBA_Object)locatorid, IF_LOGGING_ID, &loggerid, &env);
    }


    /* Guess nameserverid */
    nameserverid = L4_GlobalId (5353, 1);

    printf ("Registering with the nameserver ...\n");
    IF_NAMESERVER_register ((CORBA_Object)locatorid, "/clients/simplethread1", &env);


    /* Spin forever */
    while (42);
    
    return 0;
}
