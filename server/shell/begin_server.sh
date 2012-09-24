#!/bin/bash

# make sure gnomescroll user owns the folder
chown -R gnomescroll:gnomescroll .

# stop the run script
pid=`pidof gnomescroll_server_py gnomescroll_server`
if [ "$pid" ]
then
  kill $pid
fi

# wait until it all shuts down...
while [ `pidof gnomescroll_server gnomescroll_server_py` ]
do
    echo "waiting for old server shutdown..."
    sleep 1
done

# destroy the old screen
sudo -u gnomescroll ./shell/killscreens.sh gnomescroll_server

# create a detached screen with the forever script running inside
sudo -u gnomescroll /usr/bin/screen -d -m -S gnomescroll_server ./shell/run_server_forever.py  

echo "Started server in new screen for user: gnomescroll"

# To reattach to the screen,
#su - gnomescroll
#script /dev/null
#screen -r gnomescroll_server
