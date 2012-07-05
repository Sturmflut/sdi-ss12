//
// File: include/new.h
//
// Description: Support for C++ new and delete
//
//
// Author: Ulf Vatter <uvatter@ira.uka.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#if !defined(__INCLUDE__NEW_H__)
#define __INCLUDE__NEW_H__

#include <types.h>
#include <stdlib.h>
// Very, very simple new and delete
// C++-Standart say, that new never returns 0, but throws an exeption
// We simulate this via an L4_KDB_Enter

extern inline void * operator new      (size_t size)
	{
		void* foo=malloc(size);
		if (!foo) L4_KDB_Enter("new: Out if memory");
		return foo;
	}

extern inline void * operator new[]    (size_t size)
	{
		void* foo=malloc(size);
		if (!foo) L4_KDB_Enter("new: Out if memory");
		return foo;
	}

extern inline void   operator delete   (void * p)    { free(p); return ;  }
extern inline void   operator delete[] (void * p)    { free(p); return ;  }

// The placement new is for manually triggering the constructor
extern inline void * operator new      (size_t size, void* placement) { return placement; };
extern inline void * operator new[]    (size_t size, void* placement) { return placement; };

// actually, these doesn't really make sense to call. Use obj->~obj() instead
extern inline void   operator delete   (void *, void*)    { };
extern inline void   operator delete[] (void *, void*)    { };


#endif

