Name:           min
Version:        2010w24
Release:        1%{?dist}
Summary:        A test framework for C/C++

Group:          Development/Tools
License:        GPL
URL:            http://min.sourceforge.net
Source0:         %{name}_%{version}.tar.gz
BuildRoot:      %(mktemp -ud %{_tmppath}/%{name}_%{version}-%{release}-XXXXXX)

BuildRequires:  libtool, autoconf, chrpath, ncurses-devel, python-devel, dbus-devel, dbus-glib-devel, doxygen, qt-devel, qt, qt-sqlite
Requires:       ncurses

%description
The MIN test framework provides a toolset for creating and executing test cases. Test cases are compiled into module(s).
 MIN engine starts a Test Module Controller (TMC) for each test module. TMC in turn forks a process for each test case when the test is executed. 
 Tests are run in own process so that e.g. segfaults can be caught and handled. MIN can be operated via console UI (ncurses) or from the commandline.

%package devel
Summary:        MIN headers and test module template wizard
Requires:       min

%description devel
The headers and tools needed to develop MIN test modules.

%package pythonmodule
Summary:        Python scripter module for MIN
Requires:       min, python

%description pythonmodule
Python scripter for MIN.

%package tests
Summary:        MIN Basic Acceptance Tests
Requires:       min

%description tests
Tests for MIN.

%package ci-tests
Summary:  MIN tests for CI
Provides: Meego-CI-Packages(min)
Provides: Meego-CI-Stage(staging)
Provides: Meego-CI-Stage(acceptance)
Requires: min

%description ci-tests
This package includes MIN test modules that can be used to test MIN itself.

%package dbusplugin
Summary: dbus plugin for MIN.
Requires: min

%description dbusplugin
Allows interaction with MIN through DBus intreface.

%package qt
Summary:Qt GUI for MIN

%description qt
MIN graphical user interface written in Qt.

%package qt-maemo
Summary:Qt GUI for MIN in maemo environment.

%description qt-maemo
MIN graphical user interface written in Qt.

%prep
%setup -q -n %{name}_%{version}

%build
cd min
autoreconf --install
./configure  --libdir=%{_libdir} --prefix=/usr --sysconfdir=/etc
make %{?_smp_mflags}
cd ..
cd minpythonmodule
autoreconf --install
./configure --libdir=%{_libdir} --prefix=/usr --sysconfdir=/etc
make %{?_smp_mflags}
cd ..
cd mintests 
autoreconf --install
./configure --libdir=%{_libdir} --prefix=/usr --sysconfdir=/etc
make %{?_smp_mflags}
cd ..
cd mincitests 
autoreconf --install
./configure --libdir=%{_libdir} --prefix=/usr --sysconfdir=/etc
make %{?_smp_mflags}
cd ..
cd mindbusplugin
autoreconf --install
./configure --libdir=%{_libdir} --prefix=/usr --sysconfdir=/etc
make %{?_smp_mflags}
cd ..
cd minqt
%ifarch ARCHITECTURE_NAME
export DEB_HOST_ARCH=i386; qmake-qt4
%else
export DEB_HOST_ARCH=arm; qmake-qt4
%endif
make %{?_smp_mflags}
cd ..
cd mincitests/qsrc
qmake-qt4
make %{?_smp_mflags}
cd ../..

%install
rm -rf $RPM_BUILD_ROOT
cd min
make install DESTDIR=$RPM_BUILD_ROOT
cd ..
cd minpythonmodule
make install DESTDIR=$RPM_BUILD_ROOT
cd ..
cd mintests 
make install DESTDIR=$RPM_BUILD_ROOT
cd ..
cd mincitests 
make install DESTDIR=$RPM_BUILD_ROOT
cd ..
cd mindbusplugin
make install DESTDIR=$RPM_BUILD_ROOT
cd ..
cd minqt
make install INSTALL_ROOT=$RPM_BUILD_ROOT 
cd ..
cd mincitests/qsrc
make install INSTALL_ROOT=$RPM_BUILD_ROOT && cp -d *.so* $RPM_BUILD_ROOT%{_libdir}/min
cd ../..

%post
ldconfig

%post devel
ldconfig

