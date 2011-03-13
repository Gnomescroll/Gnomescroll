import time
from client import Client
from gui_elements import Button, Menu, Message_Log

import libtcodpy as libtcod

class Display:
	"""handles rendering"""
	def __init__(self, redraw_side=True, show_fps=True, viewer_start_x=0, viewer_start_y=0, gui_redraw_map=False, offset_x=0, offset_y=0):
		self.redraw_side = redraw_side
		self.show_fps = show_fps
		self.viewer_start_x = viewer_start_x
		self.viewer_start_y = viewer_start_y
		self.gui_redraw_map = gui_redraw_map
		self.offset_x = offset_x
		self.offset_y = offset_y
		self.viewer_bot_x = MAP_VIEWER_WIDTH  + self.viewer_start_x
		self.viewer_bot_y = MAP_VIEWER_HEIGHT + self.viewer_start_y
		
	def move_screen(self, dx, dy):
		#moves the screen if that wouldn't cause the edge of the map to be exceeded.
		#If it would, it moves as much as it can without passing the edge of the map.
		self.offset_x = self.viewer_start_x + dx
		self.offset_y = self.viewer_start_y + dy
		if self.offset_x < 0: 
			dx = self.viewer_start_x * -1

		elif self.viewer_bot_x + dx >= MAP_WIDTH:
			dx = MAP_WIDTH - self.viewer_bot_x

		if  self.offset_y < 0:
			dy = self.viewer_start_y * -1

		elif self.viewer_bot_y + dy >= MAP_HEIGHT:
			dy = MAP_HEIGHT - self.viewer_bot_y


		self.viewer_start_x = self.offset_x
		self.viewer_start_y = self.offset_y
		self.viewer_bot_x = self.viewer_start_x + MAP_VIEWER_WIDTH
		self.viewer_bot_y = self.viewer_start_y + MAP_VIEWER_HEIGHT
		self.gui_redraw_map = True

	def render_all(self):
		if client.terrain_map.redraw or self.gui_redraw_map or client.agent_handler.agents_changed:
			tmap = client.terrain_map.get_map_section(self.viewer_start_x, self.viewer_start_y, current_z, self.viewer_bot_x, self.viewer_bot_y)
			#print tmap
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
					libtcod.console_set_char(map_viewer,  x, y, ' ')	
					libtcod.console_set_char_background(map_viewer, x, y, color, libtcod.BKGND_SET)
					y += 1
				y = 0
				x += 1

			#draw the characters
			for agent in client.agent_handler.agents:
				position = agent['position']
				libtcod.console_set_char(map_viewer, position[1] - self.offset_x, position[2] - self.offset_y, '@')

			#clear flags
			client.terrain_map.redraw = False
			self.gui_redraw_map = False
			client.agent_handler.agents_changed = False

			libtcod.console_blit(map_viewer, 0, 0, MAP_VIEWER_WIDTH, MAP_VIEWER_HEIGHT, 0, 0, 0)

		if message_log.redraw:
			message_con = message_log.draw()
			libtcod.console_blit(message_con, 0, 0, MESSAGE_LOG_WIDTH, MESSAGE_LOG_HEIGHT, 0, 0, MAP_VIEWER_HEIGHT)

		if self.redraw_side:
			libtcod.console_set_default_background(side_panel, libtcod.dark_blue)
			libtcod.console_clear(side_panel)
			libtcod.console_blit(side_panel, 0, 0, SIDE_PANEL_WIDTH, SIDE_PANEL_HEIGHT, 0, MAP_VIEWER_WIDTH, 0)
			self.redraw_side = False

		if self.show_fps:
			libtcod.console_set_default_background(fps_monitor, libtcod.black)
			libtcod.console_set_default_foreground(fps_monitor, libtcod.white)
			libtcod.console_clear(fps_monitor)
			fps = "FPS: " +str(libtcod.sys_get_fps())
			libtcod.console_print(fps_monitor, 0, 0, fps)	
			libtcod.console_blit(fps_monitor, 0, 0, FPS_MONITOR_WIDTH, FPS_MONITOR_HEIGHT, 0, 0, 0)

class Input:
	"""handles keyboard and mouse"""
	def __init__(self, display):
		self.drawing_demo = 0 #variable used for testing.
		self.check_mouse_drag = True
		self.mouse_x = None            #should be updated every tic
		self.mouse_y = None
		self.check_mouse_drag = True
		self.mouse_on_drag_start = None
		self.current_mouse = None
		self.display = display
	
	def handle_keys(self):
		key = libtcod.console_check_for_keypress(libtcod.KEY_PRESSED) 
		key_char = chr(key.c)
		
		if key.vk == libtcod.KEY_ESCAPE:
			return "exit"
			
		if key_char == 'a':
			client.admin.set_map(self.drawing_demo, self.drawing_demo, 0, 5)
			self.drawing_demo += 1
			
		if key_char == 'c':
			client.admin.create_agent(self.drawing_demo+5, self.drawing_demo, 0)
			self.drawing_demo += 1
			
		if key.vk == libtcod.KEY_UP and key.shift:
			move_screen(0, -10)
			
		elif key.vk == libtcod.KEY_DOWN and key.shift:
			move_screen(0, 10)
			
		elif key.vk == libtcod.KEY_LEFT and key.shift:
			move_screen(-10, 0)
			
		elif key.vk == libtcod.KEY_RIGHT and key.shift:
			move_screen(10, 0)
			
		elif key.vk == libtcod.KEY_UP:
			move_screen(0, -1)
			
		elif key.vk == libtcod.KEY_DOWN:
			move_screen(0, 1)
			
		elif key.vk == libtcod.KEY_LEFT:
			move_screen(-1, 0)
			
		elif key.vk == libtcod.KEY_RIGHT:
			move_screen(1, 0)
	
	def handle_mouse(self, current_mouse):
		self.current_mouse = current_mouse #!!!
		#TODO handle mouse clicks
		if self.current_mouse.lbutton:
			dx = self.mouse_on_drag_start.cx - self.current_mouse.cx;
			dy = self.mouse_on_drag_start.cy - self.current_mouse.cy;
			self.display.move_screen(dx, dy);
			self.mouse_on_drag_start = self.current_mouse;
		else:
		    self.mouse_on_drag_start = self.current_mouse;


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

#Describes what part of the map is currently in view
current_z = 0

###MAIN PROGRAM###
client = Client(world_id=0)
client.setup() #start server-client communications
Display = Display()
Input   = Input(Display)
libtcod.console_init_root(SCREEN_WIDTH, SCREEN_HEIGHT, 'dc_mmo', False, libtcod.RENDERER_OPENGL)
libtcod.sys_set_fps(20)
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

while not libtcod.console_is_window_closed():
	Input.handle_mouse(libtcod.mouse_get_status())
	key_result = Input.handle_keys()
	if key_result == "exit":
		break
	libtcod.console_flush()
	Display.render_all()
