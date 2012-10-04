#ifndef _SHELL_H_
#define _SHELL_H_

#include <sdi/sdi.h>

typedef struct {
        const char* cmd;
        void (*command)(char* cmdline);
} builtin_cmd_t;


// Server Ids
extern L4_ThreadId_t loggerid;
extern L4_ThreadId_t consoleid;
extern L4_ThreadId_t fileid;


// Builtin commands
void builtin_cat(char* cmdline);
void builtin_ls(char* cmdline);
void builtin_uname(char* cmdline);


#endif
