#!/bin/sh
if [ -e Makefile ] # run distclean everytime but the "first"
then
   make distclean
fi
./autogen.sh && \
./configure CFLAGS='-g -Wall' --prefix=/usr  && \
make | tee build.log 2>&1


