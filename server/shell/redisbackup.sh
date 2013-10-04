#!/usr/bin/env bash

mkdir -p /home/gnomescroll/gsdata/backups/redis
date=`date +%m-%d-%Y-%H-%M`
tar cvzf /home/gnomescroll/gsdata/backups/redis/dump_${date}.rdb.tar.gz /var/lib/redis/dump.rdb
