'''
Default settings for server

DO NOT MODIFY

Edit your settings in settings.py
'''

''' Server Settings '''
server_name = 'dc_mmo_server'

''' Network '''
#ip_address = '127.0.0.1'
ip_address = '0.0.0.0' # use internal default
port = 0    # use internal default

''' Game Settings '''
game_mode = 'ctf'
number_of_teams = 2
team_kills = False
victory_points = 3

''' Map '''
map = ''
seed = 8007513

try:
    from settings import *
except ImportError:
    print 'settings.py not found; using all defaults'
        
