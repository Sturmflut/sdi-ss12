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


int console_printf(const char *format, ...)
{
	int retval=0;
	va_list ap;
	char buf[256];

	L4_ThreadId_t consoleid;
	CORBA_Environment env(idl4_default_environment);

        while (L4_IsNilThread(consoleid))
                consoleid = nameserver_lookup("/server/console");

	va_start(ap, format); /* Initialize the va_list */

	retval = vsnprintf(buf, sizeof(buf), format, ap); /* Call vprintf */
	IF_CONSOLESERVER_putstring((CORBA_Object)consoleid, buf, SDI_CONSOLE_ATTRIBUTE_FGLIGHTWHITE, &env);

	va_end(ap); /* Cleanup the va_list */

	return retval;
}
