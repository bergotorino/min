#!/bin/sh
if [ -e Makefile ] # run distclean everytime but the "first"
then
make distclean
fi
cp ./Makefile.am.build ./Makefile.am
./autogen.sh && \
./configure  --prefix=/usr --sysconfdir=/etc  && \
make && make install

