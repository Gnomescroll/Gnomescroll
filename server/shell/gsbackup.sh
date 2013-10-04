#!/usr/bin/env bash

# find latest world

datapath=/home/gnomescroll/dc_mmo/server/world/
latest=`ls $datapath | sort -r | head -1`

if [ -z "$latest" ]; then
    exit 0
fi

# create backup

date=`date +%m-%d-%Y-%H-%M`
tar cvzf /home/gnomescroll/gsdata/backups/${latest}_${date}.tar.gz ${datapath}/${latest} --exclude="*.bak" --exclude="*.tmp"
