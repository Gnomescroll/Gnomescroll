#!/bin/bash

# run backup script
/root/dc_mmo/server/shell/gsbackup.sh

function send_save_map
{
  local p=`pidof "$1"`
  if [ -z "$p" ]
  then
    return 1
  else
    kill -s SIGUSR1 "$p"
  fi
  return 0
}

if [ -z "$1" ]
then
  send_save_map "run"
  if [ $? -ne 0 ]
  then
    send_save_map "gnomescroll_server"
    if [ $? -ne 0 ]
    then
      echo "No server process found"
    fi
  fi
else
  send_save_map "$1"
fi
