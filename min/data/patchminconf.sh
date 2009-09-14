#!/bin/sh
set -x
# $1 config file
# $2 module
# $3 test case file
CONFFILE="$1"
CONFSTR="ModuleName=$2"
TESTFILE="TestCaseFile=$3"

if [ ! -e $CONFFILE ] ; then
   cp /etc/min.conf $CONFFILE
fi

if [ $# -lt 2 ] ; then
    exit 1;
fi


if [ $# -eq 2 ] ; then
    grep $CONFSTR $CONFFILE
    if [ $? -eq 0 ] ; then
	exit 0;
    fi
    cp $CONFFILE $CONFFILE.backup
    echo "[New_Module]" >> $CONFFILE;
    echo $CONFSTR  >> $CONFFILE;
    echo "[End_Module]" >> $CONFFILE;
fi

if [ $# -eq 3 ] ; then
    grep $TESTFILE $CONFFILE
    if [ $? -eq 0 ] ; then
	exit 0;
    fi
    cp $CONFFILE $CONFFILE.backup
    echo "[New_Module]" >> $CONFFILE;
    echo $CONFSTR  >> $CONFFILE;
    echo $TESTFILE  >> $CONFFILE;
    echo "[End_Module]" >> $CONFFILE;
fi
