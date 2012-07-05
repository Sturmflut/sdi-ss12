//
// File:  src/root/root.h
//
// Description: Header for root task
//

#include <l4/kdebug.h>

#include <l4io.h>

#include <macros.h>
#include <sdi/types.h>


/* ======== Thread IDs =========== */

extern L4_ThreadId_t sigma0id;   // sigma0, just in case
extern L4_ThreadId_t pagerid;    // our internal pager
extern L4_ThreadId_t locatorid;  // locator service
extern L4_ThreadId_t loggerid;   // messaging service


extern void locator_server (void);
extern void logger_server (void);
extern void pager_loop (void);

