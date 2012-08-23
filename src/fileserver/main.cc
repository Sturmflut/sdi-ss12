#include <sdi/sdi.h>
#include <sdi/constants.h>
#include <l4io.h>

#include "fileserver.h"

int main(void)
{
	fileserver_server();

	while(42)
	{
	}
}

