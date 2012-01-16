
from __future__ import division
import math
import time

import sys
import os
sys.path.insert(0, './ext/')

import _Noise as N
from wave import *
import random

import scipy, pylab

def stft(x, fs, framesz, hop):
    framesamp = int(framesz*fs)
    hopsamp = int(hop*fs)
    w = scipy.hamming(framesamp)
    X = scipy.array([scipy.fft(w*x[i:i+framesamp])
                     for i in range(0, len(x)-framesamp, hopsamp)])
    return X

def istft(X, fs, T, hop):
    x = scipy.zeros(T*fs)
    framesamp = X.shape[1]
    hopsamp = int(hop*fs)
    for n,i in enumerate(range(0, len(x)-framesamp, hopsamp)):
        x[i:i+framesamp] += scipy.real(scipy.ifft(X[n]))
    return x

'''
if __name__ == '__main__':
    #f0 = 440         # Compute the STFT of a 440 Hz sinusoid
    fs = 8000        # sampled at 8 kHz
    T = 5            # lasting 5 seconds
    #framesz = 0.050  # with a frame size of 50 milliseconds
    hop = 0.020      # and hop size of 20 milliseconds.
'''

def new_wav(name= "test.wav"):
    W = open(name, 'wb')
    W.setnchannels(1)
    W.setsampwidth(1)
    W.setframerate(48000)
    W.setnframes(0)
    return W

def gen_perlin(timestep= 0.8, offset=0.8):
    li = []
    x,y = 0.5,0.5
    for i in range(0,48000):
        z = i *float(timestep)
        l = N._p3(x,y,z) + offset
        if l < 0:
            l=0
        if l > 1:
            l=1
        li.append(int(l*255))
    b = bytearray(li)
    return b

def gen_voronoi(timestep= 0.8, offset=0.8):
    li = []
    x,y = 0.5,0.5
    print "Generate noise values"
    for i in range(0,1*48000):
        z = i *float(timestep)
        l = N._Vo(x,y,z, 0, 0) + offset
        if l < 0:
            l=0
        if l > 1:
            l=1
        li.append(int(l*255))
    '''
    print "stft start"
    fs = 8000
    T = 1
    hop = 0.020
    li = stft(li, fs, T, hop)
    print str(li)
    '''
    b = bytearray(li)
    return b

W = new_wav(name= "test.wav")
#W.writeframes(gen_perlin(timestep= 0.0001, offset=0.0))
W.writeframes(gen_voronoi(timestep= 0.0050, offset=0.0))
