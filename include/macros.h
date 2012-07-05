//
// File: include/macros.h
//
// Description: Some usefull macros
//


#if !defined(__INCLUDE_MACROS_H__)
#define __INCLUDE_MACROS_H__


// MKSTR takes a Macroname, and turns the name into a string
// without this macro, the name would get replaced with its value
// ex.: printf(MKSTR(MYMACRO) " is: %d", MYMACRO);
#define    MKSTR(sym)    MKSTR2(sym)
#define    MKSTR2(sym)    #sym



// compute include path for diffrent architecutres (this is the only which is
// supported for now) ARCHs may be ia32 / ia64
#define INC_ARCH(x)             <__ARCH__/x>


#endif /* !__INCLUDE_MACROS_H__ */


