import libtcodpy as libtcod
 
#actual size of the window
SCREEN_WIDTH = 160
SCREEN_HEIGHT = 100
viewer_bot_x = MAP_VIEWER_WIDTH
viewer_bot_y = MAP_VIEWR_HEIGHT
 
LIMIT_FPS = 20  #20 frames-per-second maximum

def move_screen(dx, dy):
		#moves the screen if that wouldn't cause the edge of the map to be exceeded.
		#If it would, it moves as much as it can without passing the edge of the map.
		viewer_bot_x = offset_x + MAP_VIEWER_WIDTH
		viewer_bot_y = offset_y + MAP_VIEWER_HEIGHT

		if offset_x + dx < 0: 
		        dx = offset_x * -1
	    
		elif viewer_bot_x + dx >= MAP_WIDTH:
		        dx = MAP_WIDTH - viewer_bot_x

		if  offset_y + dy < 0:
		        dy = offset_y * -1

		elif viewer_bot_y + dy >= MAP_HEIGHT:
		        dy = MAP_HEIGHT - viewer_bot_y

		
		offset_x = offset_x + dx
		offset_y = offset_y + dy
		
def render():
	for row in tmap:
		for element in row:
			if element == -1:
				char  = ' '
				color = libtcod.darker_green
			else: #there is a whole of code that will go here.
				char = unichr(87)
			libtcod.console_set_char_background(map_viewer, x, y, color, libtcod.BKGND_SET)
			y += 1
		y = 0
		x += 1
 
def handle_keys():
 
    key = libtcod.console_check_for_keypress()  #real-time
 
    if key.vk == libtcod.KEY_ENTER and libtcod.KEY_ALT:
        #Alt+Enter: toggle fullscreen
        libtcod.console_set_fullscreen(not libtcod.console_is_fullscreen())
 
    elif key.vk == libtcod.KEY_ESCAPE:
        return True  #exit game
 

 
 
#############################################
# Initialization & Main Loop
#############################################
 
libtcod.console_init_root(SCREEN_WIDTH, SCREEN_HEIGHT, 'Gnomescroll Worldgen', False)
libtcod.sys_set_fps(LIMIT_FPS)
 
while not libtcod.console_is_window_closed():
 
	render();
	
	
	#handle keys and exit game if needed
	exit = handle_keys()
	if exit:
		break