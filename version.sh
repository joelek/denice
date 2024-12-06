#!/bin/sh

source ./version.env

if [ $# -eq 1 ] && ([ $1 = "major" ] || [ $1 = "minor" ] || [ $1 = "patch" ]); then
	echo "[updating version]"
else
	echo "usage: version <major|minor|patch>"
	exit 1
fi

echo "old version: $MAJOR.$MINOR.$PATCH"

if [ $1 = "major" ]; then
	MAJOR=$((MAJOR+1))
	MINOR=0
	PATCH=0
elif [ $1 = "minor" ]; then
	MAJOR=$((MAJOR+0))
	MINOR=$((MINOR+1))
	PATCH=0
elif [ $1 = "patch" ]; then
	MAJOR=$((MAJOR+0))
	MINOR=$((MINOR+0))
	PATCH=$((PATCH+1))
fi

echo "new version: $MAJOR.$MINOR.$PATCH"

EOF="\r\n"

rm version.env
printf "%s$EOF" "MAJOR=$MAJOR" >> version.env
printf "%s$EOF" "MINOR=$MINOR" >> version.env
printf "%s$EOF" "PATCH=$PATCH" >> version.env

git add version.env
git commit -m "$MAJOR.$MINOR.$PATCH"
git tag -a "v$MAJOR.$MINOR.$PATCH" -m "$MAJOR.$MINOR.$PATCH"
git push && git push --tags
