#include <sdi/sdi.h>
#include <idl4glue.h>

#include <sdi/constants.h>
#include <stdlib.h>

#include <if/iftaskserver.h>

// We split the ThreadNo into a "task id" and a "thread count", so
// when we are given a global thread id, we can deduce the task id from
// it.

L4_Word_t get_task_id(L4_ThreadId_t threadid) {
    // get first 10 bit of thread no
    return (L4_ThreadNo(threadid) >> 8);
}


L4_Word_t get_thread_count(L4_ThreadId_t threadid) {
    // get last 8 bit of thread no
    return (L4_ThreadNo(threadid) & 0xff); 
}

L4_ThreadId_t create_thread_id(L4_Word_t task_id, L4_Word_t thread_count) {
    return (L4_GlobalId(task_id << 8 | thread_count, 1));
}

/* blocking function to wait for the death of a given task */
void task_wait(L4_ThreadId_t id)
{
	CORBA_Environment env(idl4_default_environment);

    /* task server id */
    L4_ThreadId_t tservID = L4_nilthread;

    while (L4_IsNilThread(tservID))
    {
        tservID = nameserver_lookup("/task");
    }

    /* call isRunning from taskserver to check, if task exists */
	while (IF_TASKSERVER_is_running((CORBA_Object)tservID, &id, &env))
	{
		sleep(100);
	}
}


