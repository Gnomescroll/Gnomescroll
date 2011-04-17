import libtcodpy as libtcod
 
#actual size of the window
SCREEN_WIDTH = 80
SCREEN_HEIGHT = 50
viewer_top_x = 0
viewer_top_y = 0
viewer_bot_x = SCREEN_WIDTH
viewer_bot_y = SCREEN_HEIGHT
MAP_WIDTH = 200
MAP_HEIGHT = 200
map_viewer = libtcod.console_new(MAP_WIDTH, MAP_HEIGHT)
mouse_on_drag_start = None

LIMIT_FPS = 20  #20 frames-per-second maximum

#3 zoom
#defaults
#6 octaves
noise_octaves = 4.0
noise_zoom = 3.0
noise_hurst = libtcod.NOISE_DEFAULT_HURST
noise_lacunarity = libtcod.NOISE_DEFAULT_LACUNARITY

map = [[-1 for col, x in enumerate(range(MAP_WIDTH))] for row, y in enumerate(range(MAP_HEIGHT))]

noise = libtcod.noise_new(2, noise_hurst, noise_lacunarity)
for y in range(MAP_HEIGHT):
        for x in range(MAP_WIDTH):
            f = [noise_zoom * x / (MAP_WIDTH), noise_zoom * y / (MAP_HEIGHT)]
            map[x][y] = libtcod.noise_get(noise, f, libtcod.NOISE_PERLIN)
			
			
def move_screen(dx, dy):
	global viewer_top_x, viewer_top_y, viewer_bottom_x, viewer_bottom_y
	
	#moves the screen if that wouldn't cause the edge of the map to be exceeded.
	#If it would, it moves as much as it can without passing the edge of the map.
	viewer_bot_x = viewer_top_x + SCREEN_WIDTH
	viewer_bot_y = viewer_top_y + SCREEN_HEIGHT

	if viewer_top_x + dx < 0: 
			dx = viewer_top_x * -1
	
	elif viewer_bot_x + dx >= MAP_WIDTH:
			dx = MAP_WIDTH - viewer_bot_x

	if  viewer_top_y + dy < 0:
			dy = viewer_top_y * -1

	elif viewer_bot_y + dy >= MAP_HEIGHT:
			dy = MAP_HEIGHT - viewer_bot_y

	
	viewer_top_x = viewer_top_x + dx
	viewer_top_y = viewer_top_y + dy
		
def render():
	global SCREEN_WIDTH, SCREEN_HEIGHT, map_viewer
	x = 0;
	y = 0;
	libtcod.console_flush()
	for x, row in enumerate(map):
		for y, element in enumerate(row):
		
			"""if element == -1:
				color = libtcod.black
			elif element == 0:
				color = libtcod.darkest_blue
			elif element == 1:
				color = libtcod.dark_blue
			elif element == 2:
				color = libtcod.blue
			elif element == 3:
				color = libtcod.light_blue
			elif element == 4:
				color = libtcod.lighter_blue
			elif element == 5:
				color = libtcod.lightest_blue
			elif element == 6:
				color = libtcod.lighter_amber
			elif element == 7:  
				color = libtcod.lightest_green
			elif element == 8:  
				color = libtcod.lighter_green
			elif element == 9:  
				color = libtcod.light_green
			elif element == 10: 
				color = libtcod.green
			elif element == 11: 
				color = libtcod.dark_green
			elif element == 12: 
				color = libtcod.darker_green
			elif element == 13:
				color = libtcod.darkest_green
			elif element == 14:
				color = libtcod.darker_sepia
			elif element == 15:
				color = libtcod.darkest_sepia
			elif element == 16:
				color = libtcod.darker_grey
			elif element == 17:
				color = libtcod.grey
			elif element == 18:
				color = libtcod.light_grey
			elif element == 19:
				color = libtcod.white"""

			h = map[x][y];
			c = int((h + 1.0) / 2.0 * 255)
			if c < 0:
				c = 0
			elif c > 255:
				c = 255
			col = libtcod.Color(c / 2, c / 2, c)
			libtcod.console_set_default_background(map_viewer, c)
			libtcod.console_put_char(map_viewer, x, y, " ", libtcod.BKGND_SET)
	libtcod.console_blit(map_viewer, viewer_top_x, viewer_top_y, viewer_bot_x, viewer_bot_y, 0, 0, 0)
	
def handle_mouse(current_mouse):
	global mouse_on_drag_start
	if current_mouse.lbutton:
		dx = mouse_on_drag_start.cx - current_mouse.cx;
		dy = mouse_on_drag_start.cy - current_mouse.cy;
		move_screen(dx, dy);
		mouse_on_drag_start = current_mouse;
	else:
		mouse_on_drag_start = current_mouse;
		
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
	
	handle_mouse(libtcod.mouse_get_status())
	#handle keys and exit game if needed
	exit = handle_keys()
	
	if exit:
		break