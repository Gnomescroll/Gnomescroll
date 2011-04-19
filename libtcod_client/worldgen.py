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

rnd=libtcod.random_new_from_seed(34534534)
noise=libtcod.noise_new(2, 0.5, 2.0 ,rnd) #First number has to do with smoothness/jaggedness of the form
# 3x3 kernel for smoothing operations
smoothKernelSize=9
smoothKernelDx=[-1,0,1,-1,0,1,-1,0,1]
smoothKernelDy=[-1,-1,-1,0,0,0,1,1,1]
smoothKernelWeight=[1.0,2.0,1.0,2.0,20.0,2.0,1.0,2.0,1.0]


# function building the heightmap
def buildMap(hm) :
	libtcod.heightmap_add_fbm(hm,noise,5,5,0,0,6,0.5,1)
	#for i in range(2,-1,-1):
		#libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
	libtcod.heightmap_normalize(hm,0,1)
	print "done"

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
			
	for x in range(MAP_WIDTH) :
		for y in range(MAP_HEIGHT) :
			z = libtcod.heightmap_get_value(hm,x,y)
			val=int(z*255) & 0xFF
			c=libtcod.Color(val/2,val/2,val)
			libtcod.console_set_char_background(map_viewer,x,y,c,libtcod.BKGND_SET)
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
# Initialization
#############################################
 
libtcod.console_init_root(SCREEN_WIDTH, SCREEN_HEIGHT, 'Gnomescroll Worldgen', False)
libtcod.sys_set_fps(LIMIT_FPS)

hm=libtcod.heightmap_new(MAP_WIDTH,MAP_HEIGHT)
buildMap(hm)

while not libtcod.console_is_window_closed():
 
	render();
	
	handle_mouse(libtcod.mouse_get_status())
	#handle keys and exit game if needed
	exit = handle_keys()
	
	if exit:
		break




# stuff to be used later
	
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