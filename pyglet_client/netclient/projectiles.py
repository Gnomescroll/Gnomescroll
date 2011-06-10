from game_objects import GameObject
from math import sin, cos, pi

projectile_dat = {

    1 : {
        'speed' : 15,
        'damage' : 20,
        'ttl_max' : 400 #time to live in ticks
    #    'splash' : {
    #    'radius' : 3,
    #    'damage' : 15,
    #    'force' : 150,
    },

    2 : {
        'speed' : 15,
        'damage' : 20,
        'ttl_max' : 400
    },

}


class Projectile(GameObject):

    def __init__(self, state=None, id=None, type=None): #more args
        if None in (state, id, type,):
            print 'Projectile __init__ missing args'
            raise TypeError

        global projectile_dat
        assert projectile_dat.has_key(type)
        p = projectile_dat[type]
        #load projectile settings


        #vx = cos( x_angle * pi) * cos( y_angle * pi)
        #vy = sin( x_angle * pi) * cos( y_angle * pi)
        #vz = sin( y_angle)
        self.state = map(lambda k: float(k), state)

        x, y, z, vx, vy, vz = state

        self.id = id
        self.type = type
        self.speed = p['speed']
        self.damage = p['damage']
        self.ttl = 0
        self.ttl_max = p['ttl_max']

    def update(self, **args):
        try:
            state = args['state']
            state = list(state)
            assert len(state) == 6
            self.state = state
        except KeyError:
            pass
        except TypeError:
            print 'projectile update :: state is not iterable'
        except AssertionError:
            print 'projectile update :: state is wrong length'

    def delete(self):
        GameStateGlobal.projectileList.destroy(self)

    #run this once per frame for each projectile
    def tick(self):
        [x,y,z,vx,vy,vz] = self.state

        fps = 60. # frame per second
        speed = self.speed / fps

        self.ttl += 1
        if self.ttl > self.ttl_max:
            self.delete()
            return

        x += vx / fps
        y += vy / fps
        z += vz / fps

        if CubeGlobal.collisionDetection.collision(int(x), int(y), int(z)):
            self.delete()

        self.state = [x,y,z,vx,vy,vz]

    def delete(self):
        GameStateGlobal.projectileList.destroy(self)

##deprecate
import pyglet
from pyglet.gl import *
## deprecate

import SDL

def draw_projectiles():
    SDL_global = SDL.SDL_global

    v_num = 0
    v_list = []
    c_list = []

    l= 1
#    for p in GameStateGlobal.projectileList:
#        print str(p)

    for p in GameStateGlobal.projectileList.values():
        x,y,z,vx,vy,vz = p.state

        v_num += 2
        v_list += [x,y,z, (x+vx)/l, (y+vy)/l, (z+vz)/l]
        c_list += [0,0,255]*2

        SDL.draw_line(0,0,255, x,y,z, (x+vx)/l, (y+vy)/l, (z+vz)/l)

    pyglet.graphics.draw(v_num, GL_LINES,
        ("v3f", v_list),
        ("c3B", c_list)
        )

from game_state import GameStateGlobal
from cube_dat import CubeGlobal
