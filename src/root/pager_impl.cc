//
// File:  src/root/pager.cc
//
// Description: Pagerloop for root
//

#include <l4/message.h>
#include <l4/ipc.h>
#include <l4/thread.h>
#include <l4/sigma0.h>
#include <sdi/sdi.h>

#include <stdlib.h>

#include <idl4glue.h>

#include <l4io.h>
#include "root.h"


void  pager_pagefault_implementation_real(CORBA_Object  _caller, const L4_Word_t  address, const L4_Word_t  ip, const L4_Word_t  privileges, idl4_fpage_t * page, idl4_server_environment * _env)
{
	L4_Word_t page_bits = lsBit (L4_PageSizeMask ((L4_KernelInterfacePage_t*)L4_KernelInterface ()));

	//printf ("Pager got msg from %lx (%p, %p)\n",
	//    _caller.raw, (void *) address, (void *) ip);

	/* This is really ugly, we just touch this address to bring 
	the page into our address space */
	volatile char* dummy = (char *)address;
	*dummy;

	    
	/* Send mapitem, unless the recipient resides the same address space */
	if (!L4_IsLocalId(_caller))
	{
		idl4_fpage_set_base(page, address);
		idl4_fpage_set_mode(page, IDL4_MODE_MAP);
		idl4_fpage_set_page(page, L4_FpageLog2 (address, page_bits));
		idl4_fpage_set_permissions(page, IDL4_PERM_READ|IDL4_PERM_WRITE|IDL4_PERM_EXECUTE);
	}
}



