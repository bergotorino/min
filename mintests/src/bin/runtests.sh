#!/bin/sh

SLOGFILE=/tmp/min-tests/testrun-scripter.log
LLOGFILE=/tmp/min-tests/testrun-luascripter.log

mkdir -p /tmp/min-tests
numberfailed=0
min -cx /usr/lib/min/scripter.so:/usr/share/min-tests/scripter.cfg > $SLOGFILE
numberfailed=`expr $numberfailed + $?`; 
min -cx /usr/lib/min/luascripter.so:/usr/share/min-tests/luascripter.lua > $LLOGFILE
numberfailed=`expr $numberfailed + $?`; 
echo "$numberfailed failed MIN test cases"
test $numberfailed -eq 0;
