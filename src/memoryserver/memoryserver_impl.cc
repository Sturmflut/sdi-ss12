
#include "memoryserver.h"
#include <l4/sigma0.h>
#include <l4io.h>

#include <if/iflogging.h>
#include <if/iffileserver.h>
#include <if/ifmemoryserver.h>

Taskheader_t taskList[NUM_T_ENTRY];
unsigned char Taskheader_index;

void memoryserver_init() {
    while (L4_IsNilThread(loggerid)){
        loggerid = nameserver_lookup("/server/logger");
    }

    log_printf(loggerid, "[MEMORY] Registering");

    nameserver_register("/server/memory");

    log_printf(loggerid, "[MEMORY] Registered...");

    //initialize data
    Taskheader_index = 0;
    sigma0id = L4_GlobalId(48, 1); // TODO: hard coded for now

    log_printf(loggerid, "[MEMORY] sigma0id = %x", sigma0id);

    while (L4_IsNilThread(fileserverid)){
        fileserverid = nameserver_lookup("/file");
    }

    for (int i = 0; i < NUM_T_ENTRY; i++) {
        taskList[i].task_exists = false;
        taskList[i].anon_mapping_index = 0;
        taskList[i].file_mapping_index = 0;
    }
}

unsigned char findOrCreateTaskEntry(L4_Word_t taskid)
{
	char taskheader_entry = -1;

	for(int i = 0; i < NUM_T_ENTRY; i = i+1)
	{
		if (taskList[i].task_exists && taskList[i].taskid == taskid)
			taskheader_entry = i;
	}
	
	if(taskheader_entry == -1)
	{
        if(Taskheader_index < NUM_T_ENTRY) {
            taskheader_entry = Taskheader_index++;
            taskList[taskheader_entry].anon_mapping_index = 0;
            taskList[taskheader_entry].file_mapping_index = 0;
            taskList[taskheader_entry].taskid = taskid;
            taskList[taskheader_entry].task_exists = true;
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
	for(int i=0; i<myTaskheader->anon_mapping_index; i = i+1)
	{
		L4_Word_t mapped_virt_start_address = myTaskheader->anon_mappings[i].virt_address;
		L4_Word_t mapped_virt_end_address = mapped_virt_start_address + myTaskheader->anon_mappings[i].size;
		
		if( checkAddressOverlapping(virt_start_address, virt_end_address, mapped_virt_start_address, mapped_virt_end_address) == 0 )
			return 1;
		
	}	

	for(int i=0; i<myTaskheader->file_mapping_index; i = i+1)
	{
		L4_Word_t mapped_virt_start_address = myTaskheader->file_mappings[i].virt_address;
		L4_Word_t mapped_virt_end_address = mapped_virt_start_address + myTaskheader->file_mappings[i].size;

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
        log_printf(loggerid, "[MEMORY] Find or create task entry failed.");
        return -1;
    }

	L4_Word_t virt_end_address = virt_start_address + size;
	Taskheader_t *myTaskheader = &taskList[taskheader_entry];	

	//check address overlappings 
	if(isAddressConflict(myTaskheader, virt_start_address, virt_end_address)) {
        log_printf(loggerid, "[MEMORY] Address conflict.");
		return -1; // TODO define proper return values
    }

    if (myTaskheader->anon_mapping_index >= MAX_ANON_MAPPINGS) {
        panic("Max. anonymous mappings reached");
    }

	//create new page entry for thread
	Anon_mapping_t pe;
    pe.virt_address = virt_start_address;
    pe.size = size;
	
    myTaskheader->anon_mappings[(myTaskheader->anon_mapping_index)++] = pe;

	log_printf(loggerid, "[MEMORY] Anonymous memory mapped for threadid %p at %x", threadid->raw, virt_start_address);

	return 0;
}

L4_Word_t  memoryserver_map_file_pages_real(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  type, const path_t  path, const L4_Word_t  offset, const L4_Word_t  virt_start_address, const L4_Word_t  size, const L4_Word_t  realsize, idl4_server_environment * _env)
{
	char taskheader_entry = -1;
	
	//TODO: _caller == taskserver || threadid
	//TODO: validate virt_start_address

	//find taskheader entry
	taskheader_entry = findOrCreateTaskEntry(get_task_id(*threadid));

    log_printf(loggerid, "[MEMORY] Creating mapping for task_id=%d,thread_count=%d", get_task_id(*threadid), get_thread_count(*threadid));

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

    if (myTaskheader->file_mapping_index >= MAX_FILE_MAPPINGS) {
        panic("Max. file mappings reached");
    }
	
    //create new file entry for thread
	File_mapping_t fe; 
    fe.virt_address = virt_start_address;
    fe.offset = offset;
    fe.size = size;
    fe.realsize = realsize;
    strncpy(fe.path, path, ELEM_COUNT(fe.path));

    
	myTaskheader->file_mappings[(myTaskheader->file_mapping_index)++] = fe;

	log_printf(loggerid, "[MEMORY] Memory mapped for threadid %p at %x", threadid->raw, virt_start_address);

	return 0;

}

void  memoryserver_pagefault_real(CORBA_Object  _caller, const L4_Word_t  address, const L4_Word_t  ip, const L4_Word_t  privileges, idl4_fpage_t * page, idl4_server_environment * _env)
{
	Anon_mapping_t *pe = NULL;
	Anon_mapping_t *tmp_pe = NULL;
	File_mapping_t *fe = NULL;
	File_mapping_t *tmp_fe = NULL;
	int taskListIndex = -1;


//    log_printf(loggerid, "[MEMORY] Pagefault occured at %p. task_id=%d, thread_count=%d", 
//            address,
//            get_task_id(_caller),
//            get_thread_count(_caller));

    // search mapping
	for(int i=0; i < NUM_T_ENTRY; i++)
	{
		if(taskList[i].task_exists && taskList[i].taskid == get_task_id(_caller))
		{
			taskListIndex = i;
			break;
		}
	}

	if(taskListIndex == -1) {
        panic("PF: task not found");        
		return;
    }

	for(int i=0; i<taskList[taskListIndex].anon_mapping_index; i = i+1)
	{
		tmp_pe = &(taskList[taskListIndex].anon_mappings[i]);

		if(address >= tmp_pe->virt_address &&
			address <= (tmp_pe->virt_address + tmp_pe->size))
		{
			pe = tmp_pe;
			break;
		}
	}
	
	if(pe == NULL)
	{
		for(int i=0; i<taskList[taskListIndex].file_mapping_index; i = i+1)
		{
			tmp_fe = &(taskList[taskListIndex].file_mappings[i]);

			if(address >= tmp_fe->virt_address &&
				address <= (tmp_fe->virt_address + tmp_fe->size))
			{
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

    // 4k page
    L4_Fpage_t newpage = L4_Fpage(-1, PAGESIZE);
    L4_Word_t page_nr = (address - virt_address) / PAGESIZE;
    
    newpage = L4_Sigma0_GetPage(sigma0id, newpage);

    idl4_fpage_set_base(page, virt_address + page_nr * PAGESIZE);
    idl4_fpage_set_mode(page, IDL4_MODE_MAP);
    idl4_fpage_set_page(page, newpage); 
    idl4_fpage_set_permissions(page, IDL4_PERM_READ|IDL4_PERM_WRITE|IDL4_PERM_EXECUTE); 

    if (L4_IsNilFpage(newpage)) {
        panic("PF handler: new page is a nil page!");
    }

    if(fe != NULL)
    {
        if (address < virt_address + fe->realsize) {
            // region where we have to load file from file server
            buf_t buff;
            char tbuff[PAGESIZE];
            memset(tbuff, 0, PAGESIZE);

            buff._buffer = (CORBA_char*)tbuff;
            buff._maximum = PAGESIZE;


            L4_Word_t fileid = IF_FILESERVER_get_file_id(fileserverid, fe->path, &env);

            L4_Word_t in_max = PAGESIZE;

            if ((virt_address + page_nr * PAGESIZE) + PAGESIZE >= virt_address + fe->realsize) {
                // we are in the last block, so in_max has to be adjusted
                in_max = (virt_address + fe->realsize) - (virt_address + page_nr * PAGESIZE);
            }

            L4_Word_t res_read = IF_FILESERVER_read(
                    fileserverid, 
                    fileid, 
                    fe->offset + page_nr * PAGESIZE,
                    in_max, &buff, &env);

            memcpy((void *)L4_Address(newpage), buff._buffer, in_max);

        } else if (address >= virt_address + fe->realsize && address < virt_address + fe->size) 
        {
            // region where we have to fill with zeros
            memset((void *)L4_Address(newpage), 0, PAGESIZE); 
        }
    } 	
     

	return;
}

void  memoryserver_startup_real(CORBA_Object  _caller, const L4_ThreadId_t * threadid, const L4_Word_t  ip, const L4_Word_t  sp, idl4_server_environment * _env)
{
	log_printf(loggerid, "[MEMORY] Starting thread(raw) %p (%d,%d) ip %x sp %x", threadid->raw, get_task_id(*threadid), get_thread_count(*threadid), ip, sp);

    /* send startup IPC */
    L4_Msg_t msg;
    L4_Clear (&msg);
    L4_Append (&msg, ip);
    L4_Append (&msg, sp);
    L4_Load (&msg);
    L4_Send (*threadid);

	log_printf(loggerid, "[MEMORY] Thread started");
}


