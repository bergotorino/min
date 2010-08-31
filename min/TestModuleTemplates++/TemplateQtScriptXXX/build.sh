#!/bin/sh
touch README NEWS
qmake && make
mkdir ~/.min;
cp libTemplateQtScriptXXX.so ~/.min/libTemplateQtScriptXXX.so;
cp data/TemplateQtScriptXXX.cfg ~/.min/;
patchminconf.sh ~/.min/min.conf scripter TemplateQtScriptXXX.cfg;
