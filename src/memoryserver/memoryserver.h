#ifndef _MEMORYSERVER_H_
#define _MEMORYSERVER_H_

#include <sdi/sdi.h>

#define NUM_T_ENTRY 30
#define NOT_YET_MAPPED -1
#define FILE_READ_BUFFER 8

/*Data structures for memory server */

typedef struct{
      L4_Word_t base_address; //Address of page in own space
      L4_Word_t virt_address; //Address of page in clients space
      L4_Word_t size;
} Page_entry_t;

typedef struct{
      L4_Word_t virt_address;
      path_t path;
      L4_Word_t offset;
      L4_Word_t size; //size >= realsize
      L4_Word_t realsize;
      L4_Fpage_t page;
} File_entry_t;

typedef struct{
      L4_Word_t taskid;
      Page_entry_t pages[1000];
      File_entry_t filemaps[10];
	unsigned int pages_index;
	unsigned int filemaps_index;	
} Taskheader_t;

extern Taskheader_t taskList[NUM_T_ENTRY];

extern unsigned char Taskheader_index;

extern L4_ThreadId_t loggerid;
extern L4_ThreadId_t sigma0id;
extern L4_ThreadId_t fileserverid;
extern CORBA_Environment env;
extern char logbuf[80];

extern L4_Word_t memoryserver_map_anon_pages_real(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  type, const L4_Word_t  virt_start_address, const L4_Word_t  size, idl4_server_environment * _env);

extern void  memoryserver_pagefault_real(CORBA_Object  _caller, const L4_Word_t  address, const L4_Word_t  ip, const L4_Word_t  privileges, idl4_fpage_t * page, idl4_server_environment * _env);

extern void memoryserver_startup_real(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  ip, const L4_Word_t  sp, idl4_server_environment * _env);

extern L4_Word_t memoryserver_map_file_pages_real(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  type, const path_t  path, const L4_Word_t  offset, const L4_Word_t  virt_start_address, const L4_Word_t  size, const L4_Word_t  realsize, idl4_server_environment * _env);


void  memoryserver_server(void);

#endif

