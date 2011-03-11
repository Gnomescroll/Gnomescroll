import time
from client import Client
from gui_elements import Button, Menu, Message_Log

import libtcodpy as libtcod

##Constants##
SCREEN_WIDTH = 140
SCREEN_HEIGHT = 80
#Map size
MAP_WIDTH = 200
MAP_HEIGHT = 200
#How much of the map is displayed on the screen at one time
MAP_VIEWER_WIDTH = SCREEN_WIDTH-40
MAP_VIEWER_HEIGHT = SCREEN_HEIGHT-10
#Message log, at the bottom
MESSAGE_LOG_WIDTH = MAP_VIEWER_WIDTH
MESSAGE_LOG_HEIGHT = SCREEN_HEIGHT - MAP_VIEWER_HEIGHT
#Side panel, for information that will be added such as health, character overview, jobs, inventory, etc
SIDE_PANEL_WIDTH = SCREEN_WIDTH - MAP_VIEWER_WIDTH
SIDE_PANEL_HEIGHT = SCREEN_HEIGHT
#FPS Monitor width
FPS_MONITOR_WIDTH = 8
FPS_MONITOR_HEIGHT = 1
#Screen update frequency
LIMIT_FPS = 20
#Describes what part of the map is currently in view
viewer_start_x = 0
viewer_start_y = 0
current_z = 0
#Flags to optimize drawing
redraw_map = True
redraw_side = True
show_fps = True
#Other random but important variables, mostly having to do with the gui
gui_state = "default"        #There is a game state handler for each multi-step process
gui_status = 0            #This describes how far through a gui_state we've gotten
current_menu = None        #This keeps track of whatever temporary menu is up at the moment.
main_menu = None        #This menu is always up, still to come
mouse_x    = None            #should be updated every tic
mouse_y = None
drawing_demo = 0
gui_redraw_map = False
check_mouse_drag = True
mouse_on_drag_start = None
current_mouse = None
client = Client(world_id=0)
client.setup()			#start server-client communications

def render_all():
	global redraw_messages, redraw_map, redraw_side, show_fps, viewer_start_x, viewer_start_y, test, gui_redraw_map

	if client.terrain_map.redraw or gui_redraw_map:
		viewer_bot_x = MAP_VIEWER_WIDTH+viewer_start_x
		viewer_bot_y = MAP_VIEWER_HEIGHT+viewer_start_y
		tmap = client.terrain_map.get_map_section(viewer_start_x, viewer_start_y, current_z, viewer_bot_x, viewer_bot_y)
		x = 0
		y = 0
		#this section is messy. I'm not actually sure why it works. 
		#TODO clean this up
		for row in tmap:
			for element in row:
				if element == -1:
					color = libtcod.darker_green
				else:
					color = libtcod.black
				libtcod.console_set_char_background(map_viewer, x, y, color, libtcod.BKGND_SET)
				y += 1
			y = 0
			x += 1
		client.terrain_map.redraw = False
		gui_redraw_map = False	

	#re-blit the map if the map in case an agent position has changed
	libtcod.console_blit(map_viewer, 0, 0, MAP_VIEWER_WIDTH, MAP_VIEWER_HEIGHT, 0, 0, 0)
		

	if message_log.redraw:
		message_con = message_log.draw()
		libtcod.console_blit(message_con, 0, 0, MESSAGE_LOG_WIDTH, MESSAGE_LOG_HEIGHT, 0, 0, MAP_VIEWER_HEIGHT)

	if redraw_side:
		libtcod.console_set_default_background(side_panel, libtcod.dark_blue)
		libtcod.console_clear(side_panel)
		libtcod.console_blit(side_panel, 0, 0, SIDE_PANEL_WIDTH, SIDE_PANEL_HEIGHT, 0, MAP_VIEWER_WIDTH, 0)
		redraw_side = False
	
	if show_fps:
		libtcod.console_set_default_background(fps_monitor, libtcod.black)
		libtcod.console_set_default_foreground(fps_monitor, libtcod.white)
		libtcod.console_clear(fps_monitor)
		fps = "FPS: " +str(libtcod.sys_get_fps())
		libtcod.console_print(fps_monitor, 0, 0, fps)	
		libtcod.console_blit(fps_monitor, 0, 0, FPS_MONITOR_WIDTH, FPS_MONITOR_HEIGHT, 0, 0, 0)

