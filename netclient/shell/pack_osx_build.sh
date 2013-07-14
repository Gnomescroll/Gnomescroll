#!/usr/bin/env bash

version=`cat ../src/c_lib/common/version.h | grep GS_VERSION | cut -d " " -f 3`
if [[ $version == */* ]]; then
    echo "Invalid version:" $version
    exit 1
fi

ssh -t -t maslow@macmini <<'ENDSSH'
cd ~/dc_mmo/netclient
hg pull -u
./shell/create_osx_app.sh
ENDSSH

scp maslow@macmini:~/Desktop/Gnomescroll.zip ./Gnomescroll.zip

rsync -v -e ssh ./Gnomescroll.zip root@m643.com:/usr/freespace/gnomescroll_downloads/"$version"/Gnomescroll.zip

rm Gnomescroll.zip
