#!/usr/bin/python

import subprocess
import os
import platform
import sys
import time

if platform.system() != "Linux":
    print "This script is only compatible with linux"
    sys.exit()

subprocess.Popen('ulimit -c unlimited', shell=True)

with open('../src/c_lib/common/version.h') as f:
    r = f.readlines()
    r = [g for g in r if 'DC_VERSION' in g]
    assert len(r) == 1
    v = r[0].strip()
    version = v.split(' ')[-1]
    version = int(version)

print "Server version is", version
print "Starting eternal server"
MAX_DUMPS = 100
ct = 0

while True:

    print '%d-th server run' % ct

    logname = 'log-%s-%d' % (version, time.time(),)
    corename = 'core-%s-%d' % (version, time.time())
    mapname = './world/map-%d.map' % (version,)
    if not os.path.exists(mapname):
        mapname = ''
    cmd = './shell/m643_run_log.sh %s %s' % (logname, mapname,)
    subprocess.call([cmd], shell=True)

    print "Server died"

    ct += 1
    if ct >= MAX_DUMPS:
        print "Reached max core dumps."
        break
    if os.path.exists('./core'):
        print "Core dumped; saving"
        os.rename("./core", "%s/coredumps/%s" % (os.path.expanduser('~'), corename,))
    if os.path.exists('./%s' % (logname,)):
        print "Saving %s" % (logname,)
        os.rename('./%s' % (logname,), '%s/coredumps/%s' % (os.path.expanduser('~'), logname,))

subprocess.Popen('ulimit -c 0', shell=True)

print "Exiting"
