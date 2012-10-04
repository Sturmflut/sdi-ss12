
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

	for(int i = 0; i < Taskheader_index; i = i+1)
	{
		if(taskList[i].taskid == taskid)
			taskheader_entry = i;
	}
	
	if(taskheader_entry == -1)
	{
        if(Taskheader_index < NUM_T_ENTRY) {
            taskheader_entry = Taskheader_index++;
            taskList[taskheader_entry].pages_index = 0;
            taskList[taskheader_entry].filemaps_index = 0;
            taskList[taskheader_entry].taskid = taskid;
        }
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

    if(taskheader_entry == -1){
        log_printf(loggerid, "[MEMORY] Find or create task entry failed.\n");
        return -1;
    }

	L4_Word_t virt_end_address = virt_start_address + size;
	Taskheader_t *myTaskheader = &taskList[taskheader_entry];	

	//check address overlappings 
	if(isAddressConflict(myTaskheader, virt_start_address, virt_end_address)) {
        log_printf(loggerid, "[MEMORY] Address conflict.\n");
		return -1; // TODO define proper return values
    }

	//create new page entry for thread
	Page_entry_t pe = {NOT_YET_MAPPED, virt_start_address, size};
	myTaskheader->pages[(myTaskheader->pages_index)++] = pe;

	log_printf(loggerid, "[MEMORY] Memory mapped for threadid %i at %x\n", *threadid, virt_start_address);

	return 0;
}

L4_Word_t  memoryserver_map_file_pages_real(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  type, const path_t  path, const L4_Word_t  offset, const L4_Word_t  virt_start_address, const L4_Word_t  size, const L4_Word_t  realsize, idl4_server_environment * _env)
{
	char taskheader_entry = -1;
	
	//TODO: _caller == taskserver || threadid
	//TODO: validate virt_start_address

	//find taskheader entry
	taskheader_entry = findOrCreateTaskEntry(get_task_id(*threadid));

    log_printf(loggerid, "Creating mapping for task_id=%d,thread_count=%d", get_task_id(*threadid), get_thread_count(*threadid));

    if(taskheader_entry == -1){
        log_printf(loggerid, "[MEMORY] Find or create task entry failed.\n");
        return -1;
    }

	L4_Word_t virt_end_address = virt_start_address + size;
	Taskheader_t *myTaskheader = &taskList[taskheader_entry];	

    //check address overlappings 
    if(isAddressConflict(myTaskheader, virt_start_address, virt_end_address)) {
        log_printf(loggerid, "[MEMORY] Address conflict.\n");
        return -1; // TODO define proper return values
    }

	//create new file entry for thread
	File_entry_t fe = {virt_start_address, path, offset, size, realsize, L4_Nilpage};
	myTaskheader->filemaps[(myTaskheader->filemaps_index)++] = fe;

	log_printf(loggerid, "[MEMORY] Memory mapped for threadid %i at %x\n", *threadid, virt_start_address);

	return 0;

}

