#!/bin/sh
set -x
# $1 config file
# $2 module
CONFFILE="$1"
CONFSTR="ModuleName=$2"

if [ ! -e $CONFFILE ] ; then
   cp /etc/min.conf $CONFFILE
fi

grep $CONFSTR $CONFFILE
if [ $? -ne 0 ] ; then
    cp $CONFFILE $CONFFILE.backup
    echo "[New_Module]" >> $CONFFILE;
    echo $CONFSTR  >> $CONFFILE;
    echo "[End_Module]" >> $CONFFILE;
fi

