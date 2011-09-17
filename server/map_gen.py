#!/usr/bin/python

'''
Map gen
'''

import noise
import random
class Gen:

    def __init__(self, o=10, p=0.5, f=1.3, a=1, seed=0.9, xint=128, yint=128, zint=128, salt=0):
        self.octaves = o
        self.num_octaves = range(o)
        self.persistence = p
        self.frequency = f
        self.amplitude = a
        self.seed = 2 + seed**2
        self.xint = xint
        self.yint = yint
        self.zint = zint
        self.salt = salt

    def getHeight(self, x, y):
        #return self.amplitude * self.total(x,y)
        x /= float(self.xint)
        y /= float(self.yint)
        return self.amplitude * self.noise(x, y)

    def getDensity(self, x,y,z, octaves=None, persistence=None):
        x /= float(self.xint)
        y /= float(self.yint)
        z /= float(self.zint)
        return self.amplitude * self.noise3d(x,y,z, octaves=octaves, persistence=persistence)

    def noise(self, x, y):
        n = noise.pnoise2(x+self.salt, y+self.salt, octaves=self.octaves, persistence=self.persistence)
        return n

    def noise3d(self, x,y,z, octaves=None, persistence=None):
        persistence = persistence or self.persistence
        octaves = octaves or self.persistence
        n = noise.pnoise3(x+self.salt, y+self.salt, z+self.salt, octaves=octaves, persistence=persistence)
        return n

    #def snoise(self, x, y):
        ## noise package broken, this will fail
        #n = noise.snoise2(x+self.salt, y+self.salt, octaves=self.octaves, persistence=self.persistence)
        #return n


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

def load_map1(terrain_map):
    print 'Begin generating map'
    octaves = 6
    f = 200.0 / octaves

    x = 256
    y = 256
    z = 5

    rx = range(x)
    ry = range(y)
    rz = range(z)

    for i in rx:
        for j in ry:
            for k in rz:
                v = noise.pnoise3(i/f, j/f, k/f, octaves=octaves, persistence=10, repeatx=32, base=7)
                if v < 0:
                    p = 0
                else:
                    v *= 100
                    if 0 <= v < 5:
                        p = 1
                    elif 5 <= v < 10:
                        p = 0
                    elif 10 <= v < 15:
                        p = 3
                    elif 15 <= v < 20:
                        p = 0
                    else:
                        p = 5
                terrain_map.set(i,j,k, p)
    print 'Done generating map'

# simple perlin generator, baseline height 2 (small mountains + lava on top)
def load_map2(terrain_map):
    print 'start map gen'
    max_height = 15
    g = Gen(salt=random.random())
    h=0
    for i in range(128):
        for j in range(128):
            h = g.getHeight(i,j)
            h = abs(h)
            h *= 100
            h %= max_height
            h = int(h)
            for k in range(h+1):
                terrain_map.set(i, j, k, 2)
            if h==0 or h==1:
                terrain_map.set(i,j, 2, 3)
    print 'done map gen'


# simple perlin generator, baseline height 100 (big cube, small mountains + lava on top)
def load_map3(terrain_map):
    import time
    _n = time.time()
    print 'start map gen'
    max_height = 15
    baseline = 100
    g = Gen(salt=random.random())
    h=0
    for i in range(512):
        for j in range(512):
            h = g.getHeight(i,j)
            h = abs(h)
            h *= 100
            h %= max_height
            h = int(h)
            for k in range(h+baseline):
                terrain_map.set(i, j, k, 2)
            if h<=1:
                terrain_map.set(i,j, baseline+1, 3)
                if h == 0:
                    terrain_map.set(i,j, baseline, 3)

    print 'done map gen'
    print 'took %d seconds' % (time.time() - _n)

# cave cube (test)
def load_map4(terrain_map):
    import time
    _n = time.time()
    print 'start map gen'
    max_height = 15
    baseline = 100
    xd = 128
    yd = 128
    zd = 128
    g = Gen(salt=random.random(), xint=xd, yint=yd, zint=zd)
    h=0

    xdr = range(xd)
    ydr = range(yd)
    zdr = range(zd)

    # fill in baseline cube with density tubes
    for i in xdr:
        for j in ydr:
            for k in zdr:
                solid = g.getDensity(i,j,k, persistence=0.8, octaves=8)
                if solid >= -0.1:
                    terrain_map.set(i,j,k, 2)

    ## do height
    #for i in range(512):
        #for j in range(512):
            #h = g.getHeight(i,j)
            #h = abs(h)
            #h *= 100
            #h %= max_height
            #h = int(h)
            #for k in range(baseline, h+baseline):
                ## do 3d noise density
                #solid = g.getDensity(i,j,k)
                #if solid >= 0:
                    #terrain_map.set(i, j, k, 2)
            #if h<=1:  # lava (this will cover up holes in low surface caves)
                #terrain_map.set(i,j, baseline+1, 211)
                #if h == 0:
                    #terrain_map.set(i,j, baseline, 211)

    print 'done map gen'
    print 'took %d seconds' % (time.time() - _n)


# cave cube + mountain top
def load_map5(terrain_map):
    import time
    _n = time.time()
    print 'start map gen'
    xd = 128
    yd = 128
    zd = 128
    max_height = 15
    baseline = zd - max_height
    g = Gen(salt=random.random(), xint=xd, yint=yd, zint=zd)
    h=r=0

    xdr = range(xd)
    ydr = range(yd)
    zdr = range(zd)

    print 'generating swiss cube'
    # fill in baseline cube with density tubes
    for i in xdr:
        for j in ydr:
            for k in zdr:
                solid = g.getDensity(i,j,k, persistence=0.8, octaves=8)
                if solid >= -0.1:
                    terrain_map.set(i,j,k, 2)

    print 'generating mountains + lava'
    # do height subtractively
    for i in xdr:
        for j in ydr:
            h = g.getHeight(i,j)
            h = abs(h)
            h *= 100
            h %= max_height
            h = int(h)
            r = zd - (max_height - h)
            for k in range(r, zd):
                terrain_map.set(i, j, k, 0) #subtract
            if h<=1:  # lava
                # find highest open point, make lava
                k = terrain_map.get_highest_open_block(i,j)
                terrain_map.set(i,j, k, 3)

    print 'done map gen'
    print 'took %d seconds' % (time.time() - _n)
