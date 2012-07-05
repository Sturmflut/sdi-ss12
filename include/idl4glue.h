//
// File: include/idl4Glue.h
//
// Description: Include this before and idl4 include files
//
//
// Author: Ulf Vatter <uvatter@ira.uka.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef __INCLUDE__IDLE4GLUE_H__
#define __INCLUDE__IDLE4GLUE_H__

#include <stdlib.h>
/* import our alloc and free */
#include <sdi/sdi.h>

#define CORBA_alloc(X...) alloc(X)
#define CORBA_free(X...)  free(X)

#define CORBA_string_alloc (CORBA_char*)alloc
#define CORBA_string_free free

#define malloc alloc

#endif


