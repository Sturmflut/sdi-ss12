//
// File:  src/test/main.cc
//
// Description: Testclient
//

#include <l4io.h>
#include <stdlib.h>

#include <sdi/sdi.h>
#include <sdi/types.h>
#include <sdi/constants.h>

#include <if/iflogging.h>
#include <if/ifnaming.h>
#include <if/ifconsoleserver.h>
#include <if/iffileserver.h>

#include <sdi/console_attributes.h>
#include <sdi/io.h>


L4_ThreadId_t consoleid = L4_nilthread;
L4_ThreadId_t loggerid = L4_nilthread;

CORBA_Environment env(idl4_default_environment);

int main()
{
	char logbuf[80];

	int scancode;
	int scanstatus;

	while (L4_IsNilThread(loggerid))
		loggerid = nameserver_lookup("/server/logger");

	IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[SIMPLETHREAD2] Active", &env);

	char tbuf[10];
	buf_t buf;
	buf._buffer = (CORBA_char*)&tbuf;
	buf._maximum = 10;	

	/* Resolve fileserver */
	L4_ThreadId_t fileid;
	while (L4_IsNilThread(fileid))
		fileid = nameserver_lookup("/file");

	/* Call fileserver */
	IF_FILE_read((CORBA_Object) fileid, 0, 0, 0, &buf, &env);

	snprintf(logbuf, sizeof(logbuf), "[TEST] %s Len %i\n", buf._buffer, buf._length);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuf, &env);

	/* testing get_file_id */
	char *path = "/nameserver";
	L4_Word_t res = IF_FILESERVER_get_file_id(fileid, path, &env);
	printf("get file id for >%s< returns >>%d<< (except 0)\n", path, res);

	char *path2 = "/simplethread1";
	L4_Word_t res2 = IF_FILESERVER_get_file_id(fileid, path2, &env);
	printf("get file id for >%s< returns >>%d<< (except >0)\n", path2, res2);

	char *path3 = "simplethread2";
	L4_Word_t res3 = IF_FILESERVER_get_file_id(fileid, path3, &env);
	printf("get file id for >%s< returns >>%d<< (except >0)\n", path3, res3);

	char *path4 = "simplethread3";
	L4_Word_t res4 = IF_FILESERVER_get_file_id(fileid, path4, &env);
	printf("get file id for >%s< returns >>%d<< (except -1)\n", path4, res4);

	/* Spin forever */
	while (42) ;

	return 0;
}
