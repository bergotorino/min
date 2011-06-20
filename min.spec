Name:           min
Version:        2011w25
Release:        1%{?dist}
Summary:        A test framework for C/C++

Group:          Development/Tools
License:        GPL
URL:            http://min.sourceforge.net
Source0:         %{name}_%{version}.tar.gz
BuildRoot:      %(mktemp -ud %{_tmppath}/%{name}_%{version}-%{release}-XXXXXX)

BuildRequires:  libtool, autoconf, chrpath, ncurses-devel, python-devel, dbus-devel, dbus-glib-devel, doxygen, qt-devel, qt, qt-sqlite, fdupes
Requires:       ncurses

%description
The MIN test framework provides a toolset for creating and executing test cases. Test cases are compiled into module(s).
 MIN engine starts a Test Module Controller (TMC) for each test module. TMC in turn forks a process for each test case when the test is executed. 
 Tests are run in own process so that e.g. segfaults can be caught and handled. MIN can be operated via console UI (ncurses) or from the commandline.

%package devel
Summary:        MIN headers and test module template wizard
Requires:       %{name} = %{version}-%{release}

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
Requires: min, min-pythonmodule

%description ci-tests
This package includes MIN test modules that can be used to test MIN itself.

%package dbusplugin
Summary: Dbus plugin for MIN
Requires: min

%description dbusplugin
Allows interaction with MIN through DBus intreface.

%package qt
Summary: Qt GUI for MIN

%description qt
MIN graphical user interface written in Qt.

%package qt-maemo
Summary: Qt GUI for MIN in maemo environment

%description qt-maemo
MIN graphical user interface written in Qt.

%prep
%setup -q -n %{name}_%{version}

%build
cd min
autoreconf --install
%configure  --libdir=%{_libdir} --prefix=/usr --sysconfdir=/etc
make %{?_smp_mflags}
cd ..
cd minpythonmodule
autoreconf --install
%configure --libdir=%{_libdir} --prefix=/usr --sysconfdir=/etc
make %{?_smp_mflags}
cd ..
cd mintests 
autoreconf --install
%configure --libdir=%{_libdir} --prefix=/usr --sysconfdir=/etc
make %{?_smp_mflags}
cd ..
cd mincitests 
autoreconf --install
%configure --libdir=%{_libdir} --prefix=/usr --sysconfdir=/etc
make %{?_smp_mflags}
cd ..
cd mindbusplugin
autoreconf --install
%configure --libdir=%{_libdir} --prefix=/usr --sysconfdir=/etc
make %{?_smp_mflags}
cd ..
cd minqt
%ifarch ARCHITECTURE_NAME
export DEB_HOST_ARCH=i386; qmake
%else
export DEB_HOST_ARCH=arm; qmake
%endif
make %{?_smp_mflags}
cd ..
cd mincitests/qsrc
qmake
make %{?_smp_mflags}
cd ../..

%install
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
rm %{buildroot}%{_libdir}/*.a
rm %{buildroot}%{_libdir}/*.la
rm %{buildroot}%{_libdir}/min/*.a
rm %{buildroot}%{_libdir}/min/*.la
%fdupes %{buildroot}%{_datadir}/min/

%post
ldconfig

%post devel
ldconfig

%postun
ldconfig

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
%config %{_sysconfdir}/min.conf
%config %{_sysconfdir}/event.d/min
%config %{_sysconfdir}/init.d/min

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
%config %{_sysconfdir}/min.d/min-ci-tests.min.conf
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
%{_bindir}/check_cli.sh

%files tests
%defattr(-,root,root,-)
%{_libdir}/min-tests/EventsTestModule*
%{_libdir}/min-tests/HardCoded*
%{_libdir}/min-tests/TestClass*
%{_libdir}/min-tests/LuaTestClass*
%{_datadir}/min-tests/scripter.cfg
%{_datadir}/min-tests/luascripter.lua
%{_datadir}/min-tests/runtests.sh

 
