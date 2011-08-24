#!/usr/bin/python

'''
Map gen
'''

import noise
from random import random
class Gen:

    '''
    Adapted from:
    http://stackoverflow.com/questions/4753055/perlin-noise-generation-for-terrain
    '''

    def __init__(self, o=10, p=0.9, f=1.3, a=1, seed=0.9, xint=128, yint=128):
        self.octaves = o
        self.num_octaves = range(o)
        self.persistence = p
        self.frequency = f
        self.amplitude = a
        self.seed = 2 + seed**2
        self.xint = xint
        self.yint = yint

    def getHeight(self, x, y):
        return self.amplitude * self.total(x,y)

    def total(self, i, j):
        t = 0.
        amp = 1
        f = self.frequency
        xi = float(self.xint)
        yi = float(self.yint)
        

        for i in self.num_octaves:
            xv = (i/xi)*f + self.seed
            yv = (j/yi)*f + self.seed
            t += self.getValue(xv, yv) * amp
            amp *= self.persistence
            f *= 2
        if self._y % 128 == 10:
            #print i, j
            #print xv, yv
            pass
        self._y += 1
        return t

    def getValue(self, x, y):
        ix = int(x)
        iy = int(y)
        rx = x - ix
        ry = y - iy

        n1 = self.noise(ix-1, iy-1)
        n2 = self.noise(ix+1, iy-1)
        n3 = self.noise(ix-1, iy+1)
        n4 = self.noise(ix+1, iy+1)
        n5 = self.noise(ix-1, iy)
        n6 = self.noise(ix+1, iy)
        n7 = self.noise(ix, iy-1)
        n8 = self.noise(ix, iy+1)
        n9 = self.noise(ix, iy)
        
        n12 = self.noise(ix+2, iy-1)
        n14 = self.noise(ix+2, iy+1)
        n16 = self.noise(ix+2, iy)

        n23 = self.noise(ix-1, iy+2)
        n24 = self.noise(ix+1, iy+2)
        n28 = self.noise(ix, iy+2)

        n34 = self.noise(ix+2, iy+2)

        x0y0 = 0.0625*(n1+n2+n3+n4) + 0.125*(n5+n6+n7+n8) + 0.25*n9
        x1y0 = 0.0625*(n7+n12+n8+n14) + 0.125*(n9+n16+n2+n4) + 0.25*n6
        x0y1 = 0.0625*(n5+n6+n23+n24) + 0.125*(n3+n4+n9+n28) + 0.25*n8
        x1y1 = 0.0625*(n9+n16+n28+n34) + 0.125*(n8+n14+n6+n24) + 0.25*n4

        #if self._y % 128 == 10:
            ##print x0y0, x1y0, x0y1, x1y1
            ##print ix, iy
            #print x, y
        #self._y += 1

        v1 = self.interpolate(x0y0, x1y0, rx)
        v2 = self.interpolate(x0y1, x1y1, rx)
        fin = self.interpolate(v1, v2, ry)
        return fin

    _last_x = 0
    _y = 0
    _switch = True
    def interpolate(self, x, y, a, fin=False):
        neg_a = 1.0 - a
        neg_a_sq = neg_a ** 2
        fac1 = 3.0 * (neg_a_sq) - 2.0 * (neg_a_sq * neg_a)
        a_sq = a**2
        fac2 = 3.0 * a_sq - 2.0 * (a_sq * a)
        if fin:
            if self._switch:
                self._switch = False
                print x, fac1, y, fac2
            self._y += 1
            lx = self._last_x
            self._last_x = self._y // 128
            if lx < self._last_x:
                self._switch = True
        return x*fac1 + y*fac2

    def noise(self, x, y):
        n = x + y*57
        n = (n << 13) ^ n
        t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff
        return 1.0 - t * 0.931322574615478515625e-9
