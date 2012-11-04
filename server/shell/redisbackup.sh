#!/usr/bin/env bash

mkdir -p /usr/freespace/gsbackups/redis
date=`date +%m-%d-%Y-%R`
tar cvzf /usr/freespace/gsbackups/redis/dump_${date}.rdb.tar.gz /var/lib/redis/dump.rdb
