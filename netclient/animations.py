'''
Animations
'''
import draw_utils
import vector_lib
import random
from math import ceil

import c_lib.c_lib_objects

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
            if not animation.life:
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


class HitscanLaserGunAnimation(Animation):

    def __init__(self, origin, vector, target=None):
        self.length = 15
        self.speed = 2 # per tick!
        #self.color = (30, 255, 100)
        self.png_id = 18
        self.radius = 0.5

        self.origin = origin
        self.loc = self.origin
        self.vector = vector
        #self.end = vector_lib.move_point(self.loc, self.vector, self.length)

        if target is None: # "go forever" somewhere really far
            life_ticks = 10000
        else:
            life_ticks = ceil(vector_lib.distance(self.origin, target) / self.speed)

        self.life = life_ticks
        self.length = self.life

    def next(self):
        # move self.loc in direction of vector  by magnitude speed
        #self.loc = vector_lib.move_point(self.loc, self.vector, self.speed)
        #self.end = vector_lib.move_point(self.loc, self.vector, self.length)
        self.life -= 1
        self.length = min(self.life, 1000)
        if self.length < 1000:
            self.length -= 1
        self.length = max(self.length, 0)
        #print 'animation life: %i' % self.life

    def draw(self):
        # draw a line of some length & color
        fade = 1
        draw_utils.draw_laser_ray(self.loc, self.vector, self.length, self.radius, fade, self.png_id)
        #draw_utils.draw_ray(self.loc, self.vector, self.length, self.color)
        #draw_utils.draw_line(self.loc, self.end, self.color)


class C_Animation(Animation):

    anim = lambda x,y,z, vx,vy,vz: None

    def __init__(self):
        Animation.__init__(self)

    def create_particles(self):
        pass

class GrenadeExplodeAnimation(C_Animation):

    anim = c_lib.c_lib_objects._create_shrapnel
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


class BlockCrumbleAnimation(C_Animation):
    #anim = c_lib.c_lib_objects._create_minivox
    anim = c_lib.c_lib_objects._create_minivox_colored
    minivox_size = 0.1

    def __init__(self, pos):
        C_Animation.__init__(self)
        self.interval = int(1.0 / self.minivox_size) - 1
        self.n = self.interval ** 3
        self.n_range = xrange(self.n)
        self.pos = pos
        self.create_particles()

    def create_particles(self):
        _vx,_vy,_vz = 2.,2.,2.
        x,y,z = 0,0,0
        for i in self.n_range:
            x += 1
            x %= self.interval
            if not x:
                y += 1
                y %= self.interval
                if not y:
                    z += 1
                    z %= self.interval
            nx,ny,nz = [n*self.minivox_size + self.pos[i] for i,n in enumerate([x,y,z])]
            nx,ny,nz = [n + ((random.random()-0.5) * 0.05) for n in [nx,ny,nz]]

            vx = _vx*(random.random() -0.5)
            vy = _vy*(random.random() -0.5)
            vz = _vz*(random.random() -0.5)
            self.anim(nx,ny,nz, vx,vy,vz, random.randint(0,256), random.randint(0,256), random.randint(0,256))
