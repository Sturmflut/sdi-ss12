#include <sdi/sdi.h>
#include <sdi/constants.h>
#include <sdi/io.h>
#include <l4io.h>

#include <if/iffileserver.h>
#include "fileserver.h"


L4_ThreadId_t  fileserver_Lookup_impl(CORBA_Object  _caller, const path_t  path, path_t * remaining, idl4_server_environment * _env)
{
	if(strlen(path) == 0 || (strlen(path) == 1 && path[0] == '/'))
		return L4_Myself();

	if(fileserver_get_file_id_impl(_caller, path, _env) >= 0)
	{
		strncpy(*remaining, path, strlen(path));
		return L4_Myself();
	}

	return L4_nilthread;
}


/**
 * Find last_occurrence of chr from given str - e.g. '/'
 */
unsigned int find_last_of(char* str, const char ch)
{
    unsigned int last_occurrence = 0;
    char *pch;
    pch=strchr(str,ch);

    while (pch!=NULL)
    {
        last_occurrence = pch-str+1;
        pch=strchr(pch+1,ch);
    }
    return last_occurrence;
}


L4_Word_t  fileserver_get_file_id_impl(CORBA_Object  _caller, const path_t  path, idl4_server_environment * _env)
{
  L4_Word_t  __retval = -1;
  L4_BootInfo_t* bootinfo = (L4_BootInfo_t*)L4_BootInfo (L4_KernelInterface ());
  L4_BootRec_t* bootrec = L4_BootInfo_FirstEntry (bootinfo);
  unsigned int type1_cnt = 0;
  for (unsigned int i=0; i < L4_BootInfo_Entries (bootinfo); i++) {
	 if((int)L4_Type (bootrec) == 1) {	//only records of type 1 are relevant
		char *cmdline = L4_Module_Cmdline(bootrec);
		//printf("%d) %s\n", i, cmdline);
		char *mpath = path;
		//removes initial '/'
		if (strncmp(path, "/", 1) == 0)
			mpath = path+1;

		unsigned int last_occurrence = find_last_of(cmdline, '/');
		if (last_occurrence != 0)		//'/' found; now cut string
			cmdline+=last_occurrence;

		/**
		 * Now compare both strings:
		 *  given path with modified boot
		 *  if equal, remember i and leave for-loop
		 */
		if (strncmp(cmdline, mpath, strlen(cmdline)) == 0) {
			__retval = type1_cnt;
			break;
		}
		++type1_cnt;
	 }
	 bootrec = L4_Next (bootrec);
 }

  return __retval;
}



L4_Word_t  fileserver_read_impl(CORBA_Object  _caller, const L4_Word_t  file_id, const L4_Word_t  offset, const L4_Word_t  count, buf_t * buf, idl4_server_environment * _env)
{
//	L4_Word_t  __retval = 0;
	  buf->_length = 0;

	  L4_BootInfo_t* bootinfo = (L4_BootInfo_t*)L4_BootInfo (L4_KernelInterface ());
	  L4_BootRec_t* bootrec = L4_BootInfo_FirstEntry (bootinfo);
	  unsigned int type1_cnt = 0;
	  unsigned int i;
	  for (i=0; i < L4_BootInfo_Entries (bootinfo); i++) {
			 if((int)L4_Type (bootrec) == 1) { //only records of type 1 are relevant
				 if (type1_cnt == file_id)
					 break;
				 ++type1_cnt;
			 }
			 bootrec = L4_Next (bootrec);
	   }

	  if (i < L4_BootInfo_Entries(bootinfo)) {

		  /**
		   * At this point matching bootrecord was found (according to given file_id)
		   */
		    if(offset < L4_Module_Size (bootrec))
		    {
		        buf->_length = min(count, L4_Module_Size (bootrec) - offset);
		        memcpy( buf->_buffer, (void*) (L4_Module_Start (bootrec) + offset), buf->_length);
		    }
		    //else
		        //Offset out of range
	  }
	  return buf->_length;
}

L4_Word_t  fileserver_write_impl(CORBA_Object  _caller, const L4_Word_t  file_id, const L4_Word_t  offset, const L4_Word_t  count, const buf_t * buf, idl4_server_environment * _env)
{
	return -1;
}


