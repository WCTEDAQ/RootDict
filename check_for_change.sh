#!/bin/bash
#set -x
for file in ./*; do
	fname=$(basename $file);
	if [ ! -f ../WCTEDAQ/DataModel/${fname} ]; then
		continue;
	fi;
	echo "comparing $fname";
	cmp $file ../WCTEDAQ/DataModel/${fname} &> /dev/null;
	if [ $? -ne 0 ]; then 
		echo "$fname changed";
		if [ $# -gt 0 ]; then
			git diff --no-index -w ${file} ../WCTEDAQ/DataModel/${fname}
		fi
	fi;
done




