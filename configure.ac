dnl ###################################################################
dnl
dnl Copyright (C) ,  Karlsruhe University
dnl
dnl File path:     configure.in
dnl
dnl Redistribution and use in source and binary forms, with or without
dnl modification, are permitted provided that the following conditions
dnl are met:
dnl 1. Redistributions of source code must retain the above copyright
dnl    notice, this list of conditions and the following disclaimer.
dnl 2. Redistributions in binary form must reproduce the above copyright
dnl    notice, this list of conditions and the following disclaimer in the
dnl    documentation and/or other materials provided with the distribution.
dnl 
dnl THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
dnl ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
dnl IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
dnl ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
dnl FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
dnl DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
dnl OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
dnl HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
dnl LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
dnl OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
dnl SUCH DAMAGE.
dnl
dnl $Id: configure.in,v 1.18.2.23 2005/03/21 01:15:15 cvansch Exp $
dnl
dnl ###################################################################

dnl Process this file with autoconf to produce a configure script.

AC_INIT(sdiOS, [0.1])

if test "x$host_alias" != x; then
  # We do not necessarily need crt0 and friends when cross-compiling.
  LDFLAGS="-nostdlib $LDFLAGS"
  HOST=$host_alias
else
  HOST=`(uname -m) 2>/dev/null || echo unknown`
fi

# Append /l4 to the libdir default
case ${libdir} in
  \${exec_prefix}/lib)
	libdir='${exec_prefix}/lib/l4'
	;;
esac

# Append /l4 to the libexecdir default
case ${libexecdir} in
  \${exec_prefix}/libexec)
	libexecdir='${exec_prefix}/libexec/l4'
	;;
esac

# Determine hardware architecture to build for.
case $HOST in
  ia32|x86|i386*|i486*|i586*|i686*)
	ARCH=ia32
	ARCHSW=IA32
	BITS=32
	;;
  powerpc64*|ppc64*)
	ARCH=powerpc64
	ARCHSW=PPC64
	BITS=64
	;;
  powerpc*)
	ARCH=powerpc
	ARCHSW=PPC
	BITS=32
	;;
  amd64*)
	ARCH=amd64
	ARCHSW=AMD64
	BITS=64
	;;
  *)
	AC_MSG_ERROR([Unknown hardware architecture: $HOST.
	Please specify hardware architecture using --host=ARCH.])
	;;
esac


# Try guessing the correct name of the C compiler and friends.  If
# compiling for build machine, let autoconf do the job instead.
CROSSCC=
if test "x$CC" = x -a "x$host_alias" != x; then
  if test "x$gcc_prefix" = x; then
    case $ARCH in
      ia32)
	  gcc_prefix=i386
	  ;;
      amd64)
	  gcc_prefix=x86_64
	  ;;
      *)
	  gcc_prefix=$ARCH
	  ;;
    esac
  fi

  AC_CHECK_PROGS(CROSSCC,
	[$host_alias-gcc $gcc_prefix-elf-gcc $gcc_prefix-unknown-elf-gcc \
	$gcc_prefix-linux-gcc $gcc_prefix-unknown-linux-gcc \
	$gcc_prefix-unknown-linux-gnu-gcc $gcc_prefix-pc-linux-gnu-gcc])
fi

if test "x$CROSSCC" != x; then
  CC=$CROSSCC
fi

dnl Autoheader stuff.
AC_CONFIG_HEADERS([config.h])


dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_PROG_AWK

dnl Configure parameters
AC_ARG_WITH([l4dir],            
	[AC_HELP_STRING([--with-l4dir=DIR],
		[local directory of the L4 user headers and programs [[mandantory]]])],
	[L4DIR=$withval], [L4DIR=no])
AC_ARG_WITH([idl4dir],
	[AC_HELP_STRING([--with-idl4dir=DIR],
		[local directory of the idl compiler [[guess]]])],
	[IDL4DIR=$withval], [IDL4DIR=no])
AC_ARG_WITH([ROOTTASK_LINKBASE],
	[AC_HELP_STRING([--with-roottask_linkbase=BASE],
		[link-base for root task [[architecture-dependent]]])],
	[ROOTTASK_LINKBASE=$withval], [ROOTTASK_LINKBASE=no])

dnl Find prefix to use for gcc/binutils programs.
TOOLPREFIX=`echo $CC | sed 's,gcc[[^/]]*$,,'`
AC_SUBST([TOOLPREFIX])

