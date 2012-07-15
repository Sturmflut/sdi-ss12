#include <sdi/sdi.h>

#include <l4/ipc.h>

void sleep (int msec)
{
	L4_Sleep( L4_TimePeriod( msec * 1000 ) );
}
