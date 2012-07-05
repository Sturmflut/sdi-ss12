//
// File: lib/sdi/logging.cc
//
// Description: Logging messages
//

#include <l4io.h>

#include <stdlib.h>

#include <sdi/types.h>
#include <sdi/sdi.h>


void LogMessage (const char* message) {
	printf("[LOG] %s\n", message);
}
