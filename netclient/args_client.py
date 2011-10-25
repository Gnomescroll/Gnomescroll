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

#from sys import argv
import sys

import argparse
import default_settings as settings

DC_VERSION = '0.2'

DEFAULTS = {
    'server'    :   '127.0.0.1',
    'port'      :   5055,
    'tick'      :   0.01,
    'name'      :   settings.name,
    'alt_name'  :   settings.alt_name,
    'fullscreen':   settings.fullscreen,
    'width'     :   settings.width,
    'height'    :   settings.height,
    'sensitivity':  90,
    'mouse_sensitivity':    None,
    'camera_sensitivity':   None,
    'sfx'       :   settings.sfx,
    'music'     :   settings.music,
}

def load_defaults():
    global DEFAULTS

    if hasattr(settings, 'sensitivity'):
        DEFAULTS['sensitivity'] = settings.sensitivity

    for prop in ['mouse_sensitivity', 'camera_sensitivity']:
        if hasattr(settings, prop):
            DEFAULTS[prop] = getattr(settings, prop)
        else:
            DEFAULTS[prop] = DEFAULTS['sensitivity']
        
load_defaults()

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

    parser.add_argument('-t', '--tick', default=DEFAULTS['tick'], type=int)

    parser.add_argument('-n', '--name', default=DEFAULTS['name'])
    parser.add_argument('-an', '--alt-name', default=DEFAULTS['alt_name'], dest='alt_name')

    parser.add_argument('-aa', '--auto-assign', action='store_true', dest='auto_assign_team')

    parser.add_argument('-pa', '--print-args', action='store_true')

    parser.add_argument('-fs', '--fullscreen', action='store_true')

    parser.add_argument('-x', '--width', default=DEFAULTS['width'], type=int)

    parser.add_argument('-y', '--height', default=DEFAULTS['height'], type=int)

    parser.add_argument('-sen', '--sensitivity', default=argparse.SUPPRESS, type=int)

    parser.add_argument('-csen', '--camera-sensitivity', default=argparse.SUPPRESS, type=int)

    parser.add_argument('-msen', '--mouse-sensitivity', default=argparse.SUPPRESS, type=int)

    parser.add_argument('-nl', '--no-load', action='store_true')

    parser.add_argument('-fps', '--fps', action='store_true')

    parser.add_argument('--ping', action='store_true')

    parser.add_argument('-nh', '--no-hud', action='store_true')

    parser.add_argument('--no-audio', action='store_false', dest='audio')
    parser.add_argument('--sfx', default=DEFAULTS['sfx'])
    parser.add_argument('--music', default=DEFAULTS['music'])

    parser.add_argument('-dad', '--disable-agents-draw', action='store_true')

    if cl_args is not None:
        args = parser.parse_args(cl_args)
    else:
        args = parser.parse_args()
        
    setattr(args, 'version', DC_VERSION)
    
    return args

def get_args():
    try:
        args = parse()
    except Exception, e:             # this allows us to do: python gameloop.py 222.33.44.55  or 222.333.44.55:6666 (i.e. specifying only the ip address)
        print 'args exception', e
        server = sys.argv[1]
        if ':' in server:
            server, port = server.split(':')
            cl_args = '--server %s --port %s' % (server, port,)
        else:
            cl_args = '--server %s' % (server,)

        args = parse(cl_args.split())

    args.fullscreen = int(bool(args.fullscreen))

    # if -sen --sensitivity is provided on the command line, override all sensitivity,
    # except other cli sensitivities
    # e.g. ./run -sen 500           --- mouse and camera sen 500
    # e.g. ./run -sen 500 -csen 200 --- mouse 500, camera 200
    # e.g. ./run -msen 200          --- mouse 200, camera is either settings.camera_sensitivity, settings.sensitivity, or DEFAULTS['sensitivity'] (in decreasing priority order)
    sen_cli_defined = hasattr(args, 'sensitivity')
    for prop in ['mouse_sensitivity', 'camera_sensitivity']:
        if not hasattr(args, prop):
            if sen_cli_defined:
                sen_attr = args.sensitivity
            else:
                sen_attr = DEFAULTS[prop]
            setattr(args, prop, sen_attr)
    if not sen_cli_defined:
        args.sensitivity = DEFAULTS['sensitivity']

    if not args.fps and settings.fps:
        args.fps = settings.fps

    if not args.ping and settings.ping:
        args.ping = settings.ping

    if not args.auto_assign_team:
        args.auto_assign_team = settings.auto_assign_team

    if not args.disable_agents_draw:
        args.disable_agents_draw = not settings.draw_agents
    args.draw_agents = not args.disable_agents_draw

    #sound
    if args.audio:
        args.audio = settings.audio
    args.sfx = max(min(args.sfx, 100), 0)
    args.sfx /= 100.
    args.music = max(min(args.music, 100), 0)
    args.music /= 100.
    
    if args.print_args:
        print_args(args)

    if args.no_load:
        import sys
        sys.exit()
    
    return args

def print_args(args):
    keys = [
        'version',
        'server',
        'port',
        'quiet',
        'cli',
        'no_player',
        'admin',
        'tick',
        'name',
        'fullscreen',
        'width',
        'height',
        'sensitivity',
        'mouse_sensitivity',
        'camera_sensitivity',
        'auto_assign_team',
        'fps',
        'ping',
        'no_hud',
        'audio',
        'sfx',
        'music',
        'disable_agents_draw',
    ]
    print 'Options:'
    for key in keys:
        print '%s :: %s' % (key, getattr(args, key),)

#def main():
    #import gameloop
    #args = parse()
    #gameloop.App(args).mainLoop()

if __name__ == '__main__':
    #main()
    args = get_args()
    print dir(args)