%postun
ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files 
%defattr(-,root,root,-)
%doc
%{_bindir}/min
%{_bindir}/mind
%{_bindir}/tmc
%{_bindir}/min-clean.sh
%{_bindir}/patchminconf.sh 
%{_bindir}/mindelmod.pl 
%{_mandir}/man1/min.1.gz
%{_datadir}/%{name}/minbaselib.lua 
%{_mandir}/man5/min.conf.5.gz
%{_libdir}/min/luascripter*
%{_libdir}/min/scripter*
%{_libdir}/min/minDemoModule*
%{_libdir}/min/demoClass*
%{_libdir}/min/min_cui*
%{_libdir}/min/min_tcpip*
%{_libdir}/min/min_cli*
%{_libdir}/pkgconfig/min.pc
%{_libdir}/libminutils.so.*
%{_libdir}/libmintfwif.so.*
%{_libdir}/libminevent.so.*
%{_libdir}/libmintmapi.so.*
%{_libdir}/libmininterference.so.*
%{_sysconfdir}/min.conf
%{_sysconfdir}/event.d/min
%{_sysconfdir}/init.d/min

%files devel
%defattr(-,root,root,-)
%{_includedir}/min_logger.h
%{_includedir}/mintfwif.h
%{_includedir}/min_logger_output.h
%{_includedir}/min_unit_macros.h
%{_includedir}/dllist.h
%{_includedir}/min_parser_common.h
%{_includedir}/tec.h
%{_includedir}/min_common.h
%{_includedir}/min_parser.h
%{_includedir}/tec_rcp_handling.h
%{_includedir}/min_plugin_interface.h
%{_includedir}/min_eapi_protocol.h
%{_includedir}/test_module_api.h
%{_includedir}/min_engine_api.h
%{_includedir}/min_section_parser.h
%{_includedir}/min_settings.h
%{_includedir}/tllib.h
%{_includedir}/min_file_parser.h
%{_includedir}/min_system_logger.h
%{_includedir}/tmc_common.h
%{_includedir}/min_ipc_mechanism.h
%{_includedir}/min_test_event_if.h
%{_includedir}/tmc.h
%{_includedir}/min_item_parser.h
%{_includedir}/min_test_interference.h
%{_includedir}/tmc_ipc.h
%{_includedir}/min_logger_common.h
%{_includedir}/min_text.h
%{_includedir}/tmc_tpc.h
%{_mandir}/man1/createtestmodule.1.gz
%{_mandir}/man3/*
%{_datadir}/min/TestModuleTemplates
%{_datadir}/min/TestModuleTemplates++
%{_datadir}/min/minunit.c
%{_libdir}/pkgconfig/min-dev.pc
%{_bindir}/createtestmodule
%{_bindir}/createtestmodule++
%{_libdir}/libminutils.so
%{_libdir}/libmintfwif.so
%{_libdir}/libminevent.so
%{_libdir}/libmintmapi.so
%{_libdir}/libmininterference.so
%{_libdir}/libmin*.a
%{_libdir}/libmin*.la

%files pythonmodule
%defattr(-,root,root,-)
%{_libdir}/min/pythonscripter*
%{_libdir}/min/min_ext*

%files dbusplugin
%defattr(-,root,root,-)
%{_libdir}/min/min_dbus*
%{_includedir}/min_dbus_client_interface.h
%{_bindir}/mindbus.sh
%{_datadir}/dbus-1/services/org.maemo.MIN.service

%files qt
%defattr(-,root,root,-)
%{_bindir}/minqt

%files qt-maemo
%defattr(-,root,root,-)
%{_bindir}/minqt
%{_datadir}/applications/*
%{_datadir}/icons/*
%{_datadir}/pixmap/*


%files ci-tests
%defattr(-,root,root,-)
%{_sysconfdir}/min.d/min-ci-tests.min.conf
%{_libdir}/min/dllist_tests*
%{_libdir}/min/min_unit_selftest*
%{_libdir}/min/item_parser_tests*
%{_libdir}/min/logger_tests*
%{_libdir}/min/settings_tests*
%{_libdir}/min/minqtdbtests*
%{_libdir}/min/minqtdbtests.cfg
%{_libdir}/min/python_tests.py*
%{_datadir}/min-ci-tests/tests.xml
%{_datadir}/min-ci-tests/test.cfg
%{_bindir}/min_scripter_cli_test.sh

%files tests
%defattr(-,root,root,-)
%{_libdir}/min-tests/EventsTestModule*
%{_libdir}/min-tests/HardCoded*
%{_libdir}/min-tests/TestClass*
%{_libdir}/min-tests/LuaTestClass*
%{_datadir}/min-tests/scripter.cfg
%{_datadir}/min-tests/luascripter.lua
%{_datadir}/min-tests/runtests.sh

%changelog
* Mon Jun 14 2010 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2010w24
  - Fixed: still some rpmlint problems
* Thu Jun 10 2010 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2010w23
  - Fixed: MIN spec file rpmlint problems
  - Fixed: linking issues with MIN cui plugin
  - Fixed: sigsegv/sigabrt signal handlers (enable default action for core dumps)
* Wed Apr 21 2010 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2010w16
  - Added: parallel execution from commandline
  - Fixed: python parallel execution
  - Fixed: a TMC process continues running after exiting MIN.
* Fri Mar 12 2010 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2010w10
  - Added: Support for python2.6 in MIN python module.
  - Fixes: NB#157310 - testclass symbol validation can be turned off with env var.
  - Fixes: NB#156981 - MIN cui high cpu usage in hw.
* Wed Jan 27 2010 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2010w04
  - Fixed: MIN consoleUI does not work over serial line (minicom).
  - Fixed: logger settings overwriting only for min debug log.
  - Fixed: build rules for min-ci-tests:minqtdbtests
* Fri Jan 15 2010 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2010w03
  - Added: indication to consoleUI when the menu contains more than visible items.
  - Added: scripter command line interface for prototyping test scripts.
  - Added: Qt GUI displays previous test runs.
* Wed Dec 9 2009  Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2009w51   
  - Fixed: refactored validate_test_case() for smaller code complexity.
  - Fixed: refactored rcp-message handling so that it uses MIN Engine api
  - Added: test case description feature
* Mon Nov 9 2009  Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2009w46   
  - Fixed: Test case pausing fails in scratchbox ARM emulation.
  - Fixed: Compile warnings, most lintian warnings.
  - Added: min-ci-tests debian package for ci testing.
  - Added: min settings utility.
* Mon Oct 26 2009 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2009w44
  - Fixed: Entering "start new case" menu very fast right after MIN has started can cause crash
  - Fixed: Scripter test class method return value handled incorrectly.
  - Fixed: Scripter leaves processes behind.
  - Fixed: MIN prints invalid characters to syslog.
  - Fixed: MIN -ct gets stuck if non-existing test modules are configured.
  - Added: stdout and stderr outputs to MIN logger.
  - Added: Make test case execution possible scratchbox armel emulation (sbox2).
  - Added: Display case result in "all cases" view when the case has finnished.
  - Added: Qt GUI: Display number of errors in the label of log messages tab. 
* Mon Oct 12 2009 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2009w42
  - Fixed: Aborting paused test case fails.
  - Fixed: Scripter gets stuck trying to call methods from a crashed test class process.
  - Added: min-doc debian package.
  - Added: Log messages menu to Qt GUI.
  - Added: Debian packaging to test module templates.
  - Added: Aborting/pausing/resuming possible also from "all cases" view in Qt GUI.
* Mon Sep 28 2009 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2009w40
  - Fixed: The wrapper macro around strncpy  (STRCPY) and checked it's usage. 
  - Fixed: Remaining resource leaks reported by static analysis tool.
  - Fixed: Scripter: error in loop keyword syntax validation
  - Fixed: Bug in remote run handling.
  - Fixed: ConsoleUI: View output for ongoing case does not update automatically.
  - Added: Debian package for Embedded version of Qt GUI
  - Added: Mechanism to get the name of currently running test case from c-code.
* Mon Sep 14 2009 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2009w38
  - Fixed: Around 100 defects from static analysis tool.
  - Fixed: MIN Qt GUI: first test run fails to start sporadically.
  - Fixed: Crashes from test method not caught properly by scripter-module.
  - Fixed: MIN Qt GUI: menu titles more intuitive.
  - Fixed: MIN Qt GUI: problems in ongoing cases view.
  - Fixed: MIN crashes if .min directory does not exist.
  - Added: Qt test class for C++ Test Module Template Wizard.
* Mon Aug 31 2009 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2009w36
  - Fixed: Around 300 defects from static analysis tool.
  - Fixed: MIN Qt GUI missed start time for "ongoing cases".
  - Fixed: State event set causes crash when event not requested. 
  - Added: tcp/ip plugin for MIN and the use of plugin from GUI.
* Mon Aug 17 2009 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2009w34
  - Twelfth MIN Release
  - Fixed: Doxygen tags updated and harmonized.
  - Added: Test case debugging from ConsoleUI
* Mon Jul 6 2009 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2009w28        
  - Fixed: MIN Qt GUI has problems executing multiple cases. 
  - Fixed: Updated contact information.
  - Added: Improve the "Add test cases to test set" menu on ConsoleUI
  - Added: Timeout for blocking scripter commands.
  - Added: Support for debugging test cases: commandline mode.
* Mon Jun 22 2009 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2009w26
  - Fixed: Remote run error not handled properly on master side
  - Fixed: MIN events are not working properly when looping
  - Added: CLI interface plugin
* Wed Jun 10 2009 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2009w24
  - Fixed: MIN variable transmission between master and slave doesn't work on tpc/ip version (4145)
  - Fixed: Slave cannot execute test case when multiple test case files are defined (4146) 
  - Fixed: Bug in commandline option parsing --plugin does not accept short form (-p) (4148)
  - Fixed: MIN Qt does not work when module is configured twice (4177)
  - Fixed: MIN Qt works only once without killing MIN manually (4217)
  - Fixed: Test case reported as passed when it exits prematurely. (4225) 
  - Added: Possibility to register slave devices in /etc/min.d/.  (4181)
  - Added: Support C++ in Test Module Template Wizard (4150)
  - Added: IP slave registration to dbus plugin. (4180)
* Mon May 25 2009 Konrad Marek Zapalowicz <ext-konrad.zapalowicz@nokia.com> 2009w22
  - Fixed: ELF binaries from mintest package go to /usr/lib/min-tests instead of /usr/share/min-tests
  - Fixed: TestModule appended to the name of modules generated by createmodule script
  - Fixed ConsoleUI executed wrong test case
  - Fixed: qulonglong issue with DBus
  - Fixed: min_debug() does not work when called directly
  - Added: new features documented in MIN
* Tue Apr 14 2009 Konrad Marek Zapalowicz <ext-konrad.zapalowicz@nokia.com> 2009w16
  - Added: ip slave registration to MIN Engine API and a pool of slaves to Engine
  - Added: usability changes to MIN
* Tue Mar 24 2009 Konrad Marek Zapalowicz <ext-konrad.zapalowicz@nokia.com> 2009w13
  - Fixed: run keyword in scripter does not accept full path, only filename
  - Added: error log to console ui
  - Added: chapter about DBus interface to the documentation
  - Added: return values to eapi towards the engine
* Tue Mar 3  2009 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2009w10
  - Fixed: Scripter run keywords mixes up remote commands
  - Fixed: Scripter complete keyword works only once
  - Added: clean API towards the engine (eapi)
  - Added: consoleUI as plugin that uses the eapi
* Mon Feb 9  2009 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2009w07
  - Fixed: bug in timed scripter loops
  - Fixed: bug in compelete command syntax validation
  - Fixed: MIN stucks when cases are still running on exit
  - Fixed: Memory leaks in logger and syntax validation
  - Fixed: Removed non-reentrant function calls from signal handlers
  - Fixed: Logger does not segfaults when log message is too long
  - Added breakloop keyword to scripter
  - Added if-else statement support to scripter
  - Added scritpter internal variables (FAIL_COUNT etc.)
* Tue Jan 20 2009 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2009w04
  - Fixed: bug in scripter result reporting
  - Enablers for C++ in Test Modules added
  - Changed the default output to syslog
* Mon Dec 15 2008 Sampo Saaristo <ext-sampo.2.saaristo@nokia.com> 2008w51
  - Initial release
 
