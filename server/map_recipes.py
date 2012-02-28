import c_lib.map_gen


'''
Noise notes:

For each octave:
    amplitude *= persistence
    frequency *= lacunarity

As persistence approaches 1.0, it gets very bumpy. Above 1.0, extremely bumpy.
0.6 seems to be the border for bumpiness

RMF (ridged multifractal) properties:
Always positive
Sharp peaks/ edges.

DO NOT USE SIMPLEX3. probably dont use simplex2 either. it is bad broken code stolen

'''

import init_c_lib

import args_server
import opts
opts.opts = args_server.get_args()
opts = opts.opts

def feb3_test_map():
    #setup
    init_c_lib.set_map_dimensions(128,128,128) # TODO:: get this value from the map gen or saved map    
    m = c_lib.map_gen
    m.init(128,128, 128)
    m.conf.seed(opts.seed)

    #1
    m.conf\
    .size(128,128,128)\
    .tile(101)\
    .interpolate(4,4,2)\
    .scale(3.0, 3.0, 1.0)\
    .heightmap(baseline=20, maxheight=40)\
    .p2(octaves=9, persistence=20, frequency=0.01)\
    .start()\
    .reset()

    #2
    c_lib.map_gen.conf\
    .size(128,128,128)\
    .tile(102)\
    .scale(x=4.0, y=4.0, z=1.0)\
    .group(1)\
    .gradient(z0=0.0, z1=-0.3)\
    .interpolate(4,4,2)\
    .density(threshold=0.75)\
    .p3(octaves=6, persistence=0.7)\
    .start()\
    .reset()
    
    m.destroy()


def best_caves():

    #feb3_test_map()
    #return
    #init_c_lib.set_map_dimensions(512,512,128) # TODO:: get this value from the map gen or saved map    

    c_lib.map_gen.init(512,512,128)
    c_lib.map_gen.conf.seed(opts.seed)
    _n = time.time()

    ##base heightmap, smooth shallow hills
    c_lib.map_gen.conf\
    .size(512,512,128)\
    .group(0)\
    .tile(2)\
    .interpolate(4,2,1)\
    .scale(4.0, 4.0, 1.0)\
    .heightmap(baseline=40, maxheight=60)\
    .p2(octaves=6, persistence=0.45)\
    .start()\
    .reset()

     ###base heightmap, smooth shallowER hills
    c_lib.map_gen.conf\
    .size(512,512,128)\
    .group(5)\
    .scale(x=1.0, y=1.0, z=1.0)\
    .tile(2)\
    .interpolate(2,4,1)\
    .heightmap(baseline=30, maxheight=45)\
    .p2(octaves=6, persistence=0.4)\
    .start()\
    .reset()

    c_lib.map_gen.grass(512,512,0)

    ### 3d density noise. floating islands, overhangs
    #c_lib.map_gen.conf\
    #.size(512,512,128)\
    #.tile(2)\
    #.scale(x=3.5, y=3.5, z=1.0)\
    #.gradient2()\
    #.group(1)\
    #.interpolate(4,4,2)\
    #.density(threshold=0.75)\
    #.p3(octaves=6, persistence=0.7)\
    #.start()\
    #.reset()

    ## 3d density noise. floating islands, overhangs, same seed different scale
    c_lib.map_gen.conf\
    .size(512,512,128)\
    .tile(2)\
    .scale(x=4.0, y=4.0, z=1.0)\
    .group(1)\
    .gradient(z0=0.0, z1=-0.3)\
    .interpolate(4,4,2)\
    .density(threshold=0.75)\
    .p3(octaves=6, persistence=0.7)\
    .start()\
    .reset()

    c_lib.map_gen.grass(512,512,0)

    ## 3d density noise, subtractive RMF. forms caves
    c_lib.map_gen.conf\
    .interpolate(4,2,4)\
    .size(512,512,128)\
    .tile(0)\
    .scale(x=4.0, y=4.0, z=2.0)\
    .group(2)\
    .density(threshold=0.97)\
    .gradient2()\
    .rmf()\
    .p3(octaves=6, persistence=0.8)\
    .start()\
    .reset()

     ##3d density noise, subtractive RMF. forms caves
    c_lib.map_gen.conf\
    .interpolate(2,4,2)\
    .size(512,512,128)\
    .gradient2()\
    .tile(0)\
    .scale(x=4.0, y=4.0, z=3.0)\
    .group(3)\
    .density(threshold=0.97)\
    .rmf()\
    .p3(octaves=6, persistence=0.7)\
    .start()\
    .reset()

    """
    New gradient method
    Uses a parabolic falloff
    #"""
    ## 3d density noise. floating islands, overhangs
    #c_lib.map_gen.conf\
    #.size(512,512,128)\
    #.tile(2)\
    #.scale(x=1.0, y=1.0, z=1.0)\
    #.gradient2()\
    #.interpolate(4,4,2)\
    #.density(threshold=0.75)\
    #.p3(octaves=6, persistence=0.7)\
    #.grass()\
    #.start()\
    #.reset()

    #c_lib.map_gen.ceiling(512,512,128,1,3)

    """
    - Perturbing the noisemap tests
    #c_lib.map_gen.conf\
    #.size(512,512,128)\
    #.group(0)\
    #.tile(2)\
    #.interpolate(4,2,1)\
    #.p2(octaves=6, persistence=0.45)\
    #.start()
    #c_lib.map_gen.noise_parameters(octaves=6, persistence=0.9, lacunarity=2.0, frequency=1.0, amplitude=15.0)
    #c_lib.map_gen.perturb2d(512,512, turbulence=1.0, blend='-')
    #c_lib.map_gen.heightmap(512,512,128, 40, 40, base_tile=2)
    #c_lib.map_gen.grass(512,512, 0)
    """

    c_lib.map_gen.destroy()

    print "512,512,128 shit took %0.2f seconds" % (time.time() - _n)

