//
// File:  src/test/main.cc
//
// Description: Testclient
//

#include <l4io.h>

#include <sdi/sdi.h>
#include <sdi/types.h>
#include <sdi/constants.h>

#include <if/iflogging.h>
#include <if/ifnaming.h>
#include <if/ifconsoleserver.h>

#include <sdi/console_attributes.h>

#include "shell.h"


L4_ThreadId_t loggerid = L4_nilthread;
L4_ThreadId_t consoleid = L4_nilthread;
L4_ThreadId_t fileid = L4_nilthread;
CORBA_Environment env(idl4_default_environment);


char cmdbuf[256];


builtin_cmd_t builtins[] = {
	{ "cat", builtin_cat },
	{ "lookup", builtin_lookup },
	{ "ls", builtin_ls },
	{ "uname", builtin_uname },
	{ "\0", NULL }
};


bool builtin_command(char* cmd, char* cmdline)
{
	int cmdlen = sizeof(cmd);
	int i = 0;

	while(builtins[i].cmd[0] != '\0')
	{
		if(strncmp(cmd, builtins[i].cmd, cmdlen) == 0)
		{
			builtins[i].command(cmdline);
			return true;
		}
		i++;
	}

	return false;
}


void command_loop()
{
	char* cmd;
	char* cmdline;

	while(1)
	{
		// Clear buffer
		console_printf(consoleid, "shell$ ");
		console_readline(consoleid, cmdbuf, sizeof(cmdbuf)-1);
		console_printf(consoleid, "\n");

		if(strlen(cmdbuf) == 0)
			continue;

		// Split command and cmdline
		int firstspace = -1;
		int len = strlen(cmdbuf);
		cmd = cmdbuf;
	
		for(int i = 0; i < len && firstspace < 0; i++)
			if(cmdbuf[i] == ' ')
				firstspace = i;
	
		if(firstspace == -1)
			cmdline = "";
		else
		{
			cmdbuf[firstspace] = '\0';
			cmdline = &cmdbuf[firstspace + 1];
		}


		// Handle builtin commands
		if(!builtin_command(cmd, cmdline))
		{
			console_printf(consoleid, "Unknown command, should do taskserver::create_task here\n");
		}
	
	}
}


void print_banner()
{
	console_printf(consoleid, "SDI-Shell 0.1 beta\n\n");
}


int main()
{
	CORBA_Environment env(idl4_default_environment);

	// Resolve logger
	while (L4_IsNilThread(loggerid))
		loggerid = nameserver_lookup("/server/logger");

	// Resolve console
	while (L4_IsNilThread(consoleid))
		consoleid = nameserver_lookup("/server/console");

	// Resolve fileserver
	while (L4_IsNilThread(fileid))
		fileid = nameserver_lookup("/file");

	L4_ThreadId_t myself = L4_Myself();

	/* Startup shell */
	print_banner();

	/* And now.... action! */
	command_loop();

	/* Spin forever */
	while (42) ;

	return 0;
}
