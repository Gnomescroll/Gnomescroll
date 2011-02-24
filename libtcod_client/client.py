from start import *
from gui_elements import *
import libtcodpy as libtcod

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
redraw_map = True
redraw_side = True
show_fps = True
#Other random but important variables, mostly having to do with the gui
gui_state = "default"		#There is a game state handler for each multi-step process
gui_status = 0			#This describes how far through a gui_state we've gotten
current_menu = None		#This keeps track of whatever temporary menu is up at the moment.
main_menu = None		#This menu is always up, still to come
mouse_x	= None			#should be updated every tic
mouse_y = None

client = Client(0) 		#world _id = 0
client.setup()			#start server-client communications

def render_all():
	global redraw_messages, redraw_map, redraw_side, show_fps, viewer_start_x, viewer_start_y, test

	if redraw_map:
		libtcod.console_set_background_color(map_viewer, libtcod.red)
		libtcod.console_clear(map_viewer)
		libtcod.console_blit(map_viewer, 0, 0, MAP_VIEWER_WIDTH, MAP_VIEWER_HEIGHT, 0, 0, 0)
		redraw_map = False

	if message_log.redraw:
		message_con = message_log.draw()
		libtcod.console_blit(message_con, 0, 0, MESSAGE_LOG_WIDTH, MESSAGE_LOG_HEIGHT, 0, 0, MAP_VIEWER_HEIGHT)

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

def handle_keys():
	global current_menu, main_menu, game_state, message_log
	key = libtcod.console_check_for_keypress(libtcod.KEY_PRESSED) 
	key_char = chr(key.c)

	#Can also test: if key_char == 'c': ...

	if key.vk == libtcod.KEY_ESCAPE:
		return "exit"


def refresh_data():
	#get updated map, object, and agent data	
	info.get_map(0)
	info.get_agent_list()
	info.get_object_list()

###MAIN PROGRAM###
libtcod.console_init_root(SCREEN_WIDTH, SCREEN_HEIGHT, 'dc_mmo', False)
libtcod.sys_set_fps(LIMIT_FPS)
map_viewer = libtcod.console_new(MAP_VIEWER_WIDTH, MAP_VIEWER_HEIGHT)
message_log = Message_Log(MESSAGE_LOG_WIDTH, MESSAGE_LOG_HEIGHT)
side_panel = libtcod.console_new(SIDE_PANEL_WIDTH, SIDE_PANEL_HEIGHT)
fps_monitor = libtcod.console_new(FPS_MONITOR_WIDTH, FPS_MONITOR_HEIGHT)

#list of game messages
message_log.add("Welcome to dc_mmo")

#get updated map, object, and agent data
if client.server_listener.ready == 1:
	print "Redis Ready"
	refresh_data()
	time.sleep(1)
else:
	print "Redis Not Ready"

#test print the terrain at 1,1
print client.terrain_map.get(1, 1, 0)
admin.set_map(1, 1, 0, 5)
refresh_data()
print client.terrain_map.get(1, 1, 0)
print client.terrain_map.get_map_section(0, 0, 0, 10, 10) #Get a 10x10 section starting at 1,1

while not libtcod.console_is_window_closed():
	render_all()
	libtcod.console_flush()
	key_result = handle_keys()
	if key_result == "exit":
		break

