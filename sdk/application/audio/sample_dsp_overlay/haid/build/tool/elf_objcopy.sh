#!/bin/sh

if [ $# -lt 4 ]; then
    exit -1
fi

rm -rf $4
$1 --xtensa-core=$2 -O binary $3 $4

file_kb=$((`ls -l $4 | awk '{print $5}'`))
rem=`expr 16 - $file_kb % 16`

dd if=/dev/zero of=$4 bs=1 count=$rem seek=$file_kb