#!/bin/sh

if [ $# -eq 3 ]; then
	echo "[externalizing]"
else
	echo "usage: externalize <identifier> <source> <target>"
	exit 1
fi

IFS=""

rm $3

EOF="\r\n"

printf "%s$EOF" "const char* $1 = \"\"" >> $3

cat $2 | tr -d "\r" | while read -r LINE || [ -n "$LINE" ]; do
	printf "%s$EOF" "\"$LINE\\n\"" >> $3
done

printf "%s$EOF" "\"\\n\"" >> $3
printf "%s$EOF" "\"\";" >> $3
