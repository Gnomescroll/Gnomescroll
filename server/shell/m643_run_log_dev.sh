#!/bin/bash

echo $PWD
ls ./settings

if [ -z "$2" ]
then
    ./gnomescroll_server ./settings/remote_dev.lua 2>&1 | tee $1
else
    ./gnomescroll_server ./settings/remote_dev.lua --map="$2" 2>&1 | tee $1
fi
