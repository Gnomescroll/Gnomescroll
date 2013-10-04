#!/usr/bin/python

import procname
import subprocess
import os
import platform
import sys
import time
import signal
import shlex

from get_version import get_version

procname.setprocname('gnomescroll_server_py')

current_process = None
current_process_id = None

def register_signals():

    def signal_handler(sig, frame):
        global current_process
        global current_process_id
        if current_process is not None:
            print "Forwarding signal %d to process %d" % (sig, current_process_id,)
            current_process.send_signal(sig)
            print "Waiting for server process to exit..."
            current_process.wait()
            print "Server process exited"
        if sig != signal.SIGUSR1 and sig != signal.SIGUSR2:
            sys.exit(0)

    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    signal.signal(signal.SIGUSR1, signal_handler)
    signal.signal(signal.SIGUSR2, signal_handler)

if platform.system() != "Linux":
    print "This script is only compatible with linux"
    sys.exit()

def run():

    global current_process
    global current_process_id

    register_signals()

    version = get_version()

    print "Server version is", version
    print "Starting eternal server"
    MAX_DUMPS = 100
    ct = 0

    subprocess.Popen('ulimit -c unlimited', shell=True)

    while True:

        print '%d-th server run' % ct

        logname = 'log-%s-%d' % (version, time.time(),)
        corename = 'core-%s-%d' % (version, time.time())
        mapname = './world/map-%d.map' % (version,)
        if not os.path.exists(mapname):
            mapname = './world/map-%d.map.bak' % (version,)
            if not os.path.exists(mapname):
                mapname = ''

        cmd = './shell/m643_run_log.sh %s %s' % (logname, mapname,)

        current_process = subprocess.Popen(shlex.split(cmd))
        current_process_id = current_process.pid
        current_process.wait()

        print "Server died"

        ct += 1
        if ct >= MAX_DUMPS:
            print "Reached max core dumps."
            break
        if os.path.exists("./core"):
            print "Core dumped; saving"
            os.rename("./core", "/home/gnomescroll/gsdata/coredumps/%s" % (corename,))
        if os.path.exists("./%s" % (logname,)):
            print "Saving %s" % (logname,)
            os.rename("./%s" % (logname,), "/home/gnomescroll/gsdata/coredumps/%s" % (logname,))

    subprocess.Popen('ulimit -c 0', shell=True)


if __name__ == '__main__':
    run()
    print "Exiting"
