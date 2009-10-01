#!/bin/bash
mkdir -p debian/tmp/usr/share/doc/min-doc/doxy.html
cd ../..
doxygen doxygenconf
cd -


