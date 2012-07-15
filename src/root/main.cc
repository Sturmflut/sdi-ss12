//
// File:  src/root/main.cc
//
// Description: a VERY simple root task for sdi
//

#include <l4/thread.h>
#include <l4/space.h>
#include <l4/message.h>
#include <l4/ipc.h>
#include <l4/sigma0.h>
#include <l4/bootinfo.h>

#include <stdlib.h>

#include <sdi/sdi.h>

#include <l4io.h>
#include <elf.h>
#include "root.h"

#include <idl4glue.h>

#include <sdi/constants.h>

/* local threadids */
L4_ThreadId_t sigma0id;
L4_ThreadId_t pagerid;
L4_ThreadId_t loggerid;


L4_Word_t pagesize;
L4_Word_t utcbsize;
L4_Fpage_t kiparea;
L4_Fpage_t utcbarea;

/* helperstuff */

extern char __elf_start;
extern char __elf_end;

extern char __heap_start;
extern char __heap_end;


L4_Word_t logger_stack[1024];


L4_ThreadId_t start_thread (L4_ThreadId_t threadid, L4_Word_t ip, L4_Word_t sp, void* utcblocation) {
    printf ("New thread with ip:%lx / sp:%lx\n", ip, sp);
    /* do the ThreadControl call */
    if (!L4_ThreadControl (threadid, L4_Myself (),  L4_Myself (), pagerid,
		      (void*)utcblocation ))
	panic ("ThreadControl failed");
    /* set thread on our code */
    L4_Start (threadid, sp, ip);

    return threadid;
}

L4_ThreadId_t start_task (L4_ThreadId_t threadid, L4_Word_t ip, L4_Fpage_t nutcbarea) {
    printf ("New task with ip:%lx\n", ip);
    /* First ThreadControl to setup initial thread */
    if (!L4_ThreadControl (threadid, threadid, L4_Myself (), L4_nilthread, (void*)-1UL))
	panic ("ThreadControl failed\n");

    L4_Word_t dummy;

    if (!L4_SpaceControl (threadid, 0, L4_FpageLog2 (0xB0000000,14), 
			   utcbarea, L4_anythread, &dummy))
	panic ("SpaceControl failed\n");

    /* Second ThreadControl, activate thread */
    if (!L4_ThreadControl (threadid, threadid, L4_nilthread, L4_Myself (), 
			   (void*)L4_Address (nutcbarea)))
	panic ("ThreadControl failed\n");

    /* send startup IPC */
    L4_Msg_t msg;
    L4_Clear (&msg);
    L4_Append (&msg, ip);
    L4_Append (&msg, (0));
    L4_Load (&msg);
    L4_Send (threadid);

    return threadid;
}

L4_Bool_t request_page (L4_Word_t addr) {
    return !(L4_IsNilFpage (L4_Sigma0_GetPage (sigma0id,
					       L4_Fpage (addr, pagesize))));
}

void list_modules (const L4_BootInfo_t* bootinfo) {
    L4_BootRec_t* bootrec = L4_BootInfo_FirstEntry (bootinfo);
    for (unsigned int i=0; i < L4_BootInfo_Entries (bootinfo); i++){
	if((int)L4_Type (bootrec) == 1)
		printf ("Module: start %lx size %lx type: %d cmdline: %s\n", 
			L4_Module_Start (bootrec),
			L4_Module_Size (bootrec),
			(int)L4_Type (bootrec),
			L4_Module_Cmdline (bootrec));
		else
		printf ("Module: start %lx size %lx type: %d\n", 
			L4_Module_Start (bootrec),
			L4_Module_Size (bootrec),
			(int)L4_Type (bootrec));

	bootrec = L4_Next (bootrec);
    }
    
}

L4_BootRec_t* find_module (unsigned int index, const L4_BootInfo_t* bootinfo) {
    if (L4_BootInfo_Entries (bootinfo) < index) 
	panic ("Some modules are missing");
    L4_BootRec_t* bootrec = L4_BootInfo_FirstEntry (bootinfo);
    for (unsigned int i = 0; i < index; i++)
	bootrec = L4_Next (bootrec);
    return bootrec;
}

L4_BootRec_t* find_module_byname (char* name, const L4_BootInfo_t* bootinfo) {
    L4_BootRec_t* bootrec = L4_BootInfo_FirstEntry (bootinfo);
    int i = 0;

   do
   {
	if((int)L4_Type (bootrec) == 1 && 
		strncmp(L4_Module_Cmdline (bootrec), name, strlen(name)) == 0)
		return bootrec;

	if(i < L4_BootInfo_Entries (bootinfo))
		bootrec = L4_Next (bootrec);
	i++;
   } while(i < L4_BootInfo_Entries (bootinfo));

  panic ("Modules not found");
}

