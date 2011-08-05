'''
Animations
'''
import draw_utils
import vector_lib
from math import ceil


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
        self.life = 0

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
        self.color = (30, 255, 100)

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
        self.loc = vector_lib.move_point(self.loc, self.vector, self.speed)
        #self.end = vector_lib.move_point(self.loc, self.vector, self.length)
        self.life -= 1
        self.length = min(self.life, 1000)
        if self.length < 1000:
            self.length -= 1
        self.length = max(self.length, 0)
        #print 'animation life: %i' % self.life

    def draw(self):
        # draw a line of some length & color
        draw_utils.draw_ray(self.loc, self.vector, self.length, self.color)
        #draw_utils.draw_line(self.loc, self.end, self.color)

