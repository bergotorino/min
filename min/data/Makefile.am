bin_SCRIPTS=patchminconf.sh mindelmod.pl
sysconf_DATA=min.conf
min.conf:
	echo [Engine_Defaults] > min.conf
	echo ModSearchPath=$(mintestmoddir) >> min.conf
	echo ModSearchPath='$$'HOME/.min >> min.conf
	echo [End_Defaults] >> min.conf
	echo [New_Module] >> min.conf
	echo ModuleName=minDemoModule >> min.conf
	echo [End_Module] >> min.conf
	echo "" >> min.conf
	echo [New_Module] >> min.conf
	echo ModuleName=scripter_cli >> min.conf
	echo [End_Module] >> min.conf
	echo "" >> min.conf
	echo [Logger_Defaults] >> min.conf
	echo EmulatorBasePath=/tmp >> min.conf
	echo EmulatorFormat=TXT >> min.conf
	echo EmulatorOutput=SYSLOG >> min.conf
	echo ThreadIdToLogFile=NO >> min.conf
	echo [End_Logger_Defaults] >> min.conf

install-data-local:
	mkdir -p $(DESTDIR)/etc/event.d/
	cp min.event $(DESTDIR)/etc/event.d/min
	mkdir -p $(DESTDIR)/etc/init.d/
	cp min.init $(DESTDIR)/etc/init.d/min

clean: 
	-rm min.conf
