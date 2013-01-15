#!/usr/bin/env bash

ext='prod'
if [ "$1" ];
then
  ext=$1
fi

secret=`./shell/make_secret_key.py`
echo $secret > ./keys/secret_key.$ext.new
echo $secret > /var/www/gnomescroll.com/data/gamekeys/secret_key.$ext
