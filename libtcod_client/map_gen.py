import libtcodpy as libtcod

MAP_WIDTH = 200
MAP_HEIGHT = 200
LEVELS_ABOVE = 20
LEVELS_BELOW = 64


#For creating noise for heightmaps:
#First number = degrees of noise 1-4, should always be 2.
#Second number needs to decrease as the map size increases, adjusts "zoom"
#Third number is seems to adjust variance - a value around 20 makes a 
#detailed noise model, but the noise in general is smoother, larger, and farther apart.


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
	#print "done with local detail map"
	
# function building the heightmap
def build_roughness(hm) :
	libtcod.heightmap_add_fbm(hm,roughness_noise,5,5,0,0,6,0.5,1)
	for i in range(2,-1,-1):
		libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
	for i in range(2,-1,-1):
		libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
	#libtcod.heightmap_normalize(hm,0,1)
	#print "done with roughness map"
	
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
	#print "done with elevation map"
	
def create_top_terrain(noise, tx=MAP_WIDTH, ty=MAP_HEIGHT, tz=LEVELS_ABOVE):
	#zoom must be equal to or greater than the map size
	ZOOM = 200.0
	cutoff = -1.1
	terrain = [[[0 for z in range(tz)] for y in range(ty)] for x in range(tx)]
	for z in range(0, tz):
		#print "Now generating level", z+1, "out of", tz, "total levels."
		if z <= 10:
			cutoff = cutoff + 0.1
		for x in range(0, tx):
			for y in range(0, tx):
				terrain[x][y][z] = libtcod.noise_get_fbm(noise, [(x/ZOOM), (y/ZOOM), (z/ZOOM)], 32.0, libtcod.NOISE_PERLIN)
				if terrain[x][y][z] < cutoff:
					terrain[x][y][z] = 0
				else:
					terrain[x][y][z] = 1
	return terrain
	
def save_terrain(filename='test'):
    """Rude and Crude."""
    import cPickle
    global top_terrain	
    f = open(filename, 'wb')
    cPickle.dump(top_terrain, f)
    f.close()
    return

def load_terrain(filename='test'):
    """Equally if not more Rude and Crude, dude."""
    import cPickle
    global top_terrain
    f = open(filename, 'rb')
    top_terrain = cPickle.load(f)
    f.close()
    return
    


###########
## Generate 3D Perlin noise for the top of the world.
## Not worrying about below ground just yet
###########

noise3d = libtcod.noise_new(3, 0.5, 2.0)
libtcod.noise_set_type(noise3d, libtcod.NOISE_PERLIN)
top_terrain = create_top_terrain(noise3d, MAP_WIDTH, MAP_HEIGHT, LEVELS_ABOVE)

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
#print "Top boundary heightmap generated."
