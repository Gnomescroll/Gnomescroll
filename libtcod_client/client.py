from server_listener import *
from server_API import *
import libtcodpy as libtcod
import textwrap

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
#FPS Monitor width
FPS_MONITOR_WIDTH = 8
FPS_MONITOR_HEIGHT = 1
#Screen update frequency
LIMIT_FPS = 20
#Flags to optimize drawing
redraw_messages = True


def message(new_msg, color = libtcod.white):
	redraw_messages = True
	#split the message if necessary, among multiple lines
	new_msg_lines = textwrap.wrap(new_msg, MESSAGE_LOG_WIDTH)

	for line in new_msg_lines:
		#if the buffer is full, remove the first line to make room for the new one
		if len(game_msgs) == MESSAGE_LOG_HEIGHT:
		    del game_msgs[0]

		#add the new line as a tuple, with the text and the color
		game_msgs.append((line, color))

	

def render_all():
	global redraw_messages

	libtcod.console_set_background_color(map_viewer, libtcod.red)
	libtcod.console_clear(map_viewer)
	libtcod.console_blit(map_viewer, 0, 0, MAP_VIEWER_WIDTH, MAP_VIEWER_HEIGHT, 0, 0, 0)

	if redraw_messages:
		libtcod.console_set_background_color(message_log, libtcod.green)
		libtcod.console_clear(message_log)
		libtcod.console_blit(message_log, 0, 0, MESSAGE_LOG_WIDTH, MESSAGE_LOG_HEIGHT, 0, 0, MAP_VIEWER_HEIGHT)
		redraw_messages = False;

	libtcod.console_set_background_color(side_panel, libtcod.blue)
	libtcod.console_clear(side_panel)
	libtcod.console_blit(side_panel, 0, 0, SIDE_PANEL_WIDTH, SIDE_PANEL_HEIGHT, 0, MAP_VIEWER_WIDTH, 0)

	libtcod.console_set_background_color(fps_monitor, libtcod.blue)
	libtcod.console_clear(fps_monitor)
	libtcod.console_blit(fps_monitor, 0, 0, FPS_MONITOR_WIDTH, FPS_MONITOR_HEIGHT, 0, 0, 0)


###MAIN PROGRAM###

libtcod.console_init_root(SCREEN_WIDTH, SCREEN_HEIGHT, 'dc_mmo', False)
libtcod.sys_set_fps(LIMIT_FPS)
map_viewer = libtcod.console_new(MAP_VIEWER_WIDTH, MAP_VIEWER_HEIGHT)
message_log = libtcod.console_new(MESSAGE_LOG_WIDTH, MESSAGE_LOG_HEIGHT)
side_panel = libtcod.console_new(SIDE_PANEL_WIDTH, SIDE_PANEL_HEIGHT)
fps_monitor = libtcod.console_new(FPS_MONITOR_WIDTH, FPS_MONITOR_HEIGHT)

#declare the server listener
sl = Server_listener()

#list of game messages
game_msgs = []

message("Welcome to dc_mmo")

while not libtcod.console_is_window_closed():
	render_all()
	libtcod.console_flush()


