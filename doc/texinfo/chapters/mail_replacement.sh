#!/bin/bash

find /home/maemo/documentation/texinfo/chapters|grep -v svn|grep -v pdf |grep texi | while read y; do
 sed "s/"/\"/g" $y > /tmp/temp_mj_chap.txt; cat /tmp/temp_mj_chap.txt > $y;
done
