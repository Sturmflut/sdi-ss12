
#include "memoryserver.h"
#include <l4/sigma0.h>
#include <l4io.h>

#include <if/iflogging.h>
#include <if/iffileserver.h>

Taskheader_t taskList[NUM_T_ENTRY];
unsigned char Taskheader_index;

unsigned char findOrCreateTaskEntry(L4_Word_t taskid)
{
	char taskheader_entry = -1;

	for(int i = 0; i < NUM_T_ENTRY; i = i+1)
	{
		if(taskList[i].taskid == taskid)
			taskheader_entry = i;
	}	
	
	if(taskheader_entry == -1)
	{
		taskheader_entry = Taskheader_index;
		Taskheader_index = Taskheader_index + 1;
		taskList[Taskheader_index].pages_index = 0;
		taskList[Taskheader_index].filemaps_index = 0;
	}
	
	return taskheader_entry;
}

char checkAddressOverlapping(L4_Word_t virt_start_address, L4_Word_t virt_end_address, L4_Word_t mapped_virt_start_address, L4_Word_t mapped_virt_end_address)
{
	if(mapped_virt_start_address <= virt_end_address &&
		virt_end_address <= mapped_virt_end_address)
		return 0;

	if(mapped_virt_start_address <= virt_start_address &&
		virt_start_address <= mapped_virt_end_address)
		return 0;
		
	if(virt_start_address <= mapped_virt_start_address &&
		mapped_virt_end_address <= virt_end_address)
		return 0;
	
	return -1;
}

char isAddressConflict(Taskheader_t * myTaskheader, L4_Word_t virt_start_address, L4_Word_t virt_end_address) 
{	
	for(int i=0; i<myTaskheader->pages_index; i = i+1)
	{
		L4_Word_t mapped_virt_start_address = myTaskheader->pages[i].virt_address;
		L4_Word_t mapped_virt_end_address = mapped_virt_start_address + myTaskheader->pages[i].size;
		
		if( checkAddressOverlapping(virt_start_address, virt_end_address, mapped_virt_start_address, mapped_virt_end_address) == 0 )
			return 1;
		
	}	

	for(int i=0; i<myTaskheader->filemaps_index; i = i+1)
	{
		L4_Word_t mapped_virt_start_address = myTaskheader->filemaps[i].virt_address;
		L4_Word_t mapped_virt_end_address = mapped_virt_start_address + myTaskheader->filemaps[i].size;

		if( checkAddressOverlapping(virt_start_address, virt_end_address, mapped_virt_start_address, mapped_virt_end_address) == 0 )
			return 1;
	
	}
	
	return 0;
}	

L4_Word_t memoryserver_map_anon_pages_real(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  type, const L4_Word_t  virt_start_address, const L4_Word_t  size, idl4_server_environment * _env)
{
	char taskheader_entry = -1;
	
	//TODO: _caller == taskserver || threadid
	//TODO: validate virt_start_address

	//find taskheader entry
	taskheader_entry = findOrCreateTaskEntry(get_task_id(*threadid));

	L4_Word_t virt_end_address = virt_start_address + size;
	Taskheader_t *myTaskheader = &taskList[taskheader_entry];	

	//check address overlappings 
	if(isAddressConflict(myTaskheader, virt_start_address, virt_end_address)) {
		return -1; // TODO define proper return values
    }

	//create new page entry for thread
	Page_entry_t pe = {NOT_YET_MAPPED, virt_start_address, size};
	myTaskheader->pages[++(myTaskheader->pages_index)] = pe;

	snprintf(logbuf, sizeof(logbuf), "[MEMORY] Memory mapped for threadid %i at %x\n", *threadid, virt_start_address);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuf, &env);

	return 0;
}

L4_Word_t  memoryserver_map_file_pages_real(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  type, const path_t  path, const L4_Word_t  offset, const L4_Word_t  virt_start_address, const L4_Word_t  size, const L4_Word_t  realsize, idl4_server_environment * _env)
{
	char taskheader_entry = -1;
	
	//TODO: _caller == taskserver || threadid
	//TODO: validate virt_start_address

	//find taskheader entry
	taskheader_entry = findOrCreateTaskEntry(get_task_id(*threadid));

	L4_Word_t virt_end_address = virt_start_address + size;
	Taskheader_t *myTaskheader = &taskList[taskheader_entry];	

	//check address overlappings 
	if(isAddressConflict(myTaskheader, virt_start_address, virt_end_address)) {
		return -1; // TODO define proper return values
    	}

	//create new file entry for thread
	File_entry_t fe = {virt_start_address, path, offset, size, realsize, L4_Nilpage};
	myTaskheader->filemaps[++(myTaskheader->filemaps_index)] = fe;

	snprintf(logbuf, sizeof(logbuf), "[MEMORY] Memory mapped for threadid %i at %x\n", *threadid, virt_start_address);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuf, &env);

	return 0;

}

