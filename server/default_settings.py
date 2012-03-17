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
team_kills = False
victory_points = 3
team_name_one = "Green Team"
team_name_two = "Red Team"
#team_color_one = [200,0,200]
#team_color_two = [200,200,0]

''' Map '''
map = 'natural_terrain'
seed = 8007513

''' Logger '''
logger = False

try:
    from settings import *
except ImportError:
    print 'settings.py not found; using all defaults'
        
