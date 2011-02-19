from server_listener import *
from server_API import *
import libtcodpy as libtcod

##Constants##
SCREEN_WIDTH = 140
SCREEN_HEIGHT = 80
LIMIT_FPS = 20

libtcod.console_init_root(SCREEN_WIDTH, SCREEN_HEIGHT, 'dc_mmo', False)
libtcod.sys_set_fps(LIMIT_FPS)

#declare the server listener
sl = Server_listener()

#list of game messages
game_msgs = []

while not libtcod.console_is_window_closed():
	libtcod.console_set_foreground_color(0, libtcod.white)
	libtcod.console_print_left(0, 1, 1, libtcod.BKGND_NONE, 'Welcome to dc_mmo!')
	libtcod.console_flush()