void  memoryserver_pagefault_real(CORBA_Object  _caller, const L4_Word_t  address, const L4_Word_t  ip, const L4_Word_t  privileges, idl4_fpage_t * page, idl4_server_environment * _env)
{
	Page_entry_t *pe = NULL;
	Page_entry_t *tmp_pe = NULL;
	File_entry_t *fe = NULL;
	File_entry_t *tmp_fe = NULL;
	int taskListIndex = -1;

	//search mapping
	for(int i=0; i<Taskheader_index; i = i+1)
	{
		if(taskList[i].taskid == get_task_id(_caller))
		{
			taskListIndex = i;
			break;
		}
	}

	if(taskListIndex == -1)
		return;

	for(int i=0; i<taskList[taskListIndex].pages_index; i = i+1)
	{
		tmp_pe = &(taskList[taskListIndex].pages[i]);

		if(address >= pe->virt_address &&
			address <= (pe->virt_address + pe->size))
		{
			pe = tmp_pe;
			break;
		}
	}
	
	if(pe == NULL)
	{
		for(int i=0; i<taskList[taskListIndex].filemaps_index; i = i+1)
		{
			tmp_fe = &(taskList[taskListIndex].filemaps[i]);

			if(address >= fe->virt_address &&
				address <= (fe->virt_address + fe->size))
			{
				fe = tmp_fe;
				break;
			}
		}
	}

	if(pe == NULL && fe == NULL)
		return;

	L4_Word_t size;
	L4_Word_t virt_address;

	if(pe == NULL)
	{
		size = fe->size;
		virt_address = fe->virt_address;
	}
	else
	{
		size = pe->size;
		virt_address = pe->virt_address;
	}
	
	//TODO:Do we have to ensure that the newpage is aligned?
	L4_Fpage_t newpage = L4_Fpage(-1, size); 
	
	/* Send mapitem, unless the recipient resides the same address space */
	if (!L4_IsLocalId(_caller))
	{
		idl4_fpage_set_base(page, virt_address);
		idl4_fpage_set_mode(page, IDL4_MODE_MAP);
		idl4_fpage_set_page(page, L4_Sigma0_GetPage(sigma0id, newpage)); 
		idl4_fpage_set_permissions(page, privileges); // or IDL4_PERM_READ|IDL4_PERM_WRITE|IDL4_PERM_EXECUTE
	}

	if(!L4_IsNilFpage(newpage))
	{	
		if(pe == NULL)
		{
			fe->page = newpage;

			buf_t buff;
			char tbuff[8];
			buff._buffer = (CORBA_char*)&tbuff;
			buff._maximum = 8;
			
			L4_Word_t fileid = IF_FILESERVER_get_file_id(fileserverid, fe->path, &env);

			int cnt = fe->realsize / 8;
			for(int i=0; i<=cnt; i++)
			{
				L4_Word_t inMax = 8;

			 	if(8*i > fe->realsize)
				{
					inMax = fe->realsize - 8*i;
				}
					
				L4_Word_t res_read = IF_FILESERVER_read(fileserverid, fileid, fe->offset + i*8, 8, &buff, &env);
				
				//fill page	
	    		memcpy((void*)(virt_address+ i*8), buff._buffer, inMax);
			}
			//fill out the rest with zero
	    	memset((void*)(virt_address + fe->realsize),0 ,fe->size - fe->realsize);
		}
		else
		{
			pe->base_address = L4_Address(newpage);	
		}
		snprintf(logbuf, sizeof(logbuf), "[MEMORY] Handled page fault for threadid %i at %x\n", _caller, address);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuf, &env);


	}

	return;
}

void  memoryserver_startup_real(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  ip, const L4_Word_t  sp, idl4_server_environment * _env)
{
	snprintf(logbuf, sizeof(logbuf), "[MEMORY] Starting thread %i ip %x sp %x\n", *threadid, ip, sp);
	IF_LOGGING_LogMessage((CORBA_Object)loggerid, logbuf, &env);

    	/* send startup IPC */
    	L4_Msg_t msg;
    	L4_Clear (&msg);
    	L4_Append (&msg, ip);
    	L4_Append (&msg, sp);
    	L4_Load (&msg);
    	L4_Send (*threadid);

	IF_LOGGING_LogMessage((CORBA_Object)loggerid, "[MEMORY] Thread started", &env);
}


