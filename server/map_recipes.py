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

from cube import dat
cubes = {} # TODO -- move this out of here
for id,v in dat.items():
    name = v.get('name', None)
    if name is None:
        continue
    cubes[name] = id

def names_available():
    print ""
    print "Cube names available:"
    for id,v in dat.items():
        name = v.get('name', None)
        if name is None:
            continue
        print name, id
    print ""

def test_names():
    missing_names = False
    names = [
        'dark_lunar_dust',
        'light_lunar_dust',
        'holy_stone',
        'infected_rock',
        'soft_rock',
        'hard_rock',
        'sand_stone',
        'blue_terminal',
        'green_terminal',
        'battery',
        'blue_ore',
        'holy_stone',
        'lava',
        'solar_panel',
    ]
    for n in names:
        if n not in cubes:
            missing_names = True
            print "%s not found in cube dat" % (n,)
    if missing_names:
        names_available()
        raise ValueError, "Cube names are missing, map gen will fail"
        
test_names()

def feb3_test_map():
    #setup
    x,y,z = 128,128,128

    init_c_lib.set_map_dimensions(x,y,z) # TODO:: get this value from the map gen or saved map    
    m = c_lib.map_gen
    m.init(x,y,z)
    m.conf.seed(opts.seed)

    #1
    m.conf\
    .size(x,y,z)\
    .tile(cubes['blue_terminal'])\
    .interpolate(4,4,2)\
    .scale(3.0, 3.0, 1.0)\
    .heightmap(baseline=20, maxheight=40)\
    .p2(octaves=9, persistence=20, frequency=0.01)\
    .start()\
    .reset()

    #2
    c_lib.map_gen.conf\
    .size(x,y,z)\
    .tile(cubes['battery'])\
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
    x,y,z = 512,512,128

    #feb3_test_map()
    #return
    #init_c_lib.set_map_dimensions(x,y,z) # TODO:: get this value from the map gen or saved map    

    c_lib.map_gen.init(x,y,z)
    c_lib.map_gen.conf.seed(opts.seed)
    _n = time.time()

    ##base heightmap, smooth shallow hills
    c_lib.map_gen.conf\
    .size(x,y,z)\
    .group(0)\
    .tile(cubes['sand_stone'])\
    .interpolate(4,2,1)\
    .scale(4.0, 4.0, 1.0)\
    .heightmap(baseline=40, maxheight=60)\
    .p2(octaves=6, persistence=0.45)\
    .start()\
    .reset()

     ###base heightmap, smooth shallowER hills
    c_lib.map_gen.conf\
    .size(x,y,z)\
    .group(5)\
    .scale(x=1.0, y=1.0, z=1.0)\
    .tile(cubes['soft_rock'])\
    .interpolate(2,4,1)\
    .heightmap(baseline=30, maxheight=45)\
    .p2(octaves=6, persistence=0.4)\
    .start()\
    .reset()

    ## short heightmap for bottom layer, blue ore
    c_lib.map_gen.conf\
    .size(x,y,z)\
    .group(112)\
    .tile(cubes['hard_rock'])\
    .interpolate(4,2,1)\
    .scale(4.0, 4.0, 1.0)\
    .heightmap(baseline=5, maxheight=25)\
    .p2(octaves=8, persistence=0.85)\
    .start()\
    .reset()

    c_lib.map_gen.grass(x,y,0, dirt=cubes['dark_lunar_dust'], grass=cubes['dark_lunar_dust'])

    ### 3d density noise. floating islands, overhangs
    #c_lib.map_gen.conf\
    #.size(x,y,z)\
    #.tile(cubes['holy_stone'])\
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
    .size(x,y,z)\
    .tile(cubes['sand_stone'])\
    .scale(x=4.0, y=4.0, z=1.0)\
    .group(1)\
    .gradient(z0=0.0, z1=-0.3)\
    .interpolate(4,4,2)\
    .density(threshold=0.75)\
    .p3(octaves=6, persistence=0.7)\
    .start()\
    .reset()

    c_lib.map_gen.grass(x,y,0, dirt=cubes['light_lunar_dust'], grass=cubes['light_lunar_dust'])

    ## 3d density noise, subtractive RMF. forms caves
    c_lib.map_gen.conf\
    .interpolate(4,2,4)\
    .size(x,y,z)\
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
    .size(x,y,z)\
    .gradient2()\
    .tile(0)\
    .scale(x=4.0, y=4.0, z=3.0)\
    .group(3)\
    .density(threshold=0.97)\
    .rmf()\
    .p3(octaves=6, persistence=0.7)\
    .start()\
    .reset()
    
     ##3d density noise, bottom half of map, lava
    c_lib.map_gen.conf\
    .interpolate(4,4,2)\
    .size(x,y,z/2)\
    .gradient2()\
    .tile(cubes['lava'])\
    .scale(x=4.0, y=4.0, z=3.0)\
    .group(101233)\
    .density(threshold=0.99)\
    .rmf()\
    .p3(octaves=6, persistence=0.7)\
    .start()\
    .reset()

     ##3d density noise, blue ore
    c_lib.map_gen.conf\
    .interpolate(2,2,2)\
    .size(x,y,z*0.75)\
    .gradient2()\
    .tile(cubes['infected_rock'])\
    .scale(x=4.0, y=4.0, z=3.0)\
    .group(1011)\
    .density(threshold=0.995)\
    .rmf()\
    .p3(octaves=6, persistence=0.9)\
    .start()\
    .reset()

    ##3d density noise, upper part of map, extruder blocks
    ## NOTE: very good
    c_lib.map_gen.conf\
    .interpolate(4,4,2)\
    .size(x,y,z)\
    .gradient(z0=0.0, z1=0.1)\
    .tile(cubes['extruder'])\
    .scale(x=4.0, y=4.0, z=1.0)\
    .group(88)\
    .density(threshold=0.998)\
    .rmf()\
    .p3(octaves=8, persistence=0.3, frequency=2.1)\
    .start()\
    .reset()

    """
    New gradient method
    Uses a parabolic falloff
    #"""
    ## 3d density noise. floating islands, overhangs
    #c_lib.map_gen.conf\
    #.size(x,y,z)\
    #.tile(cubes['holy_stone'])\
    #.scale(x=1.0, y=1.0, z=1.0)\
    #.gradient2()\
    #.interpolate(4,4,2)\
    #.density(threshold=0.75)\
    #.p3(octaves=6, persistence=0.7)\
    #.grass()\
    #.start()\
    #.reset()

    #c_lib.map_gen.ceiling(x,y,z,1,3)

    """
    - Perturbing the noisemap tests
    #c_lib.map_gen.conf\
    #.size(x,y,z)\
    #.group(0)\
    #.tile(cubes['holy_stone'])\
    #.interpolate(4,2,1)\
    #.p2(octaves=6, persistence=0.45)\
    #.start()
    #c_lib.map_gen.noise_parameters(octaves=6, persistence=0.9, lacunarity=2.0, frequency=1.0, amplitude=15.0)
    #c_lib.map_gen.perturb2d(x,y, turbulence=1.0, blend='-')
    #c_lib.map_gen.heightmap(x,y,z, 40, 40, base_tile=2)
    #c_lib.map_gen.grass(x,y, 0)
    """

    c_lib.map_gen.destroy()

    print "Best caves, map size %d,%d,%d, took %0.2f seconds" % (x,y,z, time.time() - _n)

