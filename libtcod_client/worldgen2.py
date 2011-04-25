from __future__ import division
import libtcodpy as libtcod
import random

 
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
LEVELS_ABOVE = 20
LEVELS_BELOW = 64
z_to_display = 19

#For creating noise for heightmaps:
#First number = degrees of noise 1-4, should always be 2.
#Second number needs to decrease as the map size increases, adjusts "zoom"
#Third number is seems to adjust variance - a value around 20 makes a 
#detailed noise model, but the noise in general is smoother, larger, and farther apart.

LIMIT_FPS = 20  #20 frames-per-second maximum

SEED = 555555

local_detail_random=libtcod.random_new_from_seed(SEED)
local_detail_noise=libtcod.noise_new(2, 0.5, 2, local_detail_random) 

roughness_random = libtcod.random_new_from_seed(SEED)
roughness_noise=libtcod.noise_new(2, 0.5, 20, roughness_random)

elevation_random = libtcod.random_new_from_seed(SEED)
elevation_noise=libtcod.noise_new(2, 0.5, 20, elevation_random)

# 3x3 kernel for smoothing operations
smoothKernelSize=9
smoothKernelDx=[-1,0,1,-1,0,1,-1,0,1]
smoothKernelDy=[-1,-1,-1,0,0,0,1,1,1]
smoothKernelWeight=[1.0,2.0,1.0,2.0,20.0,2.0,1.0,2.0,1.0]

# function building the heightmap
def build_local_detail(hm) :
	libtcod.heightmap_add_fbm(hm,local_detail_noise,5,5,0,0,6,0.5,1)
	#for i in range(2,-1,-1):
		#libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
	#libtcod.heightmap_normalize(hm,0,1)
	print "done with local detail map"
	
# function building the heightmap
def build_roughness(hm) :
	libtcod.heightmap_add_fbm(hm,roughness_noise,5,5,0,0,6,0.5,1)
	for i in range(2,-1,-1):
		libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
	for i in range(2,-1,-1):
		libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
	#libtcod.heightmap_normalize(hm,0,1)
	print "done with roughness map"
	
# function building the heightmap
def build_elevation(hm) :
	libtcod.heightmap_add_fbm(hm,elevation_noise,5,5,0,0,6,0.5,1)
	for i in range(2,-1,-1):
		libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
	for i in range(2,-1,-1):
		libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
	for i in range(2,-1,-1):
		libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
	for i in range(2,-1,-1):
		libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
	#libtcod.heightmap_normalize(hm,0,1)
	print "done with elevation map"

def move_screen(dx, dy):
	global viewer_top_x, viewer_top_y, viewer_bot_x, viewer_bot_y
	#moves the screen if that wouldn't cause the edge of the map to be exceeded.
	#If it would, it moves as much as it can without passing the edge of the map.
	#CRASH: Not working for some reason right now, still a crash if it moves off, probably just a last index issue
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
	viewer_bot_x = viewer_top_x + SCREEN_WIDTH
	viewer_bot_y = viewer_top_y + SCREEN_HEIGHT
	print viewer_top_x, viewer_top_y, viewer_bot_x, viewer_bot_y
            
def render():
	global top_terrain
	x = 0;
	y = 0;
	libtcod.console_flush()

	for x in range(viewer_top_x, viewer_bot_x):
		for y in range(viewer_top_y, viewer_bot_y):
			z = top_terrain[x][y][z_to_display]
			if z == 0:
				c = libtcod.lighter_blue
			else:
				c = libtcod.black
			libtcod.console_set_char_background(map_viewer, x, y, c, libtcod.BKGND_SET)
	
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
	global z_to_display, view_mode
	key = libtcod.console_check_for_keypress()  #real-time

	if key.vk == libtcod.KEY_DOWN:
	#Down to the next z-level
		if  z_to_display != 0:
			z_to_display = z_to_display - 1
			print "Current z levels is", z_to_display
	
	elif key.vk == libtcod.KEY_UP:
	#Up to the next z-level
		if z_to_display != (LEVELS_ABOVE) - 1:
			z_to_display = z_to_display + 1
			print "Current z levels is", z_to_display
	
	elif key.vk == libtcod.KEY_ESCAPE:
		return True  #exit game
 
