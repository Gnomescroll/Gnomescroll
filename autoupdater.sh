cd /root/freespace/dc_mmo_dev/server
hg pull
hg update
./waf clean
./waf optimized
./waf
./m643_run_dev.sh
