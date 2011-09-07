#!/usr/bin/python

'''
Map gen
'''

import noise
import random
class Gen:

    '''
    Adapted from:
    http://stackoverflow.com/questions/4753055/perlin-noise-generation-for-terrain
    '''

    def __init__(self, o=10, p=0.5, f=1.3, a=1, seed=0.9, xint=128, yint=128, salt=0):
        self.octaves = o
        self.num_octaves = range(o)
        self.persistence = p
        self.frequency = f
        self.amplitude = a
        self.seed = 2 + seed**2
        self.xint = xint
        self.yint = yint
        self.salt = salt

    def getHeight(self, x, y):
        #return self.amplitude * self.total(x,y)
        x /= float(self.xint)
        y /= float(self.yint)
        return self.amplitude * self.noise(x, y)

    def noise(self, x, y):
        n = noise.pnoise2(x+self.salt, y+self.salt, octaves=self.octaves, persistence=self.persistence)
        return n
        
