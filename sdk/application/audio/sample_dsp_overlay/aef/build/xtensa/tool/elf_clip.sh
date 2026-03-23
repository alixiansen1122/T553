#!/bin/sh

if [ $# != 2 ]; then
    exit -1
fi

DIR=`dirname $0`
python3 $DIR/clip.py $*
