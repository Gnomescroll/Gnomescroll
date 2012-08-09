#!/bin/bash

p=`pidof run`

if [ -z "$p" ]
then
  p=`pidof gnomescroll_server`
  if [ -z "$p" ]
  then
    echo "No server process found"
  else
    kill -s SIGUSR1 `pidof gnomescroll_server`
  fi
else
  kill -s SIGUSR1 `pidof run`
fi
