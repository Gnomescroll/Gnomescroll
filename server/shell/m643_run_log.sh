#!/bin/bash

if [ -z "$2" ]
then
    ./gnomescroll_server production 2>&1 | tee $1
else
    ./gnomescroll_server production --map="$2" 2>&1 | tee $1
fi
