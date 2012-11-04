#!/usr/bin/env bash

# find latest world

datapath=/usr/freespace/dc_mmo/server/world/
latest=`ls $datapath | sort -r | head -1`

if [ -z "$latest" ]; then
    exit 0
fi

# create backup

mkdir -p /usr/freespace/gsbackups/
date=`date +%m-%d-%Y-%R`
tar cvzf /usr/freespace/gsbackups/${latest}_${date}.tar.gz ${datapath}/${latest} --exclude="*.bak" --exclude="*.tmp"