void  memoryserver_pagefault_real(CORBA_Object  _caller, const L4_Word_t  address, const L4_Word_t  ip, const L4_Word_t  privileges, idl4_fpage_t * page, idl4_server_environment * _env)
{
	Page_entry_t *pe = NULL;
	Page_entry_t *tmp_pe = NULL;
	File_entry_t *fe = NULL;
	File_entry_t *tmp_fe = NULL;
	int taskListIndex = -1;

    log_printf(loggerid, "[MEMORY] Pagefault occured at %p", address);
    log_printf(loggerid, "PF handler: task_id=%d,thread_count=%d", get_task_id(_caller), get_thread_count(_caller));

	//search mapping
	for(int i=0; i<Taskheader_index; i = i+1)
	{
		if(taskList[i].taskid == get_task_id(_caller))
		{
			taskListIndex = i;
			break;
		}
	}

	if(taskListIndex == -1) {
        panic("PF: task not found");        
		return;
    }

	for(int i=0; i<taskList[taskListIndex].pages_index; i = i+1)
	{
		tmp_pe = &(taskList[taskListIndex].pages[i]);

		if(address >= tmp_pe->virt_address &&
			address <= (tmp_pe->virt_address + tmp_pe->size))
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

			if(address >= tmp_fe->virt_address &&
				address <= (tmp_fe->virt_address + tmp_fe->size))
			{
                log_printf(loggerid, ">>>>>> PF handler: found %x", tmp_fe->virt_address);
				fe = tmp_fe;
				break;
			}
		}
	}

	if(pe == NULL && fe == NULL) {
        /// TODO: kill thread instead
		panic("PF: Couldn't find mapping");
        return;
    }

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
    

    log_printf(loggerid, "before newpage");
    //We consider alignment and adjust the size of requested page
    //Filemappings: size * 2, anon pages: divide to 4k pages
	L4_Fpage_t newpage;
    if(pe == NULL){
        newpage = L4_Fpage(-1, size*2);
    }
    else {
        newpage = L4_Fpage(-1, size);
    }
	
    log_printf(loggerid, "after newpage");

	/* Send mapitem, unless the recipient resides the same address space */
	if (!L4_IsLocalId(_caller))
	{
        log_printf(loggerid, "before set_base");
		idl4_fpage_set_base(page, virt_address);
		idl4_fpage_set_mode(page, IDL4_MODE_MAP);
        log_printf(loggerid, "before sigma0 get, %x", sigma0id);
        newpage = L4_Sigma0_GetPage(sigma0id, newpage);
		idl4_fpage_set_page(page, newpage); 
        log_printf(loggerid, "after sigma0 get");

		idl4_fpage_set_permissions(page, privileges); // or IDL4_PERM_READ|IDL4_PERM_WRITE|IDL4_PERM_EXECUTE
	}   
    
	if(!L4_IsNilFpage(newpage))
	{	
		if(pe == NULL)
		{
			fe->page = newpage;

			buf_t buff;
			char tbuff[FILE_READ_BUFFER];
			buff._buffer = (CORBA_char*)&tbuff;
			buff._maximum = FILE_READ_BUFFER;
			
			L4_Word_t fileid = IF_FILESERVER_get_file_id(fileserverid, fe->path, &env);
            // TODO: on memcpy/memset: new page is aligned,
            // virt_start_address not. so the program data might land
            // before the virt_start_address in the task adress space.
            
            // TODO: we also do not know whether the new page will be
            // aligned to the upper or lower boundary.
            
            log_printf(loggerid, "MEMORY: starting reading file");

			int cnt = fe->realsize / FILE_READ_BUFFER;
			for(int i=0; i<=cnt; i++)
			{
				L4_Word_t inMax = FILE_READ_BUFFER;
                
                // the last chunk is only partially filled with data
			 	if (i == cnt)
				{
					inMax = fe->realsize - FILE_READ_BUFFER*i;
				}
					
				L4_Word_t res_read = IF_FILESERVER_read(fileserverid, fileid, fe->offset + i*FILE_READ_BUFFER, inMax, &buff, &env);
				
                log_printf(loggerid, "MEMORY: memcpy: dest=%p", L4_Address(newpage));
				//fill page	
	    		memcpy((void*)(L4_Address(newpage) + i*FILE_READ_BUFFER), buff._buffer, inMax);
			}


            log_printf(loggerid, "MEMORY: memset");
			//fill out the rest with zero
	    	memset((void*)(L4_Address(newpage) + fe->realsize), 0, fe->size - fe->realsize);
		}
		else
		{
			pe->base_address = L4_Address(newpage);	
		}
		log_printf(loggerid, "[MEMORY] Handled page fault for threadid %i at %x\n", _caller, address);
	} else {
        // TODO: kill task instead
        panic("PF handler: new page is a nil page!");
        return;
    }


	return;
}

void  memoryserver_startup_real(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  ip, const L4_Word_t  sp, idl4_server_environment * _env)
{
	log_printf(loggerid, "[MEMORY] Starting thread %i ip %x sp %x\n", *threadid, ip, sp);

    	/* send startup IPC */
    	L4_Msg_t msg;
    	L4_Clear (&msg);
    	L4_Append (&msg, ip);
    	L4_Append (&msg, sp);
    	L4_Load (&msg);
    	L4_Send (*threadid);

	log_printf(loggerid, "[MEMORY] Thread started");
}


