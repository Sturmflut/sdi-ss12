#ifndef _MEMORYSERVER_H_
#define _MEMORYSERVER_H_

/*Data structures for memory server */

typedef struct{
      L4_Word_t base_address; //Address of page in own space
      L4_Word_t virt_address; //Address of page in clients space
      L4_Word_t size;
} Page_entry_t;

typedef struct{
      L4_Word_t virt_address;
      char path[256];
      L4_Word_t offset;
      L4_Word_t size;
      L4_Word_t realsize;
      L4_Fpage_t page;
} File_entry_t;

typedef struct{
      L4_ThreadId_t threadid;
      Page_entry_t pages[1000];
      File_entry_t filemaps[10];
} Taskheader_t;


extern L4_ThreadId_t loggerid;

void  memoryserver_server(void);

#endif

