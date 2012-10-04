#include <sdi/sdi.h>
#include <idl4glue.h>

#include <l4io.h>

#include <sdi/constants.h>
#include <sdi/console_attributes.h>
#include <stdlib.h>

#include <stdarg.h>

#include <if/ifnameserver.h>
#include <if/ifconsoleserver.h>
#include <if/iflogging.h>


static char current_color = SDI_CONSOLE_ATTRIBUTE_FGLIGHTGREY | SDI_CONSOLE_ATTRIBUTE_BGBLACK;


int console_printf(L4_ThreadId_t consoleid, const char *format, ...)
{
	int retval=0;
	va_list ap;
	char buf[256];

	CORBA_Environment env(idl4_default_environment);

	va_start(ap, format); /* Initialize the va_list */

	retval = vsnprintf(buf, sizeof(buf), format, ap); /* Call vprintf */
	IF_CONSOLESERVER_putstring((CORBA_Object)consoleid, buf, current_color, &env);

	va_end(ap); /* Cleanup the va_list */

	return retval;
}


char console_readchar(L4_ThreadId_t consoleid, char* modifier)
{
	char key;
	CORBA_Environment env(idl4_default_environment);

	IF_CONSOLESERVER_getKey((CORBA_Object)consoleid, &key, modifier, &env);
}


int console_readline(L4_ThreadId_t consoleid, char* buffer, L4_Word_t maxlen)
{
	L4_Word_t inlen = 0;
	char key;
	char modifier;
	bool ret;
	CORBA_Environment env(idl4_default_environment);

	memset(buffer, 0, maxlen);

	while(key != '\n' && key != 0x0d && inlen < maxlen)
	{
		ret = IF_CONSOLESERVER_getKey((CORBA_Object)consoleid, &key, &modifier, &env);

		// Handle backspace
		if(key == 0x7f && modifier == 0)
		{
			if(inlen > 0)
			{
				buffer[inlen-1] = '\0';
				inlen--;
				IF_CONSOLESERVER_putchar((CORBA_Object)consoleid, 0x7f, current_color, &env);
			}

			continue;
		}
	
		// Only accept valid keys
		if(ret && key != '\t' && key != '\0' && key != '\r' && key != '\n' && key != 0x7f)
		{
			buffer[inlen] = key;
			inlen++;
			IF_CONSOLESERVER_putchar((CORBA_Object)consoleid, key, current_color, &env);
		}

		sleep(100);
	}

	return inlen;
}


void console_setcolor(char color)
{
	current_color = color;
}
