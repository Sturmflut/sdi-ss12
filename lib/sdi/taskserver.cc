#include <sdi/sdi.h>
#include <idl4glue.h>

#include <sdi/constants.h>
#include <stdlib.h>

// We split the ThreadNo into a "task id" and a "thread count", so
// when we are given a global thread id, we can deduce the task id from
// it.

L4_Word_t get_task_id(L4_ThreadId_t threadid) {
    // get first 10 bit of thread no
    return (L4_ThreadNo(threadid) >> 10);
}


L4_Word_t get_thread_count(L4_ThreadId_t threadid) {
    // get last 8 bit of thread no
    return (L4_ThreadNo(threadid) & (0xffffffff >> (32 - 8))); 
}

L4_ThreadId_t create_thread_id(L4_Word_t task_id, L4_Word_t thread_count) {
    return (L4_GlobalId(task_id << 10 | thread_count, 1));
}


