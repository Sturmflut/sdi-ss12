#ifndef _MEMORYSERVER_H_
#define _MEMORYSERVER_H_

#include <sdi/sdi.h>

#define NOT_YET_MAPPED -1
#define MAX_FILE_MAPPINGS 10
#define MAX_ANON_MAPPINGS 20

/*Data structures for memory server */

typedef struct {
      L4_Word_t virt_address; //Address of page in clients space
      L4_Word_t size;
} Anon_mapping_t;

typedef struct {
      L4_Word_t virt_address;
      char path[64]; // path_t has length 64 (see types.idl)
      L4_Word_t offset;
      L4_Word_t size; // size of mapping; size >= realsize
      L4_Word_t realsize; // size of file
} File_mapping_t;

typedef struct {
    bool task_exists;
    L4_Word_t taskid;
    Anon_mapping_t anon_mappings[MAX_ANON_MAPPINGS];
    File_mapping_t file_mappings[MAX_FILE_MAPPINGS];
    unsigned int anon_mapping_index;
    unsigned int file_mapping_index;	
} Taskheader_t;

extern Taskheader_t taskList[NUM_T_ENTRY];

// points to the first free entry in taskList
extern unsigned char Taskheader_index;

extern L4_ThreadId_t loggerid;
extern L4_ThreadId_t sigma0id;
extern L4_ThreadId_t fileserverid;
extern CORBA_Environment env;
extern char logbuf[80];

extern void memoryserver_init();

extern L4_Word_t memoryserver_map_anon_pages_real(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  type, const L4_Word_t  virt_start_address, const L4_Word_t  size, idl4_server_environment * _env);

extern void  memoryserver_pagefault_real(CORBA_Object  _caller, const L4_Word_t  address, const L4_Word_t  ip, const L4_Word_t  privileges, idl4_fpage_t * page, idl4_server_environment * _env);

extern void memoryserver_startup_real(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  ip, const L4_Word_t  sp, idl4_server_environment * _env);

extern L4_Word_t memoryserver_map_file_pages_real(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  type, const path_t  path, const L4_Word_t  offset, const L4_Word_t  virt_start_address, const L4_Word_t  size, const L4_Word_t  realsize, idl4_server_environment * _env);


void  memoryserver_server(void);

#endif

