# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4

SUBDIRS = src tests mintests data pkgconfig shared man
DISTCLEANFILES = config.guess configure config.h.in compile missing ltmain.shaclocal.m4 depcomp config.sub build.log min/min.conf
install-data-local:
	mkdir -p $(DESTDIR)/usr/share/min/TestModuleTemplates
	cp -r TestModuleTemplates/*  $(DESTDIR)/usr/share/min/TestModuleTemplates
	cp ./src/utils/lua-5.1.3/src/lua.h $(DESTDIR)/usr/share/min/TestModuleTemplates/TemplateLuaScriptXXX/src
	cp ./src/utils/lua-5.1.3/src/lstate.h $(DESTDIR)/usr/share/min/TestModuleTemplates/TemplateLuaScriptXXX/src
	cp ./src/utils/lua-5.1.3/src/lobject.h $(DESTDIR)/usr/share/min/TestModuleTemplates/TemplateLuaScriptXXX/src
	cp ./src/utils/lua-5.1.3/src/lzio.h $(DESTDIR)/usr/share/min/TestModuleTemplates/TemplateLuaScriptXXX/src
	cp ./src/utils/lua-5.1.3/src/ltm.h $(DESTDIR)/usr/share/min/TestModuleTemplates/TemplateLuaScriptXXX/src
	cp ./src/utils/lua-5.1.3/src/lmem.h $(DESTDIR)/usr/share/min/TestModuleTemplates/TemplateLuaScriptXXX/src
	cp ./src/utils/lua-5.1.3/src/llimits.h $(DESTDIR)/usr/share/min/TestModuleTemplates/TemplateLuaScriptXXX/src
	cp ./src/utils/lua-5.1.3/src/luaconf.h $(DESTDIR)/usr/share/min/TestModuleTemplates/TemplateLuaScriptXXX/src
	find $(DESTDIR)/usr/share/min/TestModuleTemplates -name .svn | xargs rm -rf
	cp  TestModuleTemplates/createtestmodule $(DESTDIR)/usr/bin

tags:	
	 find . -name '*'.c | xargs etags -o TAGS  --declarations --members

distclean-local:
	find . -name Makefile.in | xargs rm
