#!/bin/sh
if [ ! -e Makefile ]
then
echo "run ./build.sh first!"
fi
WANT_AUTOCONF_2_5="1" WANT_AUTOMAKE_1_6="1" make | tee build.log 2>&1 && make install && min --no-cui