def translate_to_terrain(hm):
	#This function will be used to erode the top of the perlin noise using a heightmap
	"""terrain = [[[0 for z in range(LEVELS_ABOVE + LEVELS_BELOW)] for y in range(MAP_HEIGHT)] for x in range(MAP_WIDTH)]
	for x in range(MAP_WIDTH):
		for y in range(MAP_HEIGHT):
			height = (int(libtcod.heightmap_get_value(final_hm, x ,y)*64 + 64))
			for z in range(height):
				terrain[x][y][z] = 1
	return terrain"""
	
def create_top_terrain(noise):
	#zoom must be equal to or greater than the map size
	ZOOM = 200.0
	cutoff = -1.1
	terrain = [[[0 for z in range(LEVELS_ABOVE)] for y in range(MAP_HEIGHT)] for x in range(MAP_WIDTH)]
	for z in range(0, LEVELS_ABOVE):
		print "Now generating level", z+1, "out of", LEVELS_ABOVE, "total levels."
		if z <= 10:
			cutoff = cutoff + 0.1
		for x in range(0, MAP_WIDTH):
			for y in range(0, MAP_WIDTH):
				terrain[x][y][z] = libtcod.noise_get_fbm(noise, [(x/ZOOM), (y/ZOOM), (z/ZOOM)], 32.0, libtcod.NOISE_PERLIN)
				if terrain[x][y][z] < cutoff:
					terrain[x][y][z] = 0
				else:
					terrain[x][y][z] = 1
	return terrain
	
def drange(start, stop, step):
	#Stole this from stackoverflow
	r = start
	while r < stop:
		yield r
		r += step
	
#############################################
# Initialization
#############################################
 
libtcod.console_init_root(SCREEN_WIDTH, SCREEN_HEIGHT, 'Gnomescroll Worldgen', False)
libtcod.sys_set_fps(LIMIT_FPS)

###########
## Generate 3D Perlin noise for the top of the world.
## Not worrying about below ground just yet
###########

noise3d = libtcod.noise_new(3, 0.5, 2.0)
libtcod.noise_set_type(noise3d, libtcod.NOISE_PERLIN)
top_terrain = create_top_terrain(noise3d)

###########
## Heightmap is flipped upside down and used for top boundaries.
###########
local_detail_hm=libtcod.heightmap_new(MAP_WIDTH,MAP_HEIGHT)
build_local_detail(local_detail_hm)

roughness_hm=libtcod.heightmap_new(MAP_WIDTH,MAP_HEIGHT)
build_roughness(roughness_hm)

elevation_hm=libtcod.heightmap_new(MAP_WIDTH,MAP_HEIGHT)
build_elevation(elevation_hm)

multiplied_hm = libtcod.heightmap_new(MAP_WIDTH, MAP_HEIGHT)
libtcod.heightmap_multiply_hm(local_detail_hm, roughness_hm, multiplied_hm)
libtcod.heightmap_delete(local_detail_hm)
libtcod.heightmap_delete(roughness_hm)

final_hm = libtcod.heightmap_new(MAP_WIDTH, MAP_HEIGHT)
libtcod.heightmap_add_hm(elevation_hm, multiplied_hm, final_hm)
libtcod.heightmap_delete(elevation_hm)
libtcod.heightmap_delete(multiplied_hm)

libtcod.heightmap_normalize(final_hm)
print "Top boundary heightmap generated."

while not libtcod.console_is_window_closed():
 
	render();
	
	handle_mouse(libtcod.mouse_get_status())
	#handle keys and exit game if needed
	exit = handle_keys()
	
	if exit:
		break