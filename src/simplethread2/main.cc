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

L4_ThreadId_t locatorid; 

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

    /* Printout message through logger */
    IF_LOGGING_LogMessage ((CORBA_Object)loggerid, "Hello World Thread 2", &env);

    /* Receive two words from the companion task */
    L4_MsgClear(&msg);
    tag = L4_Receive(L4_GlobalId ( L4_ThreadNo (L4_Myself ()) - 1, 1));
    L4_Store(tag, &msg);

    IF_LOGGING_LogMessage ((CORBA_Object)loggerid, "Thread 2 finished receiving", &env);
    printf("Thread 2 got word 1: %i\n", L4_Get(&msg, 0));
    printf("Thread 2 got word 2: %i\n", L4_Get(&msg, 1));

    /* Send the sum of both words back */
    L4_MsgClear(&msg);
    L4_MsgAppendWord(&msg, L4_Get(&msg, 0) + L4_Get(&msg, 1));
    L4_MsgLoad(&msg);

    tag = L4_Send(L4_GlobalId ( L4_ThreadNo (L4_Myself ()) - 1, 1));
    IF_LOGGING_LogMessage ((CORBA_Object)loggerid, "Thread 2 finished sending", &env);

    /* Spin forever */
    while (42);
    
    return 0;
}