import time

def gen_map_simple():
    x,y,z = 512,512,128
    #init_c_lib.set_map_dimensions(size,size,size) # TODO:: get this value from the map gen or saved map

    c_lib.map_gen.init(x,y,z)
    c_lib.map_gen.conf.seed(opts.seed)
    _n = time.time()

    ##base heightmap, smooth shallow hills
    c_lib.map_gen.conf\
    .size(x,y,z)\
    .group(0)\
    .tile(cubes['holy_stone'])\
    .interpolate(4,4,1)\
    .scale(4.0, 4.0, 1.0)\
    .heightmap(baseline=20, maxheight=30)\
    .p2(octaves=6, persistence=0.35,frequency=0.1)\
    .start()\
    .reset()

     ###base heightmap, smooth shallowER hills
    #c_lib.map_gen.conf\
    #.size(x,y,z)\
    #.group(5)\
    #.scale(x=4.0, y=4.0, z=1.0)\
    #.tile(cubes['holy_stone'])\
    #.interpolate(2,4,1)\
    #.heightmap(baseline=30, maxheight=45)\
    #.p2(octaves=6, persistence=0.4)\
    #.start()\
    #.reset()

def noisy_height():
    x,y,z = 512,512,128
    c_lib.map_gen.conf.seed(opts.seed)

    c_lib.map_gen.conf\
    .size(x,y,z)\
    .group(0)\
    .tile(cubes['holy_stone'])\
    .scale(1,1,1)\
    .heightmap(baseline=0, maxheight=z)\
    .p2(octaves=8, persistence=0.1, frequency=4.0)\
    .start()\
    .reset()

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
    .tile(cubes['blue_ore'])\
    .interpolate(4,2,1)\
    .scale(4.0, 4.0, 1.0)\
    .heightmap(baseline=22, maxheight=6)\
    .p2(octaves=6, persistence=0.35)\
    .start()\
    .reset()
    
    c_lib.map_gen.conf\
    .size(x,y,z)\
    .group(1)\
    .tile(cubes['blue_ore'])\
    .interpolate(4,2,1)\
    .scale(2.0, 2.0, 1.0)\
    .reverse_heightmap(baseline=37, maxheight=7)\
    .p2(octaves=6, persistence=0.6)\
    .start()\
    .reset()

    c_lib.map_gen.conf\
    .size(x,y,z)\
    .group(0)\
    .tile(cubes['solar_panel'])\
    .interpolate(4,2,1)\
    .scale(4.0, 4.0, 1.0)\
    .heightmap(baseline=13, maxheight=20, tile=cubes['solar_panel'])\
    .p2(octaves=6, persistence=0.6)\
    .start()\
    .reset()
    
    c_lib.map_gen.conf\
    .size(x,y,z)\
    .group(0)\
    .tile(0)\
    .interpolate(4,2,1)\
    .scale(2.0, 2.0, 1.0)\
    .reverse_heightmap(baseline=13, maxheight=12, minheight=3, tile=cubes['solar_panel'])\
    .p2(octaves=6, persistence=0.35)\
    .start()\
    .reset()

    #c_lib.map_gen.floor(x,y,14,1,cubes['blue_terminal'])

