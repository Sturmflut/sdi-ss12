#include "shell.h"

static void print_usage()
{
    console_printf(consoleid, "  Kernel/OS info\n");
    console_printf(consoleid, "  Usage: uname <options>\n");
    console_printf(consoleid, "    -a  All\n");
    console_printf(consoleid, "    -s  Kernel\n");
}



void builtin_uname(char* cmdline)
{
    if(strlen(cmdline) == 0)
    {
        print_usage();
        return;
    }

    if(strncmp(cmdline, "-a", strlen(cmdline)) == 0)
        {
                console_printf(consoleid, "SDIOS 0.1 %s i686\n", __DATE__);
                return;
        }

    if(strncmp(cmdline, "-s", strlen(cmdline)) == 0)
        {
                console_printf(consoleid, "SDIOS\n");
                return;
        }

    print_usage();
}


    
