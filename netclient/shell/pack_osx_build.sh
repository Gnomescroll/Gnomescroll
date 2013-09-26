#!/usr/bin/env bash

VERSION_PATH=../src/c_lib/common/version.h
if [ ! -e ${VERSION_PATH} ]; then
    echo "version.h not found at: " ${VERSION_PATH}
    exit 1
fi

version=`cat ../src/c_lib/common/version.h | grep GS_VERSION | cut -d " " -f 3`
if [[ "$version" == */* ]]; then
    echo "Invalid version:" $version
    exit 1
fi
if [ -z "$version" ]; then
    echo "Invalid version:" $version
    exit 1
fi


ssh -t -t maslow@macmini <<'ENDSSH'
cd ~/dc_mmo/netclient
hg pull -u
./shell/create_osx_app.sh
exit 0
ENDSSH

scp maslow@macmini:~/Desktop/Gnomescroll.zip ./Gnomescroll.zip

ssh root@direct.gnomescroll.com 'mkdir /var/www/gnomescroll_downloads/'${version}'/'
scp ./Gnomescroll.zip root@direct.gnomescroll.com:/var/www/gnomescroll_downloads/"$version"/Gnomescroll.zip

rm Gnomescroll.zip
