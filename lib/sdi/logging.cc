//
// File: lib/sdi/logging.cc
//
// Description: Logging messages
//

#include <l4io.h>

#include <stdlib.h>

#include <sdi/types.h>
#include <sdi/sdi.h>

#include <if/iflogging.h>

int log_printf(L4_ThreadId_t logger_id, const char *format, ...)
{
	int retval=0;
	va_list ap;
	char buf[256];

	//L4_ThreadId_t logger_id;
	CORBA_Environment env(idl4_default_environment);

    // this lookup causes weird effects!
    //while (L4_IsNilThread(logger_id))
    //    logger_id = nameserver_lookup("/server/logger");

	va_start(ap, format); /* Initialize the va_list */

	retval = vsnprintf(buf, sizeof(buf), format, ap); /* Call vprintf */
	IF_LOGGING_LogMessage((CORBA_Object)logger_id, buf, &env);

	va_end(ap); /* Cleanup the va_list */

	return retval;
}

