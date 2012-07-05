/*
// C++ Linker file: 
//
// Description: makes the linker do usefull stuff
//
//
// Author: Ulf Vatter <uvatter@ira.uka.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
*/

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)

/* Where sould our programm start */
ENTRY(_start)


/* Basic layout of the file */
SECTIONS
{
	/* Build our text session (input: all .text section ; output: .text) */
    .text :
	{
                __elf_start = ABSOLUTE(.) ;
		__section_text_start = ABSOLUTE(.) ;
/*		ia32-crt0.o(.text*)   */                 /* The startup stuff first (just to make sure) */
		*(.text*)                              /* Now put all the other .text sections */
		*(.gnu.linkonce.t.*)                   /* templates */
		__section_text_end   = ABSOLUTE(.) ;
	}

	/* Read only data (e.g. static const) */
	.rodata :
	{
		__section_rodata_start = ABSOLUTE(.) ;
		*(.rodata*)
		*(.gnu.linkonce.r.*)                   /* templates */
		__section_rodata_end   = ABSOLUTE(.) ;
	} = 0

	/* Build our data section */
	. = ALIGN(4096) ;
	.data :
	{
		__section_data_start = ABSOLUTE(.) ;
		*(.data*)
		*(.gnu.linkonce.d.*)                   /* templates */
		__section_data_end   = ABSOLUTE(.) ;

	}

	. = ALIGN(4) ;
	/* This is for the pointers to the constructors of global/static objects */
	.ctors :
	{
		__section_ctors_start = ABSOLUTE(.) ;
		/* Make sure, they stay are kinda sorted and not garbage collected by the linker */
		KEEP(SORT(*)(.ctors*))
		__section_ctors_end = ABSOLUTE(.) ;
	}


	. = ALIGN(4) ;
	/* This is for the pointers to the destructors of global/static objects */
	.dtors :
	{
		__section_dtors_start = ABSOLUTE(.) ;
		KEEP(SORT(*)(.dtors*))
		__section_dtors_end = ABSOLUTE(.) ;
	}
	__section_load_end = . ;


	/* Put the bss section right behind, but aligned it to page size */
	. = ALIGN(4096) ;
	.bss : 
	{
		__section_bss_start = ABSOLUTE(.) ;
		*(.bss*)
		*(.gnu.linkonce.b.*)                   /* templates */
		__section_bss_end   = ABSOLUTE(.) ;
	        __elf_end = . ;
	}


}





