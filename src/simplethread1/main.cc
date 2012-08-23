#include <sdi/sdi.h>
#include <sdi/constants.h>
#include <l4io.h>

#include <if/iffile.h>

int main(void)
{
	CORBA_Environment	env(idl4_default_environment);
	buf_t buf;
	char tbuf[10];
	buf._buffer = (CORBA_char*)&tbuf;
	buf._maximum = 10;

	L4_ThreadId_t fileserverid = L4_GlobalId (SDI_FILESERVER_DEFAULT_THREADID, 1);
	IF_FILE_read((CORBA_Object) fileserverid, 0, 0, 0, &buf, &env);
	printf("[TEST] %s Len %i\n", buf._buffer, buf._length);

	while(42)
	{
	}
}

