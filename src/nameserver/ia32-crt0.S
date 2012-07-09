//
// File:  src/root/ia32-crt0.S
//
// Description: Simple startupcode
//

	.text

	.global _start
_start:
	/* load the stack pointer for our small stack (see below) */
	leal	__stack, %esp

	/* clearing .bss segment, since we can not assume it is zero */
	leal	__section_bss_start,%esi
	mov	%esi,%edi
	inc	%edi
	leal	__section_bss_end,%ecx
	subl	%edi,%ecx
	movb	$0,(%esi)
	rep	movsb

        /* Start the constructors of global objects (supporting C++) */
        leal __section_ctors_start, %ebx
2:
        cmp $__section_ctors_end, %ebx
        je 3f
	call *(%ebx)
	add $4,%ebx
	jmp 2b
3:

	/* push a return address to catch main ending (should never happen) */
	pushl	$___return_from_main
	jmp	main /* call main() */

___return_from_main:
        /* run the destructors of global objects (supporting C++) */
        leal __section_dtors_start, %ebx
5:
        cmp $__section_dtors_end, %ebx
        je 6f
	call *(%ebx)
	add $4,%ebx
	jmp 5b
6:

	/* Enter kdebug (this is what never should happen) 
	WARNING: only L4Ka::Pistachio KDebug enter */
        int     $3
        jmp     8f
        mov     $7f, %eax
	.section .rodata
7:      .asciz "***System stopped***"
	.previous
8:      jmp 6b /* Loop forever on the enter_kdebug */

		
/* allocate 16*1024 bytes for the stack */
	.bss
	.space	16*1024
__stack:   /* this marks the top of the stack */

/* allocate 16*1024 bytes heap space and aditional pointers */
	.data
	.global __heap_ptr
__heap_ptr:	
	.long __heap_start
	.global __heap_start
__heap_start:	
	.space 16*1024
	.global __heap_end
__heap_end:

