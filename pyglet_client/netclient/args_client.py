#!/usr/bin/python

'''
Usage:

To integrate in existing code:

from this_module import get_args
args = get_args()

'''

'''
Available options:

version         Prints client version to STDOUT
server          Server IP Address
port            Server Port
quiet           Suppress STDOUT
cli             Command-line mode (no viewer)
no_player       Free-camera only. Will not create player / join game
admin           Enable admin commands
tick            Set game clock tick
name            Set player name (overrides settings.py name)

'''


import argparse
import settings

DC_VERSION = '0.2'

DEFAULTS = {
    'server'    :   '127.0.0.1',
    'port'      :   5055,
    'tick'      :   0.01,
    'name'      :   settings.name,
}

def parse(cl_args=None):
    parser = argparse.ArgumentParser(description="DC_MMO Netclient", prog="Dwarf Control (in Space)")

    vs = '%(prog)s ' + DC_VERSION
    parser.add_argument('-V', '--version', action='version', version=vs)

    parser.add_argument('-s', '--server', default=DEFAULTS['server'])

    parser.add_argument('-p', '--port', default=DEFAULTS['port'], type=int)

    #parser.add_argument('-v', '--verbose', action='store_true')
    parser.add_argument('-q', '--quiet', action='store_true')

    parser.add_argument('-c', '--cli', action='store_true') # command line mode

    parser.add_argument('-np', '--no-player', action='store_true') # no player, just camera viewer

    parser.add_argument('-a', '--admin', action='store_true') # admin mode

    parser.add_argument('-t', '--tick', default=DEFAULTS['tick'])

    parser.add_argument('-n', '--name', default=DEFAULTS['name'])

    if cl_args is not None:
        args = parser.parse_args(cl_args)
    else:
        args = parser.parse_args()
    setattr(args, 'version', DC_VERSION)
    return args

def get_args():
    try:
        args = parse()
    except:             # this allows us to do: python gameloop.py 222.33.44.55  or 222.333.44.55:6666 (i.e. specifying only the ip address)
        from sys import argv

        server = argv[1]
        if ':' in server:
            server, port = server.split(':')
            cl_args = '--server %s --port %s' % (server, port,)
        else:
            cl_args = '--server %s' % (server,)
            
        args = parse(cl_args.split())
        
    return args
        

def main():
    import gameloop
    args = parse()
    gameloop.App(args).mainLoop()

if __name__ == '__main__':
    #main()
    args = parse()
    print dir(args)
