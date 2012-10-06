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


	// Does it exist?
	if(type != IF_FILE_TYPE_DIRECTORY && type != IF_FILE_TYPE_FILE)
	{
		console_printf((CORBA_Object)consoleid, "ls: %s is neither a file nor a directory.\n", cmdline);
		return;
	}

	// Handle file
	if(type == IF_FILE_TYPE_FILE)
	{
		L4_Word_t size = IF_FILE_get_file_size((CORBA_Object)fileid, cmdline, &env);
		
		console_printf((CORBA_Object)consoleid, "%i  %s\n", size, cmdline);
		return;
	}

	// Handle directory
	// List entries
	int entries = IF_FILE_get_dir_size((CORBA_Object)fileid, cmdline, &env);
	console_printf((CORBA_Object)consoleid, "%s contains %i entries\n", cmdline, entries);

	buf_t pathbuf;
	char namebuf[256];

	for(int j = 0; j < entries; j++)
	{
		pathbuf._buffer = (CORBA_char*)&namebuf;
		pathbuf._maximum= sizeof(namebuf);
		memset(namebuf, 0, sizeof(namebuf));

		IF_FILE_get_dir_entry((CORBA_Object)fileid, cmdline, j, &pathbuf, &env);
		console_printf((CORBA_Object)consoleid, "%s\n", pathbuf._buffer);
	}
}

