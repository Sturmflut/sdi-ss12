#ifndef _SHELL_H_
#define _SHELL_H_

#include <sdi/sdi.h>

typedef struct {
        const char* cmd;
        void (*command)(char* cmdline);
} builtin_cmd_t;


extern L4_ThreadId_t loggerid;
extern L4_ThreadId_t consoleid;


void builtin_ls(char* cmdline);
void builtin_uname(char* cmdline);


#endif
