#!/bin/sh
if [ -e Makefile ] # run distclean everytime but the "first"
then
   make distclean
fi
cp ./Makefile.am.build ./Makefile.am
touch README ChangeLog AUTHORS NEWS
./autogen.sh && \
./configure CFLAGS='-g -Wall ' --prefix=/usr --sysconfdir=/etc  && \
make && make install

