#!/bin/sh

echo $MAKEFLAGS > ~/toto
cd $EXACTNESS_DATA_PATH
if [ "$MAKEFLAGS" =~ -j([0-9]+) ]
then
   JOBS=$${BASH_REMATCH[1]}
else
   JOBS=1
fi
echo $JOBS >> ~/toto
exactness -j $EXACTNESS_JOBS -p -b recordings tests.txt

