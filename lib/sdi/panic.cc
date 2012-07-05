//
// File:  lib/sdi/panic.cc
//
// Description: Panic functions
//

#include <l4/kdebug.h>
#include <l4io.h>

#include <stdlib.h>

#include <sdi/types.h>
#include <sdi/sdi.h>

/* Panic Managment */

void panic (const char* message) {
    while (42) {
	printf ("\n\n%s\n\n", message);
	L4_KDB_Enter ("panic");
    }
}
