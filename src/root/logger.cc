/*****************************************************************
 * Source file : /home/stoess/devel/sdios.new/src/root/logger.idl
 * Platform    : V4 Generic
 * Mapping     : CORBA C
 * 
 * Generated by IDL4 1.1.0 (roadrunner) on 15/05/2009 21:19
 * Report bugs to haeberlen@ira.uka.de
 *****************************************************************/

#include <l4io.h>
#include <idl4glue.h>
#include "logger-server.h"
#include <sdi/sdi.h>

#include <nameserver.h>

#include "root.h"



/* Interface logger */

IDL4_INLINE void  logger_LogMessage_implementation(CORBA_Object  _caller, const logmessage_t  message, idl4_server_environment * _env)

{
  /* implementation of IF_LOGGING::LogMessage */
  printf("[LOGGER] %s\n", message);
  
  return;
}

IDL4_PUBLISH_LOGGER_LOGMESSAGE(logger_LogMessage_implementation);

void * logger_vtable_1[LOGGER_DEFAULT_VTABLE_SIZE] = LOGGER_DEFAULT_VTABLE_1;
void * logger_vtable_discard[LOGGER_DEFAULT_VTABLE_SIZE] = LOGGER_DEFAULT_VTABLE_DISCARD;
void ** logger_itable[2] = { logger_vtable_discard, logger_vtable_1 };

void  logger_server()

{
    /* Announce logger service */
  nameserver_register("/server/logger");

  L4_ThreadId_t  partner;
  L4_MsgTag_t  msgtag;
  idl4_msgbuf_t  msgbuf;
  long  cnt;

  idl4_msgbuf_init(&msgbuf);
  for (cnt = 0;cnt < LOGGER_STRBUF_SIZE;cnt++)
    idl4_msgbuf_add_buffer(&msgbuf, malloc(8000), 8000);

  while (1)
    {
      partner = L4_nilthread;
      msgtag.raw = 0;
      cnt = 0;

      while (1)
        {
          idl4_msgbuf_sync(&msgbuf);

          idl4_reply_and_wait(&partner, &msgtag, &msgbuf, &cnt);

          if (idl4_is_error(&msgtag))
            break;

          idl4_process_request(&partner, &msgtag, &msgbuf, &cnt, logger_itable[idl4_get_interface_id(&msgtag) & LOGGER_IID_MASK][idl4_get_function_id(&msgtag) & LOGGER_FID_MASK]);
        }
    }
}

void  logger_discard()

{
}

