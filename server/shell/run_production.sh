#!/bin/bash

#update repo
hg pull; hg update
# make sure gnomescroll user owns the folder
chown -R gnomescroll:gnomescroll .
# compile & copy
sudo -u gnomescroll "./waf production; ./waf; cp run gnomescroll_server"
# stop the run script
kill gnomescroll_server_py
# stuff should save to disk. hopefully is done quick enough (should be done in a few seconds...)
sleep 15
# destroy the old screen
./shell/killscreens.sh gnomescroll_server
# create a detached screen with the forever script running inside 
sudo -u gnomescroll "screen -d -m -S gnomescroll_server ./shell/run_server_forever.py"
