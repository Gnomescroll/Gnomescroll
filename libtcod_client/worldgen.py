import libtcodpy as libtcod
 
#actual size of the window
SCREEN_WIDTH = 80
SCREEN_HEIGHT = 50
viewer_bot_x = SCREEN_WIDTH
viewer_bot_y = SCREEN_HEIGHT
MAP_WIDTH = 5
MAP_HEIGHT = 10
map_viewer = libtcod.console_new(MAP_WIDTH, MAP_HEIGHT)

LIMIT_FPS = 20  #20 frames-per-second maximum

map = [[-1 for col in range(5)] for row in range(10)]

print map

"""def move_screen(dx, dy):
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
		offset_y = offset_y + dy"""
		
def render():
	x = 0;
	y = 0;
	libtcod.console_flush()
	for x, row in enumerate(map):
		for y, element in enumerate(row):
			if element == -1:
				color = libtcod.white
			libtcod.console_set_default_background(map_viewer, color)
			libtcod.console_put_char(map_viewer, x, y, " ", libtcod.BKGND_SET)
	libtcod.console_blit(map_viewer, 0, 0, 80, 50, 0, 0, 0)
	
 
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