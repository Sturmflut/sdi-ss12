/*****************************************************************
 * Source file : consoleserver.idl
 * Platform    : V4 IA32
 * Mapping     : CORBA C
 * 
 * Generated by IDL4 1.1.0 (roadrunner) on 27/09/2012 12:52
 * Report bugs to haeberlen@ira.uka.de
 *****************************************************************/

#include <sdi/sdi.h>

#include "consoleserver-server.h"

#include "consoleserver.h"

static char bufmsg[CONSOLESERVER_STRBUF_SIZE][1024];


/* Interface consoleserver */

IDL4_INLINE void  consoleserver_interrupt_implementation(CORBA_Object  _caller, idl4_server_environment * _env)

{
	consoleserver_interrupt_impl();
}

IDL4_PUBLISH_CONSOLESERVER_INTERRUPT(consoleserver_interrupt_implementation);

IDL4_INLINE void  consoleserver_putchar_implementation(CORBA_Object  _caller, const CORBA_char  character, const CORBA_char  attrib, idl4_server_environment * _env)

{
	consoleserver_putchar_impl(_caller, character, attrib);
}

IDL4_PUBLISH_CONSOLESERVER_PUTCHAR(consoleserver_putchar_implementation);

IDL4_INLINE void  consoleserver_putcharat_implementation(CORBA_Object  _caller, const CORBA_long  x, const CORBA_long  y, const CORBA_char  character, const CORBA_char  attrib, idl4_server_environment * _env)
{
	consoleserver_putcharat_impl(_caller, x, y, character, attrib);
}

IDL4_PUBLISH_CONSOLESERVER_PUTCHARAT(consoleserver_putcharat_implementation);

IDL4_INLINE void  consoleserver_putstring_implementation(CORBA_Object  _caller, const consolestring_t  text, const CORBA_char  attrib, idl4_server_environment * _env)

{
	consoleserver_putstring_impl(_caller, text, attrib);
}

IDL4_PUBLISH_CONSOLESERVER_PUTSTRING(consoleserver_putstring_implementation);

IDL4_INLINE void  consoleserver_putstringat_implementation(CORBA_Object  _caller, const CORBA_long  x, const CORBA_long  y, const consolestring_t text, const CORBA_char  attrib, idl4_server_environment * _env)

{
	consoleserver_putstringat_impl(_caller, x, y, text, attrib);
}

IDL4_PUBLISH_CONSOLESERVER_PUTSTRINGAT(consoleserver_putstringat_implementation);

IDL4_INLINE void  consoleserver_setactivethread_implementation(CORBA_Object  _caller, const CORBA_long  console, const L4_ThreadId_t * thread, idl4_server_environment * _env)

{
	consoleserver_setactivethread_impl(_caller, console, thread);
}

IDL4_PUBLISH_CONSOLESERVER_SETACTIVETHREAD(consoleserver_setactivethread_implementation);

IDL4_INLINE CORBA_long  consoleserver_getconsolenum_implementation(CORBA_Object  _caller, idl4_server_environment * _env)

{
	return consoleserver_getconsolenum_impl(_caller);
}

IDL4_PUBLISH_CONSOLESERVER_GETCONSOLENUM(consoleserver_getconsolenum_implementation);

IDL4_INLINE CORBA_boolean  consoleserver_getKey_implementation(CORBA_Object  _caller, CORBA_char * key, CORBA_char * modifier, idl4_server_environment * _env)

{
	return consoleserver_getKey_impl(_caller, key, modifier);
}

IDL4_PUBLISH_CONSOLESERVER_GETKEY(consoleserver_getKey_implementation);

void * consoleserver_vtable[CONSOLESERVER_DEFAULT_VTABLE_SIZE] = CONSOLESERVER_DEFAULT_VTABLE;
void * consoleserver_ktable[CONSOLESERVER_DEFAULT_KTABLE_SIZE] = CONSOLESERVER_DEFAULT_KTABLE;

void  consoleserver_server(void)

{
  L4_ThreadId_t  partner;
  L4_MsgTag_t  msgtag;
  idl4_msgbuf_t  msgbuf;
  long  cnt;

  idl4_msgbuf_init(&msgbuf);
  for (cnt = 0;cnt < CONSOLESERVER_STRBUF_SIZE;cnt++)
    idl4_msgbuf_add_buffer(&msgbuf, bufmsg[cnt], 1024);

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

          if (IDL4_EXPECT_FALSE(idl4_is_kernel_message(msgtag)))
            idl4_process_request(&partner, &msgtag, &msgbuf, &cnt, consoleserver_ktable[idl4_get_kernel_message_id(msgtag) & CONSOLESERVER_KID_MASK]);
            else idl4_process_request(&partner, &msgtag, &msgbuf, &cnt, consoleserver_vtable[idl4_get_function_id(&msgtag) & CONSOLESERVER_FID_MASK]);
        }
    }
}

void  consoleserver_discard(void)

{
}