import time

def gen_map_simple():
    size = 512
    #init_c_lib.set_map_dimensions(size,size,size) # TODO:: get this value from the map gen or saved map

    c_lib.map_gen.init(512, 512 ,128)
    c_lib.map_gen.conf.seed(opts.seed)
    _n = time.time()

    ##base heightmap, smooth shallow hills
    c_lib.map_gen.conf\
    .size(512,512,128)\
    .group(0)\
    .tile(2)\
    .interpolate(4,4,1)\
    .scale(4.0, 4.0, 1.0)\
    .heightmap(baseline=20, maxheight=30)\
    .p2(octaves=6, persistence=0.35,frequency=0.1)\
    .start()\
    .reset()

     ###base heightmap, smooth shallowER hills
    #c_lib.map_gen.conf\
    #.size(128,128,128)\
    #.group(5)\
    #.scale(x=4.0, y=4.0, z=1.0)\
    #.tile(2)\
    #.interpolate(2,4,1)\
    #.heightmap(baseline=30, maxheight=45)\
    #.p2(octaves=6, persistence=0.4)\
    #.start()\
    #.reset()

'''
def pallet_pillar(x,y,z):
    for i in range(0,32):
        m = init_c_lib.
        m.set(x,y,z+2*i, i)

def clear_pillar(x,y):
     for i in range(0,128):
        m = init_c_lib.
        m.set(x,y,i, 0)
'''

def draw_sphere(block, radius, x,y,z):
    #m = init_c_lib.
    t = int(radius)+1
    radius = float(radius)
    fx = float(x);fy=float(y);fz=float(z)
    for _x in range(-t,t):
        for _y in range(-t,t):
            for _z in range (-t,t):
                d = (float(_x))**2+(float(_y))**2+(float(_z))**2
                #print "d= %s" % (str(d))
                if d < radius**2:
                    m.set(_x+x,_y+y,_z+z,block)

def good_cave1():
    print "Good cave"
    #c_lib.map_gen.Dragon.generate()
    #c_lib.map_gen.Dungeon.generate()
    
    #base heightmap, smooth shallow hills

    x,y,z = 512,512,128
    
    c_lib.map_gen.conf\
    .size(x,y,z)\
    .group(0)\
    .tile(3)\
    .interpolate(4,2,1)\
    .scale(4.0, 4.0, 1.0)\
    .heightmap(baseline=22, maxheight=6)\
    .p2(octaves=6, persistence=0.35)\
    .start()\
    .reset()
    
    c_lib.map_gen.conf\
    .size(x,y,z)\
    .group(1)\
    .tile(3)\
    .interpolate(4,2,1)\
    .scale(2.0, 2.0, 1.0)\
    .reverse_heightmap(baseline=37, maxheight=7)\
    .p2(octaves=6, persistence=0.6)\
    .start()\
    .reset()

    c_lib.map_gen.conf\
    .size(x,y,z)\
    .group(0)\
    .tile(103)\
    .interpolate(4,2,1)\
    .scale(4.0, 4.0, 1.0)\
    .heightmap(baseline=13, maxheight=20, tile=103)\
    .p2(octaves=6, persistence=0.6)\
    .start()\
    .reset()
    
    c_lib.map_gen.conf\
    .size(x,y,z)\
    .group(0)\
    .tile(0)\
    .interpolate(4,2,1)\
    .scale(2.0, 2.0, 1.0)\
    .reverse_heightmap(baseline=13, maxheight=12, minheight=3, tile=103)\
    .p2(octaves=6, persistence=0.35)\
    .start()\
    .reset()

    #c_lib.map_gen.floor(128,128,14,1,101)

