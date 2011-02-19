from server_listener import *
from server_API import *
import libtcodpy as libtcod
import textwrap

def message(new_msg, color = libtcod.white):
    #split the message if necessary, among multiple lines
    new_msg_lines = textwrap.wrap(new_msg, MESSAGE_LOG_WIDTH)
 
    for line in new_msg_lines:
        #if the buffer is full, remove the first line to make room for the new one
        if len(game_msgs) == MESSAGE_LOG_HEIGHT:
            del game_msgs[0]
 
        #add the new line as a tuple, with the text and the color
        game_msgs.append((line, color))

##Constants##
SCREEN_WIDTH = 140
SCREEN_HEIGHT = 80
#Size of the map itself
MAP_WIDTH = 400
MAP_HEIGHT = 400
#How much of the map is displayed on the screen at one time
MAP_VIEWER_WIDTH = 100
MAP_VIEWER_HEIGHT = 70
#Message log, at the bottom
MESSAGE_LOG_WIDTH = 100
MESSAGE_LOG_HEIGHT = 10
#Side panel, for information that will be added such as health, character overview, jobs, inventory, etc
SIDE_PANEL_WIDTH = 40
SIDE_PANEL_HEIGHT = 80
#Screen update frequency
LIMIT_FPS = 20

libtcod.console_init_root(SCREEN_WIDTH, SCREEN_HEIGHT, 'dc_mmo', False)
libtcod.sys_set_fps(LIMIT_FPS)
map_viewer = libtcod.console_new(MAP_VIEWER_WIDTH, MAP_VIEWER_HEIGHT)
message_log = libtcod.console_new(MESSAGE_LOG_WIDTH, MESSAGE_LOG_HEIGHT)

#declare the server listener
sl = Server_listener()

#list of game messages
game_msgs = []

message("Welcome to dc_mmo")

while not libtcod.console_is_window_closed():
	libtcod.console_set_foreground_color(0, libtcod.white)
	libtcod.console_print_left(0, 1, 1, libtcod.BKGND_NONE, 'Welcome to dc_mmo!')
	libtcod.console_flush()


