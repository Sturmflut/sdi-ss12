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
    IF_LOGGING_LogMessage ((CORBA_Object)loggerid, "Hello World Thread 1", &env);


    /* Send two words to the companion thread */
    L4_MsgClear(&msg);
    L4_MsgAppendWord(&msg, 23);
    L4_MsgAppendWord(&msg, 42);
    L4_MsgLoad(&msg);

    tag = L4_Send(L4_GlobalId ( L4_ThreadNo (L4_Myself ()) + 1, 1));

    IF_LOGGING_LogMessage ((CORBA_Object)loggerid, "Thread 1 finished sending", &env);

    /* Receive result from companion */
    L4_MsgClear(&msg);
    tag = L4_Receive(L4_GlobalId ( L4_ThreadNo (L4_Myself ()) + 1, 1));
    L4_Store(tag, &msg);

    IF_LOGGING_LogMessage ((CORBA_Object)loggerid, "Thread 1 finished receiving", &env);
    printf("Thread 1 got sum: %i\n", L4_Get(&msg, 0));


    /* Spin forever */
    while (42);
    
    return 0;
}
