#ifndef _FILESERVER_H_
#define _FILESERVER_H_

extern L4_ThreadId_t loggerid;
extern L4_BootInfo_t* bootinfo;

void fileserver_server();

#endif
