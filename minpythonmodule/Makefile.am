# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4

SUBDIRS = min_ext py_module
DISTCLEANFILES = config.guess configure config.h.in compile missing ltmain.shaclocal.m4 depcomp config.sub build.log min/min.conf digsigsums


tags:	
	 find . -name '*'.c | xargs etags -o TAGS  --declarations --globals --defines --members

distclean-local:
	find . -name Makefile.in | xargs rm
