#!/usr/bin/python

import subprocess
import os
import platform
import sys

if platform.system() != "Linux":
    print "This script is only compatible with linux"
    sys.exit()
    
print "Starting eternal server"
MAX_DUMPS = 100
ct = 0
while True:
    print '%d-th server run' % ct
    subprocess.call(['./m643_run.sh'])
    print "Server died"
    ct += 1
    if ct >= MAX_DUMPS:
        print "Reached max core dumps."
        break
    if os.path.exists('./core'):
        print "Core dumped; saving"
        os.rename("./core", "~/coredumps/core%d" % (time.time()))

print "Exiting"
