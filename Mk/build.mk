######################################################################
##                
## Copyright (C) 2003-2005, 2009-2010,  Karlsruhe University
##                
## File path:     build.mk
## Description:   Rules for compilation
##                
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
## 
## THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
## ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
## FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
## DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
## OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
## LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
## OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
## SUCH DAMAGE.
##                
## $Id: build.mk,v 1.9.2.3 2004/04/28 18:02:52 skoglund Exp $
##                
######################################################################

VPATH=		$(srcdir)
MKFILE_DEPS=	Makefile \
		$(top_srcdir)/Mk/base.mk \
		$(top_srcdir)/Mk/build.mk \
		$(top_srcdir)/Mk/lib.mk \
		$(top_srcdir)/Mk/prog.mk \
		$(top_srcdir)/Mk/idl.mk \
		$(top_builddir)/config.mk


# Portable way of converting SRCS to OBJS

_CC_OBJS=	${filter-out %.c %.S, ${SRCS}}
CC_OBJS=	$(_CC_OBJS:.cc=.o)
_C_OBJS=	${filter-out %.S %cc, ${SRCS}}
C_OBJS=		$(_C_OBJS:.c=.o)
_S_OBJS=	${filter-out %.c %cc, ${SRCS}}
S_OBJS=		$(_S_OBJS:.S=.o)

_IDL_STUBS=     ${filter %.idl, ${IDLS}}

OBJS+=		$(CC_OBJS) $(C_OBJS) $(S_OBJS) \
		${SRCS:C/.(cc|c|S)$/.o/g}

SERV_STUBS+=  $(subst .idl,-server.h,$(_IDL_STUBS))
CLIE_STUBS+=  $(subst .idl,-client.h,$(_IDL_STUBS))

# Compile .cc, .c and .S files

$(CC_OBJS) : %.o : %.cc $(MKFILE_DEPS) %.dep
	@$(ECHO_MSG) `echo $< | sed s,^$(top_srcdir)/,,`
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(C_OBJS) : %.o : %.c $(MKFILE_DEPS) %.dep
	@$(ECHO_MSG) `echo $< | sed s,^$(top_srcdir)/,,`
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(S_OBJS) : %.o : %.S $(MKFILE_DEPS) %.dep
	@$(ECHO_MSG) `echo $< | sed s,^$(top_srcdir)/,,`
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# IDL client stub code generation ...

$(SERV_STUBS) : %-server.h : %.idl $(MKFILE_DEPS) %.idl.dep
		@$(ECHO_MSG) server-stub : $@
		$(IDL4) $(IDL4FLAGS) -s -h $@ $(srcdir)/$(subst -server.h,.idl,$@)

$(CLIE_STUBS) : %-client.h : %.idl $(MKFILE_DEPS) %.idl.dep
		@$(ECHO_MSG) client-stub : $@ 
		$(IDL4) $(IDL4FLAGS) -c -h $@ $(srcdir)/$(subst -client.h,.idl,$@)

# Create dependency rules for .cc, .c, .S, .idl files.

CC_DEPS=	$(CC_OBJS:.o=.dep)
C_DEPS=		$(C_OBJS:.o=.dep)
S_DEPS=		$(S_OBJS:.o=.dep)
IDL_DEPS=	$(_IDL_STUBS:.idl=.idl.dep)
DEPS=		$(CC_DEPS) $(C_DEPS) $(S_DEPS) $(_IDL_STUBS)


$(CC_DEPS) : %.dep : %.cc
	@$(CPP) $(CPPFLAGS) -M -MG -o - $< | sed -e 's,^\(.*[^\]\):,\1 $@ : ,' > $@

$(C_DEPS) : %.dep : %.c
	@$(CPP) $(CPPFLAGS) -M -MG -o - $< | sed -e 's,^\(.*[^\]\):,\1 $@ : ,' > $@

$(S_DEPS) : %.dep : %.S
	@$(CPP) $(CPPFLAGS) -M -MG -o - $< | sed -e 's,^\(.*[^\]\):,\1 $@ : ,' > $@

 

$(IDL_DEPS) : %.idl.dep : %.idl
	dst="$@"; base="$${dst%.idl.dep}"; sed -e 's,^import\([[:space:]]*"\),#include\1,' $< | \
	$(CPP) $(CPPFLAGS) $(IDL4INCLUDES) -M -MG \
	  -MT"$${base}.h $${base}-client.h $${base}-server.h $@"  -o - - | \
	sed -e 's,\(.*[^\]\)$$,\1\\,' > $@; \
	echo "$<" >> $@

sinclude $(CC_DEPS) $(C_DEPS) $(S_DEPS) $(IDL_DEPS)

Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	@$(ECHO_MSG) Rebuilding `echo $(srcdir)/ | sed s,^$(top_srcdir)/*,,`$@
	@(cd $(top_builddir) && \
	  CONFIG_HEADER= \
	  CONFIG_LINKS= \
	  CONFIG_FILES=`echo $(srcdir)/ | sed s,^$(top_srcdir)/*,,`$@ \
	  $(SHELL) ./config.status)

$(top_builddir)/config.mk: $(top_srcdir)/config.mk.in $(top_builddir)/config.status
	@$(ECHO_MSG) Rebuilding config.mk
	@(cd $(top_builddir) && \
	  CONFIG_HEADER= \
	  CONFIG_LINKS= \
	  CONFIG_FILES=config.mk \
	  $(SHELL) ./config.status)

$(top_builddir)/config.status: $(top_srcdir)/configure
	(cd $(top_builddir) && $(SHELL) ./config.status --recheck)

$(top_srcdir)/configure: $(top_srcdir)/configure.ac
	@$(ECHO_MSG) Rebuilding configure
	@(cd $(top_srcdir) && $(AUTOCONF))
