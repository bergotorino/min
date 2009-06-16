#!/bin/sh
failures=0
min-clean.sh -f
min -h
failures=`expr $failures + $?`
min --help
failures=`expr $failures + $?`
min -v
failures=`expr $failures + $?`
min --version
failures=`expr $failures + $?`
min -ct "Demo_1"
failures=`expr $failures + $?`
min -console --title Demo_1
failures=`expr $failures + $?`
min --console --execute /usr/lib/min/minDemoModule.so
failures=`expr $failures + $?`
min -cx /usr/lib/min/minDemoModule.so
failures=`expr $failures + $?`
min -cr Demo
failures=`expr $failures + $?`
min -c -t Demo_1 --slave localhost:phone
failures=`expr $failures + $?`

test $failures -eq 0