#!/bin/sh
if [ ! -e Makefile ]
then
WANT_AUTOCONF_2_5="1" WANT_AUTOMAKE_1_6="1" make -f Makefile.cvs && ./configure --libdir=/usr/lib  --sysconfdir=/etc
fi
WANT_AUTOCONF_2_5="1" WANT_AUTOMAKE_1_6="1" make | tee build.log 2>&1 && make install

