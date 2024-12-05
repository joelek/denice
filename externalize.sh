#!/bin/sh

if [ $# -eq 3 ]; then
	echo "[externalizing]"
else
	echo "usage: externalize <identifier> <source> <target>"
	exit 1
fi

IFS=""

rm $3

echo "const char* $1 = \"\"" >> $3

cat $2 | tr -d "\r" | while read -r LINE || [ -n "$LINE" ]; do
	echo "\"$LINE\\n\"" >> $3
done

echo "\"\";" >> $3
