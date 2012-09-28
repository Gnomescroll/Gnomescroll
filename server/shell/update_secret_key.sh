#!/usr/bin/env bash

ext='prod'
if [ "$1" ];
then
  ext=$1
fi

secret=`./shell/make_secret_key.py`
echo $secret > ./data/secret_key.$ext.new
echo $secret > /var/www/gnomescroll.com/secret_key.$ext
