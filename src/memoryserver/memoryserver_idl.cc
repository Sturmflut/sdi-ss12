/*****************************************************************
 * Source file : memoryserver.idl
 * Platform    : V4 IA32
 * Mapping     : CORBA C
 * 
 * Generated by IDL4 1.1.0 (roadrunner) on 20/09/2012 14:28
 * Report bugs to haeberlen@ira.uka.de
 *****************************************************************/

//#include <sdi/sdi.h>

#include "memoryserver.h"
#include "memoryserver-server.h"

/* Interface memoryserver */

IDL4_INLINE L4_Word_t  memoryserver_map_io_pages_implementation(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  virt_start_address, const L4_Word_t  phys_start_address, const CORBA_long  size, idl4_server_environment * _env)

{
  L4_Word_t  __retval = 0;

  /* implementation of IF_MEMORYSERVER::map_io_pages */

  return __retval;
}

IDL4_PUBLISH_MEMORYSERVER_MAP_IO_PAGES(memoryserver_map_io_pages_implementation);

IDL4_INLINE L4_Word_t  memoryserver_map_anon_pages_implementation(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  type, const L4_Word_t  virt_start_address, const L4_Word_t  size, idl4_server_environment * _env)

{
  /* implementation of IF_MEMORYSERVER::map_anon_pages */
  return memoryserver_map_anon_pages_real(_caller, threadid, type, virt_start_address, size, _env);
}

IDL4_PUBLISH_MEMORYSERVER_MAP_ANON_PAGES(memoryserver_map_anon_pages_implementation);

IDL4_INLINE L4_Word_t  memoryserver_map_file_pages_implementation(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  type, const path_t  path, const L4_Word_t  offset, const L4_Word_t  virt_start_address, const L4_Word_t  size, const L4_Word_t  realsize, idl4_server_environment * _env)

{
  /* implementation of IF_MEMORYSERVER::map_file_pages */
  
  return memoryserver_map_file_pages_real(_caller, threadid, type, path, offset, virt_start_address, size, realsize, _env);


}

IDL4_PUBLISH_MEMORYSERVER_MAP_FILE_PAGES(memoryserver_map_file_pages_implementation);

IDL4_INLINE void  memoryserver_unmap_pages_implementation(CORBA_Object  _caller, const L4_Word_t  virt_start_address, idl4_server_environment * _env)

{
  /* implementation of IF_MEMORYSERVER::unmap_pages */
  
  return;
}

IDL4_PUBLISH_MEMORYSERVER_UNMAP_PAGES(memoryserver_unmap_pages_implementation);

IDL4_INLINE void  memoryserver_startup_implementation(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  ip, const L4_Word_t  sp, idl4_server_environment * _env)

{
  /* implementation of IF_MEMORYSERVER::startup */
  memoryserver_startup_real(_caller, threadid, ip, sp, _env);
  return;
}

IDL4_PUBLISH_MEMORYSERVER_STARTUP(memoryserver_startup_implementation);

IDL4_INLINE void  memoryserver_destroyed_implementation(CORBA_Object  _caller, const L4_ThreadId_t * threadid, idl4_server_environment * _env)

{
  return memoryserver_destroyed_implementation_real(_caller, threadid, _env);
}

IDL4_PUBLISH_MEMORYSERVER_DESTROYED(memoryserver_destroyed_implementation);

IDL4_INLINE void  memoryserver_pagefault_implementation(CORBA_Object  _caller, const L4_Word_t  address, const L4_Word_t  ip, const L4_Word_t  privileges, idl4_fpage_t * page, idl4_server_environment * _env)

{
  /* implementation of IF_MEMORYSERVER::pagefault */
  memoryserver_pagefault_real(_caller, address, ip, privileges, page, _env);

  return;
}

IDL4_PUBLISH_MEMORYSERVER_PAGEFAULT(memoryserver_pagefault_implementation);

void * memoryserver_vtable_12[MEMORYSERVER_DEFAULT_VTABLE_SIZE] = MEMORYSERVER_DEFAULT_VTABLE_12;
void * memoryserver_vtable_discard[MEMORYSERVER_DEFAULT_VTABLE_SIZE] = MEMORYSERVER_DEFAULT_VTABLE_DISCARD;
void ** memoryserver_itable[16] = { memoryserver_vtable_discard, memoryserver_vtable_discard, memoryserver_vtable_discard, memoryserver_vtable_discard, memoryserver_vtable_discard, memoryserver_vtable_discard, memoryserver_vtable_discard, memoryserver_vtable_discard, memoryserver_vtable_discard, memoryserver_vtable_discard, memoryserver_vtable_discard, memoryserver_vtable_discard, memoryserver_vtable_12, memoryserver_vtable_discard, memoryserver_vtable_discard, memoryserver_vtable_discard };
void * memoryserver_ktable[MEMORYSERVER_DEFAULT_KTABLE_SIZE] = MEMORYSERVER_DEFAULT_KTABLE;

void  memoryserver_server(void)

{
  L4_ThreadId_t  partner;
  L4_MsgTag_t  msgtag;
  idl4_msgbuf_t  msgbuf;
  long  cnt;

  idl4_msgbuf_init(&msgbuf);
  for (cnt = 0;cnt < MEMORYSERVER_STRBUF_SIZE;cnt++)
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

          if (IDL4_EXPECT_FALSE(idl4_is_kernel_message(msgtag)))
            idl4_process_request(&partner, &msgtag, &msgbuf, &cnt, memoryserver_ktable[idl4_get_kernel_message_id(msgtag) & MEMORYSERVER_KID_MASK]);
            else idl4_process_request(&partner, &msgtag, &msgbuf, &cnt, memoryserver_itable[idl4_get_interface_id(&msgtag) & MEMORYSERVER_IID_MASK][idl4_get_function_id(&msgtag) & MEMORYSERVER_FID_MASK]);
        }
    }
}

void  memoryserver_discard(void)

{
}

