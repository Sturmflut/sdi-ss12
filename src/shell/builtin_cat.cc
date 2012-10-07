#include <sdi/sdi.h>

#include <if/iffile.h>

#include "shell.h"


static void print_usage()
{
	console_printf(consoleid, "  Dump the content of a file on the fileserver\n");
	console_printf(consoleid, "  Usage: cat <path>\n");
}


void builtin_cat(char* cmdline)
{
	CORBA_Environment env(idl4_default_environment);

	// Print usage
	if(strlen(cmdline) == 0)
	{
		print_usage();
		return;
	}

	// Check if the given path is a file
	if(IF_FILE_get_file_type((CORBA_Object)fileid, cmdline, &env) != IF_FILE_TYPE_FILE)
	{
		console_printf((CORBA_Object)consoleid, "ls: %s is not a file.\n", cmdline);
		return;
	}

	// Read from file
	L4_Word_t id = IF_FILE_get_file_id((CORBA_Object)fileid, cmdline, &env);

	char buf[256];
	buf_t readbuf;
	readbuf._buffer = (CORBA_char*)&buf;
	readbuf._maximum = sizeof(buf)-1;

	int len = 0;
	int offs = 0;

	while((len = IF_FILE_read((CORBA_Object)fileid, id, offs, sizeof(buf)-1, &readbuf, &env)) > 0)
	{
		buf[len] = '\0';
		console_printf(consoleid, "%s", buf);
		offs += len;
	}

	return;
}

