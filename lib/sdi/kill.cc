#include <sdi/sdi.h>
#include <idl4glue.h>

#include <sdi/constants.h>
#include <stdlib.h>

#include <if/iflogging.h>
#include <if/iftaskserver.h>

void kill(void)
{
        CORBA_Environment env(idl4_default_environment);

        /* get ID of task that calls this helper */
        L4_ThreadId_t killID = L4_Myself();

        /* task server id */
        L4_ThreadId_t tservID = L4_nilthread;

        while (L4_IsNilThread(tservID))
        {
                tservID = nameserver_lookup("/task");
        }

        /* call kill_task from taskserver to clean up the stopped task */
        IF_TASKSERVER_kill_task((CORBA_Object)tservID, &killID, &env);
}