dnl Find location of libgcc.a
AC_MSG_CHECKING([location of libgcc.a])
LIBGCC=`$CC -print-libgcc-file-name`
LIBGCCDIR=`echo $LIBGCC | sed 's,/[[^/]]*$,,'`
AC_MSG_RESULT([$LIBGCCDIR])

dnl Find location of stdarg.h include file
AC_MSG_CHECKING([location of stdarg.h])
testdir=`echo $LIBGCC | sed 's,/[[^/]]*$,/include,'`
if test -f "$testdir/stdarg.h"; then
  STDARGDIR=$testdir
else
  testdir=`echo $LIBGCC | sed 's,/lib/[[^/]]*$,/include,'`
  STDARGDIR="\$(top_builddir)"
  rm -f "stdarg.h"
  if test -f "$testdir/stdarg.h"; then
    # If stdarg.h is self contained, use it.  Copy file to avoid using
    # standard system include
    grep -q '#include' "$testdir/stdarg.h" || cp "$testdir/stdarg.h" .
  fi
  if test ! -f "stdarg.h"; then
    STDARGDIR="\$(top_srcdir)/../contrib/include"
  fi
fi

if test "x$STDARGDIR" != x; then
  AC_MSG_RESULT([$STDARGDIR])
else
  AC_MSG_ERROR([stdarg.h not found])
fi


dnl Checking for idl and stuff
case $IDL4DIR in 
     yes|no)
        AC_PATH_PROG([idl4config],[idl4-config])
        ;;
     *)
        AC_PATH_PROG([idl4config],[idl4-config],,["$IDL4DIR/bin"])
        ;;
esac    

if test "x$idl4config" = 'x'; then
   AC_MSG_ERROR([idl4-config missing])
fi

IDL4DIR="`$idl4config --prefix`"

AC_MSG_CHECKING([L4 directory])
if test "x$L4DIR" = xno; then
  AC_MSG_ERROR([directory of the L4 user headers and programs not set])
fi
AC_MSG_RESULT([yes])


dnl Set compiler/linker flags.
CPPFLAGS="-I\$(top_srcdir)/include -I\$(top_builddir) -I$L4DIR/include -I. -I$STDARGDIR -I$L4DIR/include -I$IDL4DIR/include -DCONFIG_ARCH_$ARCHSW $CPPFLAGS"
case $LDFLAGS in
  *-nostdlib*)
	;;
  *)
	LDFLAGS="-nostdlib $LDFLAGS"
	;;
esac
LDFLAGS="-N -L\$(top_builddir)/lib -L$LIBGCCDIR  -L$L4DIR/lib/l4 $LDFLAGS"
CFLAGS="-nostdinc $CFLAGS"
CXXFLAGS="-fno-exceptions $CFLAGS $CXXFLAGS"
IDL4INCLUDES="-I\$(top_srcdir)/if -I\$(top_srcdir)/include -I\$(top_builddir) -I$L4DIR/include"
IDL4FLAGS="$IDL4INCLUDES -mc++ -DCONFIG_ARCH_$ARCHSW --word-size=$BITS --interface=V4 --platform=generic"



AC_CHECK_HEADER(l4/kip.h,, AC_MSG_ERROR([L4 headers not found -- did you specify the correct L4 path?]),[#include <l4/types.h>])

dnl Architecture dependent settings.
case $ARCH in
  ia32)
	default_roottask_linkbase=00300000
	;;
  amd64)
       	default_roottask_linkbase=00A00000
	;;
esac

dnl Platform dependent settings.
case $HOST in
  amd64*)
	CFLAGS="$CFLAGS -m64 -mno-red-zone"
	;;
esac


case $ROOTTASK_LINKBASE in      
  yes|no)
	ROOTTASK_LINKBASE=$default_roottask_linkbase
	;;
esac

AC_SUBST([CXXFLAGS])
AC_SUBST([ARCH])
AC_SUBST(ARCHSW)
AC_SUBST([IDL4FLAGS])
AC_SUBST([IDL4DIR])
AC_SUBST([IDL4INCLUDES])
AC_SUBST([ROOTTASK_LINKBASE])


dnl Modified files.
AC_CONFIG_FILES([
config.mk
Makefile
if/Makefile
lib/Makefile
lib/sdi/Makefile
src/Makefile
src/root/Makefile
src/memoryserver/Makefile
src/fileserver/Makefile
src/nameserver/Makefile
src/simplethread1/Makefile

])
# src/nameserver/Makefile
# src/consoleserver/Makefile
# src/keyboarddriver/Makefile
# src/simplethread2/Makefile

AC_OUTPUT
