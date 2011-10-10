'''
Default settings for server

DO NOT MODIFY

Edit your settings in settings.py
'''

server_name = 'dc_mmo_server'

tcp_port = 5055
udp_port = 5060
ip_address = '127.0.0.1'


''' Game '''
game_mode = 'ctf'
n_teams = 2
team_kills = False
victory_points = 3

map = ''
seed = 8007513

try:
    from settings import *
except ImportError:
    print 'settings.py not found; using all defaults'
        
