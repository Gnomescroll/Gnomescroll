import time
from client import Client
from gui_elements import Button, Menu, Message_Log

import libtcodpy as libtcod

class Display:
	"""handles rendering"""
	def __init__(self, redraw_side=True, show_fps=True, gui_redraw_map=False, offset_x=0, offset_y=0):
		self.redraw_side = redraw_side
		self.show_fps = show_fps
		self.offset_x = offset_x
		self.offset_y = offset_y
		self.gui_redraw_map = gui_redraw_map
		self.viewer_bot_x = MAP_VIEWER_WIDTH  + self.offset_x
		self.viewer_bot_y = MAP_VIEWER_HEIGHT + self.offset_y
		self.cursor_pos = (self.viewer_bot_x/2, self.viewer_bot_y/2)
		self.display_cursor = True
		self.cursor_char = 'X'
		self.cursor_fore_color = libtcod.black
		self.cursor_back_color = libtcod.red
		
	def move_screen(self, dx, dy):
		#moves the screen if that wouldn't cause the edge of the map to be exceeded.
		#If it would, it moves as much as it can without passing the edge of the map.
		self.viewer_bot_x = self.offset_x + MAP_VIEWER_WIDTH
		self.viewer_bot_y = self.offset_y + MAP_VIEWER_HEIGHT

		if self.offset_x + dx < 0: 
		        dx = self.offset_x * -1
	    
		elif self.viewer_bot_x + dx >= MAP_WIDTH:
		        dx = MAP_WIDTH - self.viewer_bot_x

		if  self.offset_y + dy < 0:
		        dy = self.offset_y * -1

		elif self.viewer_bot_y + dy >= MAP_HEIGHT:
		        dy = MAP_HEIGHT - self.viewer_bot_y

		
		self.offset_x = self.offset_x + dx
		self.offset_y = self.offset_y + dy
		self.gui_redraw_map = True
		
	def set_cursor_pos(self, x, y):
		self.cursor_pos = (x, y)
		if self.display_cursor:
			self.gui_redraw_map = True

	def set_cursor_char(self, char):
		self.cursor_char = char
		if self.display_cursor:
			self.gui_redraw_map = True
	
	def set_cursor_colors(self, back, fore):
		self.cursor_fore_color = fore
		self.cursor_back_color = back
		if self.display_cursor:
			self.gui_redraw_map = True	

	def render_all(self):
		if client.terrain_map.redraw or self.gui_redraw_map or client.agent_handler.agents_changed:
			tmap = client.terrain_map.get_map_section(self.offset_x, self.offset_y, current_z, self.viewer_bot_x, self.viewer_bot_y)
			#print tmap
			x = 0
			y = 0
			char = ' '
			color = libtcod.darker_green
			#this section is messy. I'm not actually sure why it works. 
			#TODO clean this up
			for row in tmap:
				for element in row:
					if element == -1:
						char  = ' '
						color = libtcod.darker_green
					else: #there is a whole of code that will go here.
						char = unichr(87)
					libtcod.console_set_char(map_viewer,  x, y, chr(ord(char))) #BRILLIANT!
					libtcod.console_set_char_background(map_viewer, x, y, color, libtcod.BKGND_SET)
					y += 1
				y = 0
				x += 1

			#draw the characters
			for agent in client.agent_handler.agents:
				position = agent['position']
				if position[1] >= self.offset_x and position[1] <= self.viewer_bot_x and position[2] >= self.offset_y and position[2] <= self.viewer_bot_x:
					libtcod.console_set_char(map_viewer, position[1] - self.offset_x, position[2] - self.offset_y, '@')

			#draw cursor
			if self.display_cursor:
				libtcod.console_set_default_background(map_viewer, self.cursor_back_color)
				libtcod.console_set_default_foreground(map_viewer, self.cursor_fore_color)
				libtcod.console_set_char(map_viewer, self.cursor_pos[0], self.cursor_pos[1], self.cursor_char)
				libtcod.console_set_char_background(map_viewer, self.cursor_pos[0], self.cursor_pos[1], libtcod.red, libtcod.BKGND_SET)			

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

		if current_menu.redraw:
			menu_con = current_menu.draw()
			libtcod.console_blit(menu_con, 0, 0, current_menu.width, current_menu.height, 0, current_menu.x, current_menu.y)

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
			
		elif key_char == 'c':
			client.admin.create_agent(self.drawing_demo+5, self.drawing_demo, 0)
			self.drawing_demo += 1
			
		elif key.vk == libtcod.KEY_UP and key.shift:
			self.display.move_screen(0, -10)
			
		elif key.vk == libtcod.KEY_DOWN and key.shift:
			self.display.move_screen(0, 10)
			
		elif key.vk == libtcod.KEY_LEFT and key.shift:
			self.display.move_screen(-10, 0)
			
		elif key.vk == libtcod.KEY_RIGHT and key.shift:
			self.display.move_screen(10, 0)
			
		elif key.vk == libtcod.KEY_UP:
			self.display.move_screen(0, -1)
			
		elif key.vk == libtcod.KEY_DOWN:
			self.display.move_screen(0, 1)
			
		elif key.vk == libtcod.KEY_LEFT:
			self.display.move_screen(-1, 0)
			
		elif key.vk == libtcod.KEY_RIGHT:
			self.display.move_screen(1, 0)

		else:
			current_menu.handle_key(key_char)
	
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

		self.display.set_cursor_pos(current_mouse.cx, current_mouse.cy)
		current_menu.update(current_mouse)

		#TODO- deal with user clicking an agent (would need to display info, changing current menu to an info menu or something.)
		

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

##build the main menu##
main_menu = Menu("Main Menu", libtcod.darker_red, libtcod.black, False)
main_menu.add_button(Button(13, 1, "Mine", 'm', "Mine some stone", libtcod.darker_red, libtcod.black))
main_menu.add_button(Button(13, 1, "Build", 'b', "Construct something", libtcod.darker_red, libtcod.black))
main_menu.add_button(Button(13, 1, "Plant", 'p', "Plant crops", libtcod.darker_red, libtcod.black))
main_menu.add_button(Button(13, 1, "Designate", 'd', "Designate an area", libtcod.darker_red, libtcod.black))
main_menu.add_button(Button(13, 1, "Info", 'i', "Get information", libtcod.darker_red, libtcod.black))
main_menu.initialize()
main_menu.set_position("center", 0, MAP_VIEWER_WIDTH, 0, SIDE_PANEL_WIDTH, SIDE_PANEL_HEIGHT)

#the active menu should always be set to be the current_menu
current_menu = main_menu

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

print current_menu.x, current_menu.y

while not libtcod.console_is_window_closed():
	Input.handle_mouse(libtcod.mouse_get_status())
	key_result = Input.handle_keys()
	if key_result == "exit":
		break
	libtcod.console_flush()
	Display.render_all()