L4_Word_t  fileserver_get_file_size_impl(CORBA_Object  _caller, const path_t  path, idl4_server_environment * _env)
{
  /* implementation of IF_FILE::get_file_size */
  L4_Word_t  __retval = 0;
  L4_BootInfo_t* bootinfo = (L4_BootInfo_t*)L4_BootInfo (L4_KernelInterface ());
  L4_BootRec_t* bootrec = L4_BootInfo_FirstEntry (bootinfo);
  for (unsigned int i=0; i < L4_BootInfo_Entries (bootinfo); i++) {
	 if((int)L4_Type (bootrec) == 1) {	//only records of type 1 are relevant
		char *cmdline = L4_Module_Cmdline(bootrec);
		//printf("%d) %s\n", i, cmdline);
		char *mpath = path;
		//removes initial '/'
		if (strncmp(path, "/", 1) == 0)
			mpath = path+1;

		/**
		 * Find last_occurrence of '/' - character
		 */
		char *pch;
		pch=strchr(cmdline,'/');
		unsigned int last_occurrence = 0;
		while (pch!=NULL)
		{
			last_occurrence = pch-cmdline+1;
//    			printf ("found at %d\n",last_occurrence);
			pch=strchr(pch+1,'/');
		}
		if (last_occurrence != 0)		//'/' found; now cut string
			cmdline+=last_occurrence;

		/**
		 * Now compare both strings:
		 *  given path with modified boot
		 *  if equal, get module size and leave for-loop
		 */
		if (strncmp(cmdline, mpath, strlen(cmdline)) == 0) {
			__retval = L4_Module_Size (bootrec);
			break;
		}
	 }
	 bootrec = L4_Next (bootrec);
 }
 return __retval;
}


L4_Word_t  fileserver_get_file_type_impl(CORBA_Object  _caller, const path_t  path, idl4_server_environment * _env)
{
	if(strlen(path) == 0)
		return -1;

	if(strncmp("/", path, strlen(path)) == 0)
		return IF_FILE_TYPE_DIRECTORY;

	if(fileserver_get_file_id_impl(_caller, path, _env) >= 0)
		return IF_FILE_TYPE_FILE;

	return -1;
}


CORBA_boolean  fileserver_remove_impl(CORBA_Object  _caller, const L4_Word_t  file_id, idl4_server_environment * _env)
{
  CORBA_boolean  __retval = 0;

  /* implementation of IF_FILE::remove */

  return __retval;
}


CORBA_boolean  fileserver_mkdir_impl(CORBA_Object  _caller, const path_t  path, idl4_server_environment * _env)
{
  CORBA_boolean  __retval = 0;

  /* implementation of IF_FILE::mkdir */

  return __retval;
}


CORBA_boolean  fileserver_rmdir_impl(CORBA_Object  _caller, const path_t  path, idl4_server_environment * _env)
{
  CORBA_boolean  __retval = 0;

  /* implementation of IF_FILE::rmdir */

  return __retval;
}


L4_Word_t fileserver_get_dir_size_impl(CORBA_Object  _caller, const path_t  path, idl4_server_environment * _env)
{
  L4_Word_t  __retval = 0;

  L4_BootInfo_t* bootinfo = (L4_BootInfo_t*)L4_BootInfo (L4_KernelInterface ());
  L4_BootRec_t* bootrec = L4_BootInfo_FirstEntry (bootinfo);

  unsigned int i;
  for (i=0; i < L4_BootInfo_Entries (bootinfo); i++) {
	  if((int)L4_Type (bootrec) == 1) { //only records of type 1 are relevant
		  ++__retval;
  	  }
      bootrec = L4_Next (bootrec);
  }
  return __retval;
}


CORBA_boolean  fileserver_get_dir_entry_impl(CORBA_Object  _caller, const path_t  path, const L4_Word_t  entry, buf_t * buf, idl4_server_environment * _env)
{
  buf->_length = 0;
  L4_BootInfo_t* bootinfo = (L4_BootInfo_t*)L4_BootInfo (L4_KernelInterface ());
  L4_BootRec_t* bootrec = L4_BootInfo_FirstEntry (bootinfo);
  unsigned int valid_entries = fileserver_get_dir_size_impl(_caller, path, _env);
  if (entry >= 0 && entry < valid_entries) {	//check range
	  unsigned int type1_cnt = 0;
	  for (unsigned int i=0; i < valid_entries; i++) {
		  if((int)L4_Type (bootrec) == 1) {
			  if (entry == type1_cnt) {
				  char *cmdline = L4_Module_Cmdline(bootrec);
				  unsigned int found_at = find_last_of(cmdline, '/');
				  if (found_at != 0) {		//'/' found; now cut string
				      cmdline+=found_at;
				  }
//				  strncpy(*buf, cmdline, strlen(cmdline));
				  buf->_length = strlen(cmdline);
				  memcpy( buf->_buffer, cmdline, buf->_length);
				  return 1;
			  }
			  ++type1_cnt;
		  }
		  bootrec = L4_Next (bootrec);
	  }
  }
  return 0;
}
