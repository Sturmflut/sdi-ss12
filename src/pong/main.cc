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
#include <if/iftaskserver.h>

#include <sdi/console_attributes.h>
#include <sdi/io.h>


L4_ThreadId_t consoleid = L4_nilthread;
L4_ThreadId_t loggerid = L4_nilthread;
L4_ThreadId_t taskserverid = L4_nilthread;

CORBA_Environment env(idl4_default_environment);

int main()
{
	char logbuf[80];

	int scancode;
	int scanstatus;

	while (L4_IsNilThread(loggerid))
		loggerid = nameserver_lookup("/server/logger");

    while (L4_IsNilThread(taskserverid)) {
        taskserverid = nameserver_lookup("/task");
    }
    
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[SIMPLETHREAD2] Active", &env);
    
	char tbuf[10];
	buf_t buf;
	buf._buffer = (CORBA_char*)&tbuf;
	buf._maximum = 10;	

	/* Resolve fileserver */
	L4_ThreadId_t fileid;
	while (L4_IsNilThread(fileid))
		fileid = nameserver_lookup("/file");


	char logbuff[100];

	/* testing get_file_id */
	char *path = "/nameserver";
	L4_Word_t res = IF_FILESERVER_get_file_id(fileid, path, &env);
	snprintf(logbuff, sizeof(logbuff), "[TEST FILESERVER get_file_id for=%s] >>%d<< (expected '0')\n", path, res);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuff, &env);

	char *path2 = "/bannerdemo";
	res = IF_FILESERVER_get_file_id(fileid, path2, &env);
	snprintf(logbuff, sizeof(logbuff), "[TEST FILESERVER get_file_id for=%s] >>%d<< (expected 'gr 0')\n", path2, res);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuff, &env);

	char *path3 = "simplethread2";
	res = IF_FILESERVER_get_file_id(fileid, path3, &env);
	snprintf(logbuff, sizeof(logbuff), "[TEST FILESERVER get_file_id for=%s] >>%d<< (expected 'gr 0')\n", path3, res);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuff, &env);

	char *path4 = "simplethread3";
	res = IF_FILESERVER_get_file_id(fileid, path4, &env);
	snprintf(logbuff, sizeof(logbuff), "[TEST FILESERVER get_file_id for=%s] >>%d<< (expected '-1')\n", path4, res);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuff, &env);


	/* testing read	 */
	const L4_Word_t  read_id = 2;
	const L4_Word_t  offset = 0;
	const L4_Word_t  count = 7;


	buf_t buff;
	char tbuff[50];
	buff._buffer = (CORBA_char*)&tbuff;
	buff._maximum = 50;
	L4_Word_t res_read = IF_FILESERVER_read(fileid, read_id, offset, count, &buff, &env);

	snprintf(logbuff, sizeof(logbuff), "[TEST FILESERVER read with fileid=%i] \"%s\" (Len %i)\n", read_id, buff._buffer, buff._length);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuff, &env);
	//printf("%s --- result of read is %i\n", logbuff, res_read);

	/* get_dir_size */
	strcpy(logbuff, "");	//empty string
	res = IF_FILE_get_dir_size(fileid, "/",&env);
	snprintf(logbuff, sizeof(logbuff), "[TEST FILESERVER get_dir_size of '/'] = %d\n", res);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuff, &env);

	/* get_file_size */
	strcpy(logbuff, "");	//empty string
	res = IF_FILE_get_file_size(fileid, "/consoleserver",&env);
	snprintf(logbuff, sizeof(logbuff), "[TEST FILESERVER get_file_size of '/consoleserver'] = %lx\n", res);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuff, &env);

	/* get_dir_entry */
	buf_t dir_entry_buffer;
	strncpy(tbuff, "", 50);
	dir_entry_buffer._buffer = (CORBA_char*)&tbuff;
	dir_entry_buffer._maximum = 50;

	L4_Word_t result = IF_FILE_get_dir_entry(fileid, "/", 1, &dir_entry_buffer, &env);
	snprintf(logbuff, sizeof(logbuff), "[TEST FILESERVER get_dir_entry nmb '1' of '/'] = '%s'\n (result: '%i')", dir_entry_buffer._buffer, result);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuff, &env);

	buf_t dir_entry_buffer2;
	strncpy(tbuff, "", 50);
	dir_entry_buffer2._buffer = (CORBA_char*)&tbuff;
	dir_entry_buffer2._maximum = 50;

	result = IF_FILE_get_dir_entry(fileid, "/", 7, &dir_entry_buffer2, &env);
	snprintf(logbuff, sizeof(logbuff), "[TEST FILESERVER get_dir_entry nmb '7' of '/'] = '%s'\n (result: '%i')", dir_entry_buffer2._buffer, result);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuff, &env);

	/* Spin forever */
	while (42) ;

	return 0;
}
