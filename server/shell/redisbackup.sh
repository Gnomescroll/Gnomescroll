#!/usr/bin/env bash

mkdir -p /root/gsdata/backups/redis
date=`date +%m-%d-%Y-%H-%M`
tar cvzf /root/gsdata/backups/redis/dump_${date}.rdb.tar.gz /var/lib/redis/dump.rdb
