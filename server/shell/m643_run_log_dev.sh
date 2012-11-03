#!/bin/bash

echo $PWD
ls ./settings

if [ -z "$2" ]
then
    ./gnomescroll_server remote_dev 2>&1 | tee $1
else
    ./gnomescroll_server remote_dev --map="$2" 2>&1 | tee $1
fi
