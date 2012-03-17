#!/usr/bin/python

'''
Usage:

To integrate in existing code:

from this_module import get_args
args = get_args()

'''

'''
Available options:

version         Prints server version to STDOUT
quiet           Suppress STDOUT (not implemented)

ip-address      Server IP Address
port        Server Port

game-mode       e.g. ctf, dm, tdm. ctf is the only valid option at the moment.
team-kills      Team killing allowed
victory-points  Number of points for a team/player victory
team-name-one   Name of first team
team-name-two   Name of second team

map             Name of map file in ./content/maps. If not provided, map will be generated
seed            RNG seed to use for map generator
save-map        Name of file to save map as after generating. Name is not required; will default to something unique

logger          Use file logging facility. Otherwise, prints to stdout/stderr

print-args      Print all settings to STDOUT
no-load         Don't start the server. Abort after argument processing.

'''

import sys
import argparse
import default_settings as settings

DC_SERVER_VERSION = '3.7'

# Note: boolean defaults are not loaded here
DEFAULTS = {
    'version'   :   '%%(prog)s %s' % (DC_SERVER_VERSION,),

    # Server Settings
    'name'      :   settings.server_name,

    # Network
    'ip_address': settings.ip_address,
    'port'      : settings.port,
    
    # Game Settings
    'game_mode' :   settings.game_mode,
    'victory_points':   settings.victory_points,
    'team_name_one':    settings.team_name_one,
    'team_name_two':    settings.team_name_two,
    
    # Map
    'map'       :   settings.map,
    'seed'      :   settings.seed,

    # Logger
    'logger'    :   settings.logger,
}

def parse(cl_args=None):
    parser = argparse.ArgumentParser(description="Server", prog="Gnomescroll")

    parser.add_argument('-V', '--version', action='version', version=DEFAULTS['version'])
    parser.add_argument('-q', '--quiet', action='store_true')   # not implemented!

    parser.add_argument('-n', '--name', default=DEFAULTS['name'], metavar='SERVER NAME', dest='server_name')

    ''' Network '''
    parser.add_argument('-ip', '--ip-address', default=DEFAULTS['ip_address'])
    parser.add_argument('-p', '--port', default=DEFAULTS['port'], type=int)

    ''' Game Settings '''
    parser.add_argument('-gm', '--game-mode', default=DEFAULTS['game_mode'], choices=['ctf', 'dm', 'tdm'])
    parser.add_argument('-tk', '--team-kills', action='store_true')
    parser.add_argument('-vp', '--victory-points', default=DEFAULTS['victory_points'], type=int)
    parser.add_argument('-tn1', '--team-name-one', default=DEFAULTS['team_name_one'])
    parser.add_argument('-tn2', '--team-name-two', default=DEFAULTS['team_name_two'])

    ''' Map '''
    parser.add_argument('-m', '--map', default=DEFAULTS['map'])
    parser.add_argument('--seed', default=DEFAULTS['seed'], type=int)
    parser.add_argument('--save-map', default='')

    ''' Logger '''
    parser.add_argument('-log', '--logger', action='store_true', default=DEFAULTS['logger'])

    ''' Debug '''
    parser.add_argument('-pa', '--print-args', action='store_true')
    parser.add_argument('-nl', '--no-load', action='store_true')

    ''' Production '''
    parser.add_argument('--production', action='store_true')

    if cl_args is not None:
        args = parser.parse_args(cl_args)
    else:
        args = parser.parse_args()

    setattr(args, 'version', DC_SERVER_VERSION)

    return args

def merge_with_settings(args):
    # collect settings keys (ignore builtins like __doc__)
    s_props = [p for p in dir(settings) if not (p.startswith('__') and p.endswith('__'))]

    # only copy values that are not defined in the args object
    # the args object already collects defaults from settings
    # in the process step, for shared arg names
    for p in s_props:
        if not hasattr(args, p):
            setattr(args, p, getattr(settings, p))

def postprocess_args(args):
    # Check booleans and override
    if not args.team_kills:
        args.team_kills = settings.team_kills

def get_args():
    try:
        args = parse()
    except argparse.ArgumentError:             # this allows us to do: python gameloop.py 222.33.44.55 (i.e. specifying only the ip address)
        server = sys.argv[1]
        try:
            if not len(server.split('.')) == 4:
                raise Exception
        except:
            sys.exit()
        cl_args = '--ip-address %s' % (server,)

        args = parse(cl_args.split())

    postprocess_args(args)
    merge_with_settings(args)

    if args.print_args:
        print_args(args)

    if args.no_load:
        sys.exit()
        
    return args

def print_args(args):
    keypairs = []
    print 'Options:'
    for key in dir(args):
        if key.startswith('__') and key.endswith('__'): continue # __special__
        val = getattr(args, key)
        if type(val).__name__ in ['instancemethod', 'module']:
            continue
        keypairs.append((key, getattr(args, key)))
    keypairs.sort()
    for keypair in keypairs:
        print '%s :: %s' % keypair

if __name__ == '__main__':
    args = get_args()
    print_args(args)
