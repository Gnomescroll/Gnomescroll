#!/usr/bin/python


### DEPRECATE WHOLE FILE ###

'''
Map gen
'''
import time
import random

'''
methods
'''
def load_map(terrain_map):
    m = terrain_map
    map_type = 5


    print "Start map generation"
    if map_type == 1:
        x_min = 0
        y_min = 0
        z_min = 0

        x_max = 64
        y_max = 64
        z_max = 64

        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    rnd = random.randint(0,64)
                    if rnd < 16:
                        rnd2 = random.randint(1,4)
                        m.set(xa,ya,za, rnd2)
        print "Finished map generation"

    if map_type == 2:
        x_min = -1
        y_min = -1
        z_min = 0

        x_max = 8
        y_max = 8
        z_max = 1

        #plane for testing z collision
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    rnd = random.randint(0,64)
                    if True or rnd < 16:
                        rnd2 = random.randint(1,4)
                        rnd2 = 3
                        m.set(xa,ya,za, rnd2)
        #protrusions for testing collision detection
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    rnd = random.randint(0,64)
                    if rnd < 16:
                        m.set(xa,ya,za+1, 4)
                        m.set(xa,ya,za+2, 4)
                        m.set(xa,ya,za+3, 5)
                        m.set(xa,ya,za+4, 6)
        print "Finished map generation"

    if map_type == 3:
        x_min = -256
        y_min = -256
        z_min = 0

        x_max = 256
        y_max = 256
        z_max = 1

        #plane for testing z collision
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    rnd = random.randint(0,64)
                    if True or rnd < 48:
                        rnd2 = random.randint(1,4)
                        rnd2 = 3
                        m.set(xa,ya,za, rnd2)
        #protrusions for testing collision detection
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    rnd = random.randint(0,64)
                    if rnd < 4:
                        m.set(xa,ya,za+1, 4)
                        m.set(xa,ya,za+2, 4)
                        m.set(xa,ya,za+3, 5)
                        m.set(xa,ya,za+4, 6)
        print "Finished map generation"

    if map_type == 4:
        x_min = 0
        y_min = 0
        z_min = 0

        x_max = 128#64
        y_max = 128#64
        z_max = 1

        #plane for testing z collision
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    m.set(xa,ya,za, 1)
        #protrusions for testing collision detection
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    rnd = random.randint(0,64)
                    if rnd < 8:
                        height =random.randint(1,5)
                        for th in range(1,height):
                            m.set(xa,ya,za+th, 4)
                    elif rnd <10:
                        height =random.randint(1,15)
                        for th in range(1,height):
                            m.set(xa,ya,za+th, 4)
                        #m.set(xa,ya,za+4, 3)
        print "Finished map generation"

    if map_type == 5:
        x_min = 0
        y_min = 0
        z_min = 0

        x_max = 512#64
        y_max = 512#64
        z_max = 1

        #plane for testing z collision
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    m.set(xa,ya,za, 2)
        #protrusions for testing collision detection
        for xa in range(x_min, x_max):
            for ya in range(y_min, y_max):
                for za in range(z_min, z_max):
                    rnd = random.randint(0,64)
                    if rnd < 8:
                        height =random.randint(1,5)
                        for th in range(1,height):
                            m.set(xa,ya,za+th, 17)
                    elif rnd <10:
                        height =random.randint(1,15)
                        for th in range(1,height):
                            m.set(xa,ya,za+th, 17)
        print "Finished map generation"

    if map_type == 6:
        m.set(1,1,3, 17)
        m.set(2,2,3, 17)
        #m.set(2,2,3, 17)

# fill map completely
def load_map_fill(terrain_map):
    _set = terrain_map.set
    print 'start map gen'
    for i in range(64*8):
        for j in range(64*8):
            for k in range(8*16):
                _set(i,j,k, 2)
    #print 'attempt to save map'
    #terrain_map.save_to_disk()
    print 'done map gen'

def grass(terrain_map):
    print 'start grass'
    _n = time.time()

    xd = 128
    yd=128
    xdr=range(xd)
    ydr=range(yd)

    for i in xdr:
        for j in ydr:
            k = terrain_map.get_highest_open_block(i,j)
            k -= 1
            if k < 0:
                continue
            lava = terrain_map.get(i,j,k)
            if lava == 3:   # dont replace lava
                continue
            terrain_map.set(i,j,k, 4) # grass
    print 'grass done'
    print 'took %0.2f seconds' % (time.time() - _n)

def castle_column1(tm, x,y,z,height,tile=10):
    for _z in range(z, z+height):
        tm.set(x,y,_z, tile)

def castle_wall1(tm, _x, _y, _z, dx,dy,dz, length, height, tile=10):
    for l in range(0,length):
        for h in range(0,height):
            tm.set(_x,_y,_z+h, tile)
        if l % 2 == 0:
            tm.set(_x,_y,_z+height, tile)
        _x+=dx
        _y+=dy
        _z+=dz

def castle_tower1(tm, _x,_y,_z, height, wall_length, tile=10):
    #wall 1
    dx,dy,dz = 1,0,0
    x,y,z = _x,_y,_z
    castle_wall1(tm, x,y,z, dx,dy,dz, wall_length, height, tile)
    #wall 3
    dx,dy,dz = 0,1,0
    x,y,z = _x,_y,_z
    castle_wall1(tm, x,y,z, dx,dy,dz, wall_length, height, tile)
    #wall 2
    dx,dy,dz = 0,-1,0
    x,y,z = _x+wall_length-1,_y+wall_length-1,_z
    castle_wall1(tm, x,y,z, dx,dy,dz, wall_length, height, tile)
    #wall 4
    dx,dy,dz = -1,0,0
    x,y,z = _x+wall_length-1,_y+wall_length-1,_z
    castle_wall1(tm, x,y,z, dx,dy,dz, wall_length, height, tile)

def castle_tower2(tm, _x,_y,_z, height=10, wall_length=3, tile=10):
    #wall 1
    dx,dy,dz = 1,0,0
    x,y,z = _x+2,_y,_z
    castle_wall1(tm, x,y,z, dx,dy,dz, wall_length, height, tile=tile)
    #wall 3
    dx,dy,dz = 0,1,0
    x,y,z = _x,_y+2,_z
    castle_wall1(tm, x,y,z, dx,dy,dz, wall_length, height, tile=tile)
    #wall 2
    dx,dy,dz = 0,1,0
    x,y,z = _x+3+wall_length,_y+2,_z
    castle_wall1(tm, x,y,z, dx,dy,dz, wall_length, height, tile=tile)
    #wall 4
    dx,dy,dz = 1,0,0
    x,y,z = _x+2,_y+3+wall_length,_z
    castle_wall1(tm, x,y,z, dx,dy,dz, wall_length, height, tile=tile)

    x,y,z = _x+1,_y+1,_z
    castle_column1(tm, x,y,z,height, tile=tile)
    x,y,z = _x+2+wall_length,_y+1,_z
    castle_column1(tm, x,y,z,height, tile=tile)
    x,y,z = _x+1,_y+2+wall_length,_z
    castle_column1(tm, x,y,z,height, tile=tile)
    x,y,z = _x+2+wall_length,_y+2+wall_length,_z
    castle_column1(tm, x,y,z,height, tile=tile)

def ground_plane(tm, xsize=64, ysize=64, tile=100):
    for x in range(0,xsize):
        for y in range(0,ysize):
            tm.set(x,y,0,tile)
