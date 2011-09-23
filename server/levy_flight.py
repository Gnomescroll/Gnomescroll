#!/usr/bin/python
#Levy flight simulator

from math import sin, cos, pi, log
import random

class Transforms:
    
    def __init__(self):
        pass

    def polar2cart(self, r, theta):
        x = r * cos(theta)
        y = r * sin(theta)
        return x,y

    def sphere2cart(self, r, theta, pho):
        x = r * sin(theta) * cos(pho)
        y = r * sin(theta) * sin(pho)
        z = r * cos(theta)
        return x,y,z
        
    def powerlaw(self, x, amp, index):
        i = amp * (x**index)
        return i

    def coord2pixel(self, coord, x_res, y_res, x_scale=1, y_scale=1):
        x, y = coord[0], coord[1]
        #x, y = int(round(x)), int(round(y))
        x_scale, y_scale = 1,1
        x += ( x_res*x_scale ) / 2.
        y += ( y_res*y_scale ) / 2.
        x, y = int(round(x)), int(round(y))
        return (x, y)
        
    def coord2pixel3d(self, coord, x_res, y_res, z_res):
        # assumes coordinates are normalized and in range [-1,1]
        scale = map(float, [x_res, y_res, z_res])
        coord = [n+1 for n in coord]
        coord = [((n*s)+s)/2. for n,s in zip(coord, scale)]
        coord = map(int, map(round, coord))
        return coord
        
    def log_coord2pixel(self, coord, x_res, y_res, x_scale, y_scale):
        x, y = coord[0], coord[1]
        
        x = 0 if x==0 else (round(x/abs(x)))*log(abs(x))
        y = 0 if y==0 else (round(y/abs(y)))*log(abs(y))
        
        x, y = int(round(x)), int(round(y))
        x += x_res/2
        y += y_res/2
        return (x,y)
        
    def log_coord2pixel3d(self, coord, x_res, y_res, z_res):
        # assumes coordinates are normalized and in range [-1,1]
        scale = map(float, [x_res, y_res, z_res])

        x,y,z = coord
        sx,sy,sz = [(n<0) for n in coord]
        x = 0 if x==0 else (round(x/abs(x)))*log(abs(x))
        y = 0 if y==0 else (round(y/abs(y)))*log(abs(y))
        z = 0 if z==0 else (round(z/abs(z)))*log(abs(z))

        x = x*-1 if sx else x
        y = y*-1 if sy else y
        z = z*-1 if sz else z
        
        x, y, z = map(int, map(round, [x,y,z]))

        x += x_res/2
        y += y_res/2
        z += z_res/2
        
        return x,y,z
        
    def scalevector(self, vector, scale):
        vector = [i*scale for i in vector]
        return vector


class Distributions:
    
    def __init__(self):
        pass
        
    def powerlaw(self, x, amp, index):
        i = amp * (x**index)
        return i


class LevyFlight:
    
    def __init__(self, origin=None, iters=1000):
        if origin is None:
            origin = [0,0,0]
            
        self.pos = origin
        self.iters = xrange(iters)
        #self.max_iters = iters

        self.max = origin[:]
        self.min = origin[:]
        self.bounds = origin[:]
        
        self.transforms = Transforms()
        self.dist = Distributions()

    def generate(self):
        self.all = []
        for i in self.iters:
            self.fly()
            self.all.append(self.pos)
        self.set_range_final()

        self.all = [[n-m for m,n in zip(self.min, pos)] for pos in self.all] # affine shift planes to positive
        self.all = [[n/m for m,n in zip(self.bounds, pos)] for pos in self.all]  # normalize
        #print self.all
        #print self.min
        #print self.max
        #print self.bounds
        #assert False
        #for a in self.all:
            #print a
        self.all = [self.transforms.coord2pixel3d(p, 128,128,128) for p in self.all]
        self._all = []
        [self._all.append(p) for p in self.all if p not in self._all]
        self.all = self._all
        #for a in self.all:
            #print a
        print len(self.all)
        return self.all

    def set_max_pos(self, pos):
        for i,n in enumerate(pos):
            if n > self.max[i]:
                self.max[i] = n

    def set_min_pos(self, pos):
        for i,n in enumerate(pos):
            if n < self.min[i]:
                self.min[i] = n

    def set_range(self):
        self.set_max_pos(self.pos)
        self.set_min_pos(self.pos)

    def set_range_final(self):
        self.bounds = map(lambda a: abs(a[0])+abs(a[1]), zip(self.max, self.min))

    #def __iter__(self):
        #for i in self.iters:
            #self.fly()
            #n = self.transforms.coord2pixel3d(self.pos, 128,128,128)
            #yield n
        

    def fly(self):
        theta = random.uniform(0, 2*pi) #uniform distribution of theta
        pho = random.uniform(0, 2*pi)
        r = self.dist.powerlaw(random.random(), 0.1, -1.0001) #power law distributed r
        #x,y = self.transforms.polar2cart(r, theta) #get x,y vector
        x,y,z = self.transforms.sphere2cart(r, theta, pho)

        #(x, y) = self.transforms.scalevector((x,y), 5)
        self.pos[0] += x #add vector to position
        self.pos[1] += y
        self.pos[2] += z
        self.set_range()
    
        
#levy = LevyFlight(iters=1000)
#levy.fly()
