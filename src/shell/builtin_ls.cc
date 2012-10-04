#include <sdi/sdi.h>

#include <if/iffile.h>

#include "shell.h"


static void print_usage()
{
	console_printf(consoleid, "  Directory listing on the fileserver\n");
	console_printf(consoleid, "  Usage: ls <path>\n");
}


void builtin_ls(char* cmdline)
{
	CORBA_Environment env(idl4_default_environment);

	// Print usage
	if(strlen(cmdline) == 0)
	{
		print_usage();
		return;
	}

	int type = IF_FILE_get_file_type((CORBA_Object)fileid, cmdline, &env);
	console_printf((CORBA_Object)consoleid, "%s type: %i\n", cmdline, type);

	if(type != IF_FILE_TYPE_DIRECTORY)
	{
		console_printf((CORBA_Object)consoleid, "ls: %s is not a directory.\n", cmdline);
		return;
	}

	// List entries
	int entries = IF_FILE_get_dir_size((CORBA_Object)fileid, cmdline, &env);
	console_printf((CORBA_Object)consoleid, "%i entries found\n", entries);
}

