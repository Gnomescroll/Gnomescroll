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

import sys
import argparse
import settings

DC_VERSION = '0.2'

DEFAULTS = {
    'ip_address'    :   '127.0.0.1',
    'tcp_port'  :   5055,
    'udp_port'  :   5060,
    'tick'      :   0.01,
    'name'      :   settings.server_name,
    'game_mode' :   settings.game_mode,
    'n_teams'   :   settings.n_teams,
}

def parse(cl_args=None):
    parser = argparse.ArgumentParser(description="DC_MMO Netclient", prog="Dwarf Control (in Space)")

    vs = '%(prog)s ' + DC_VERSION
    parser.add_argument('-V', '--version', action='version', version=vs)

    parser.add_argument('-ip', '--ip-address', default=DEFAULTS['ip_address'])

    parser.add_argument('-tp', '--tcp-port', default=DEFAULTS['tcp_port'], type=int)
    parser.add_argument('-up', '--udp-port', default=DEFAULTS['udp_port'], type=int)

    #parser.add_argument('-v', '--verbose', action='store_true')
    parser.add_argument('-q', '--quiet', action='store_true')

    parser.add_argument('-c', '--cli', action='store_true') # command line mode

    parser.add_argument('-a', '--admin', action='store_true') # admin mode

    parser.add_argument('-t', '--tick', default=DEFAULTS['tick'])

    parser.add_argument('-n', '--name', default=DEFAULTS['name'], metavar='SERVER NAME', dest='server_name')

    parser.add_argument('-g', '--game-mode', default=DEFAULTS['game_mode'])

    parser.add_argument('-nt', '-n-teams', default=DEFAULTS['n_teams'], dest='n_teams')

    parser.add_argument('--print-args', action='store_true')

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
        server = sys.argv[1]
        cl_args = '--ip-address %s' % (server,)

        args = parse(cl_args.split())

    if args.print_args:
        print_args(args)

    return args

def print_args(args):
    keys = [
        'version',
        'ip_address',
        'tcp_port',
        'udp_port',
        'quiet',
        'cli',
        'admin',
        'tick',
        'server_name',
        'game_mode',
        'n_teams',
    ]
    print 'Options:'
    for key in keys:
        print getattr(args, key)

def main():
    import gameloop
    args = parse()
    gameloop.App(args).mainLoop()

if __name__ == '__main__':
    #main()
    args = parse()
    print dir(args)
