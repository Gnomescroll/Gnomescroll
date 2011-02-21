from start import *
from gui_elements import *
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
#Describes what part of the map is currently in view
viewer_start_x = 0
viewer_start_y = 0
#Flags to optimize drawing
redraw_messages = True
redraw_map = True
redraw_side = True
show_fps = True
#Other random but important variables, mostly having to do with the gui
game_state = "playing" 		#Could also be "paused"
gui_state = "default"		#There is a game state handler for each multi-step process
gui_status = 0			#This describes how far through a gui_state we've gotten
current_menu = None
mouse_x				#should be updated every tic
mouse_y

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
	global redraw_messages, redraw_map, redraw_side, show_fps, viewer_start_x, viewer_start_y, test

	if redraw_map:
		libtcod.console_set_background_color(map_viewer, libtcod.red)
		libtcod.console_clear(map_viewer)
		libtcod.console_blit(map_viewer, 0, 0, MAP_VIEWER_WIDTH, MAP_VIEWER_HEIGHT, 0, 0, 0)
		redraw_map = False

	if redraw_messages:
		libtcod.console_set_background_color(message_log, libtcod.darker_blue)
		libtcod.console_clear(message_log)
		y = 1
		for (line, color) in game_msgs:
			libtcod.console_set_foreground_color(message_log, color)
			libtcod.console_print_left(message_log, 1, y, libtcod.BKGND_NONE, line)
			y += 1
		libtcod.console_blit(message_log, 0, 0, MESSAGE_LOG_WIDTH, MESSAGE_LOG_HEIGHT, 0, 0, MAP_VIEWER_HEIGHT)
		redraw_messages = False;

	if redraw_side:
		libtcod.console_set_background_color(side_panel, libtcod.dark_blue)
		libtcod.console_clear(side_panel)
		libtcod.console_blit(side_panel, 0, 0, SIDE_PANEL_WIDTH, SIDE_PANEL_HEIGHT, 0, MAP_VIEWER_WIDTH, 0)
		redraw_side = False
	
	if show_fps:
		libtcod.console_set_background_color(fps_monitor, libtcod.black)
		libtcod.console_set_foreground_color(fps_monitor, libtcod.white)
		libtcod.console_clear(fps_monitor)
		fps = "FPS: " +str(libtcod.sys_get_fps())
		libtcod.console_print_left(fps_monitor, 0, 0, libtcod.BKGND_NONE, fps)	
		libtcod.console_blit(fps_monitor, 0, 0, FPS_MONITOR_WIDTH, FPS_MONITOR_HEIGHT, 0, 0, 0, 1, 0.6)



###MAIN PROGRAM###
libtcod.console_init_root(SCREEN_WIDTH, SCREEN_HEIGHT, 'dc_mmo', False)
libtcod.sys_set_fps(LIMIT_FPS)
map_viewer = libtcod.console_new(MAP_VIEWER_WIDTH, MAP_VIEWER_HEIGHT)
message_log = libtcod.console_new(MESSAGE_LOG_WIDTH, MESSAGE_LOG_HEIGHT)
side_panel = libtcod.console_new(SIDE_PANEL_WIDTH, SIDE_PANEL_HEIGHT)
fps_monitor = libtcod.console_new(FPS_MONITOR_WIDTH, FPS_MONITOR_HEIGHT)

#list of game messages
game_msgs = []

message("Welcome to dc_mmo")

while not libtcod.console_is_window_closed():
	render_all()
	libtcod.console_flush()