def handle_keys():
	global current_menu, main_menu, game_state, message_log, drawing_demo
	key = libtcod.console_check_for_keypress(libtcod.KEY_PRESSED) 
	key_char = chr(key.c)

	#Can also test: if key_char == 'c': ...

	if key.vk == libtcod.KEY_ESCAPE:
		return "exit"

	if key_char == 'a':
		client.admin.set_map(drawing_demo, drawing_demo, 0, 5)
		drawing_demo += 1

def handle_mouse():
	#For now, all this function does is see if the mouse is dragging.
	#Eventually it will handle clicks as well
	global check_mouse_drag, mouse_x, mouse_y, mouse_on_drag_start, current_mouse

        if check_mouse_drag:
		if current_mouse.lbutton:
		        dx = mouse_on_drag_start.cx - current_mouse.cx;
		        dy = mouse_on_drag_start.cy - current_mouse.cy;
		        move_screen(dx, dy);
		        mouse_on_drag_start = current_mouse;
		else:
		    mouse_on_drag_start = current_mouse;

def move_screen(dx, dy):
	#moves the screen if that wouldn't cause the edge of the map to be exceeded.
	#If it would, it moves as much as it can without passing the edge of the map.
	global viewer_start_x, viewer_start_y, gui_redraw_map
	viewer_bot_x = viewer_start_x + MAP_VIEWER_WIDTH
	viewer_bot_y = viewer_start_y + MAP_VIEWER_HEIGHT

	if viewer_start_x + dx < 0: 
		dx = viewer_start_x * -1
    
	elif viewer_bot_x + dx >= MAP_WIDTH:
		dx = MAP_WIDTH - viewer_bot_x

	if  viewer_start_y + dy < 0:
		dy = viewer_start_y * -1

	elif viewer_bot_y + dy >= MAP_HEIGHT:
		dy = MAP_HEIGHT - viewer_bot_y

	
	viewer_start_x = viewer_start_x + dx
	viewer_start_y = viewer_start_y + dy
	gui_redraw_map = True

###MAIN PROGRAM###
libtcod.console_init_root(SCREEN_WIDTH, SCREEN_HEIGHT, 'dc_mmo', False, libtcod.RENDERER_OPENGL)
libtcod.sys_set_fps(LIMIT_FPS)
map_viewer = libtcod.console_new(MAP_VIEWER_WIDTH, MAP_VIEWER_HEIGHT)
message_log = Message_Log(MESSAGE_LOG_WIDTH, MESSAGE_LOG_HEIGHT)
side_panel = libtcod.console_new(SIDE_PANEL_WIDTH, SIDE_PANEL_HEIGHT)
fps_monitor = libtcod.console_new(FPS_MONITOR_WIDTH, FPS_MONITOR_HEIGHT)

#list of game messages
message_log.add("Welcome to dc_mmo")

#get updated map, object, and agent data
if client.listener.ready == 1:
	print '***'
	print "Redis Ready"
	print '***'
	#get updated map, object, and agent data	
	client.info.get_map(0)
	client.info.get_agent_list()
	client.info.get_object_list()
	client.update()
	time.sleep(1)
else:
	print "Redis Not Ready"

client.admin.create_agent(2,1,0)
client.info.get_agent(0)

while not libtcod.console_is_window_closed():
	#handle mouse
	current_mouse = libtcod.mouse_get_status();
	handle_mouse()
	#handle keyboard
	key_result = handle_keys()
	if key_result == "exit":
		break
	#redraw whatever is needed
	libtcod.console_flush()
	render_all()






