#!/usr/bin/python

'''
Usage:

To integrate in existing code:

from this_module import get_args
args = get_args()

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

def parse():
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

    args = parser.parse_args()
    setattr(args, 'version', DC_VERSION)
    return args

def get_args():
    return parse()

def main():
    import gameloop
    args = parse()
    gameloop.App(args).mainLoop()

if __name__ == '__main__':
    #main()
    args = parse()
    print dir(args)