L4_Word_t load_elfimage (L4_BootRec_t* mod) {
    /* Check type of module */
    if (L4_Type (mod) != L4_BootInfo_Module)
	panic ("Wrong module type");
    /* Bring in the memory from sigma0 */
    for (L4_Word_t addr = L4_Module_Start (mod); 
	 addr < L4_Module_Start (mod) + L4_Module_Size (mod); 
	 addr += pagesize) 
	if (!request_page (addr)) {
	    panic ("could not get module pages from sigma0");
	}
    
    Elf32_Ehdr* hdr = (Elf32_Ehdr*)L4_Module_Start (mod);
    Elf32_Phdr* phdr;
    if ((hdr->e_ident[EI_MAG0] !=  ELFMAG0) || 
	(hdr->e_ident[EI_MAG1] !=  ELFMAG1) || 
	(hdr->e_ident[EI_MAG2] !=  ELFMAG2) ||
	(hdr->e_ident[EI_MAG3] !=  ELFMAG3)) {
	return NULL;
    }
    if (hdr->e_type != ET_EXEC) { return NULL; }
    if (hdr->e_machine != EM_386) { return NULL; }
    if (hdr->e_version != EV_CURRENT) { return NULL; }
    if (hdr->e_flags != 0) { return NULL; }
    phdr = (Elf32_Phdr *) (hdr->e_phoff + (unsigned int) hdr);
    if (hdr->e_phnum <= 0) {
	return NULL;
    }
    for (int i = 0; i < hdr->e_phnum; i++) {
	if (phdr[i].p_type == PT_LOAD) {
	    L4_Word_t fstart, mstart;
	    /* found a loadable section */

	    /* figure out the start and end of image in the "file" */
	    fstart = (L4_Word_t)hdr + phdr[i].p_offset;

	    /* figure out the start and end of the final image in memory */
	    mstart =  phdr[i].p_vaddr;

	    printf ("Module psection: fstart: %lx -> mstart: %lx\n", fstart, mstart);

	    /* copy the image of the file */
	    memcpy((void*) mstart, (void*) fstart, phdr[i].p_filesz);

	    /* zero fill the rest(if any) of the destination region */
	    memset((void*)(mstart+phdr[i].p_filesz),0,phdr[i].p_memsz - phdr[i].p_filesz);
	}
    }
    return (hdr->e_entry);
}

void start_task_byname(char* path, L4_ThreadId_t taskid, L4_Fpage_t nutcbarea)
{
    printf ("Starting %s ... \n", path);
    L4_BootRec_t* taskrec = find_module_byname (path, (L4_BootInfo_t*)L4_BootInfo (L4_KernelInterface ()));
    L4_Word_t startip = load_elfimage (taskrec);

    /* some ELF loading and staring */
    start_task (taskid, startip, utcbarea);
    printf ("%s started with thread id %lx\n", path, taskid.raw);
}

#define UTCBaddress(x) ((void*)(((L4_Word_t)L4_MyLocalId().raw + utcbsize * (x)) & ~(utcbsize - 1)))

int main(void) {
    L4_KernelInterfacePage_t* kip = (L4_KernelInterfacePage_t*)L4_KernelInterface ();

    pagerid = L4_Myself ();
    sigma0id = L4_Pager ();
    loggerid = L4_nilthread;

    printf ("Early system infos:\n");
    printf ("Threads: Myself:%lx Sigma0:%lx\n", L4_Myself ().raw, L4_Pager ().raw);
    pagesize = 1 << lsBit (L4_PageSizeMask (kip));
    printf ("Pagesize: %d\n", (int)pagesize);
    kiparea = L4_FpageLog2 ((L4_Word_t)kip, L4_KipAreaSizeLog2 (kip));
    printf ("KernelInterfacePage: %lx size: %d\n", L4_Address (kiparea), (int)L4_Size (kiparea));
    printf ("Bootinfo: %lx\n", L4_BootInfo (kip));
    printf ("ELFimage: from %p to %p\n", &__elf_start, &__elf_end);
    printf ("Heap: start: %p end: %p\n", &__heap_start, &__heap_end);
    utcbsize = L4_UtcbSize (kip);

    utcbarea = L4_FpageLog2 ((L4_Word_t) L4_MyLocalId ().raw,
			      L4_UtcbAreaSizeLog2 (kip) + 1);

    /* We just bring the in the memory of the bootinfo page */
    if (!request_page (L4_BootInfo (L4_KernelInterface ()))) {
	// no bootinfo, no chance, no future. Break up
	panic ("Was not able to get bootinfo");
    }
 
   /* Quick check */
    if (!L4_BootInfo_Valid ((void*)L4_BootInfo (L4_KernelInterface ()))) 
	panic ("Bootinfo not found");
    list_modules ((L4_BootInfo_t*)L4_BootInfo (L4_KernelInterface ()));


    /* Consoleserver */
    start_task_byname("(cd)/sdios/nameserver",
	L4_GlobalId ( SDI_NAMESERVER_DEFAULT_THREADID, 1),
	utcbarea);

    /* Consoleserver */
    start_task_byname("(cd)/sdios/driverserver",
	L4_GlobalId ( L4_ThreadNo (L4_Myself ()) + 20, 1),
	utcbarea);



    /* startup our logger, to be able to put messages on the screen */
    printf ("Starting logger ... \n");

    /* Generate some threadid */
    loggerid = L4_GlobalId ( L4_ThreadNo (L4_Myself ()) + 2, 1);
    start_thread (loggerid, 
		  (L4_Word_t)&logger_server, 
		  (L4_Word_t)&logger_stack[1023], 
		  UTCBaddress(2) ); 
    printf ("Started as id %lx\n", loggerid.raw);
    

    /* Console */
    start_task_byname("(cd)/sdios/consoleserver",
	L4_GlobalId ( L4_ThreadNo (L4_Myself ()) + 30, 1),
	utcbarea);
    

    /* Console */
    start_task_byname("(cd)/sdios/simplethread1",
	L4_GlobalId ( L4_ThreadNo (L4_Myself ()) + 3, 1),
	utcbarea);
    

    /* Console */
    start_task_byname("(cd)/sdios/simplethread2",
	L4_GlobalId ( L4_ThreadNo (L4_Myself ()) + 4, 1),
	utcbarea);



    /* now it is time to become the pager for all those threads we 
       created recently */
    pager_loop();

    panic ("Unexpected return from PagerLoop()");
}

