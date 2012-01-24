'''
Animations
'''

import draw_utils
import vector_lib
import random
from math import ceil

import c_lib.c_lib_particles as cParticles

class Animations:

    def __init__(self):
        self.animations = []

    def draw(self):
        dead = []
        died = False
        for animation in self.animations:
            animation.draw()

    def tick(self):
        dead = []
        died = False
        for animation in self.animations:
            if animation.life <= 0:
                animation.die()
                dead.append(animation)
                died = True
                continue
            animation.next()
        if died:
            self.animations = [a for a in self.animations if a not in dead]

    def add(self, animation):
        self.animations.append(animation)

animations = Animations()

class Animation:

    def __init__(self):
        self.life = 1

    def next(self):
        pass

    def draw(self):
        pass

    def play(self):
        animations.add(self)

    def stop(self):
        self.life = 0

    def die(self):  # teardown
        pass


class HitscanLaserGunAnimation(Animation):

    def __init__(self, origin, vector, target=None):
        print vector
        self.length = 15
        self.png_id = 18
        self.radius = 0.5

        self.origin = origin
        self.vector = vector

        self.life = 50
        self.length = self.life

    def next(self):
        self.life -= 1

    def draw(self):
        fade = 1
        draw_utils.draw_laser_ray(self.origin, self.vector, self.length, self.radius, fade, self.png_id)

    def die(self):
        pass

class C_Animation(Animation):

    anim = lambda x,y,z, vx,vy,vz: None

    def __init__(self):
        Animation.__init__(self)

    def create_particles(self):
        pass

class GrenadeExplodeAnimation(C_Animation):

    anim = cParticles._create_shrapnel
    vel = [20]*3

    def __init__(self, pos):
        C_Animation.__init__(self)
        self.n = random.randrange(8,13)
        self.n_range = range(self.n)
        self.pos = pos
        self.create_particles()

    def create_particles(self):
        _vx, _vy, _vz = self.vel
        for i in self.n_range:
            x,y,z = [i + ((random.random()-0.5) / 20) for i in self.pos]
            vx = _vx*(random.random() -0.5)
            vy = _vy*(random.random() -0.5)
            vz = _vz*(random.random() -0.5)
            self.anim(x,y,z, vx, vy, vz)

import c_lib.c_lib_animations

class BlockCrumbleAnimation(C_Animation):
    anim = c_lib.c_lib_animations.block_crumbles

    def __init__(self, pos):
        C_Animation.__init__(self)
        self.pos = pos
        self.anim(self.pos, random.randrange(10,30))

class FloatTextAnimation(C_Animation):
    anim = cParticles._create_billboard_text

    def __init__(self, pos, text):
        C_Animation.__init__(self)
        self.pos = pos
        self.text = str(text)
        self.vel = [0.,0.,7.5] # straight up
        self.create_particles()

    def create_particles(self):
        x,y,z = self.pos
        vx,vy,vz = self.vel
        self.anim(x,y,z, vx,vy,vz, self.text)
