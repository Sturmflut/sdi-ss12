# This is used to create interface stubs from the specified IDL4 files
# Files are specified through IDLS and SRCS. Both ignoring anything else than .idl files!

include $(top_srcdir)/Mk/build.mk

#Portable way of converting SRCS/IDLS to IDLS

IDLINTERF=	${filter %.idl, ${SRCS}}
IDLINTERF+=	${filter %.idl, ${IDLS}}

IDLSTUBS =      $(subst .idl,.h,$(IDLINTERF))

do-all:		$(IDLSTUBS)

$(IDLSTUBS): $(IDLINTERF)
	@$(ECHO_MSG) client-stub : `echo $@ | sed s,^$(top_srcdir)/,,`
	$(IDL4) $(IDL4FLAGS) -c -h $@ $(srcdir)/$(subst .h,.idl,$@)

do-clean:	idl-clean

idl-clean:
	rm -f *~ \#* $(IDLSTUBS)

do-install:


