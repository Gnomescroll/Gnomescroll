from math import sin, cos, pi

projectile_dat = {

    0   :   {   # generic projectile
        'speed' :   0,
        'damage':   0,
        'ttl_max':  0,
    },

    1 : {   # laser
        'speed' : 110,
        'damage' : 20,
        'ttl_max' : 400, #time to live in ticks
        'penetrates': False,
        'suicidal'  : False, # True for grenades
    #    'splash' : {
    #    'radius' : 3,
    #    'damage' : 15,
    #    'force' : 150,
    },

    2 : {   #   grenade
        'speed' : 30,
        'damage' : 50,
        'ttl_max' : 300,
        'suicidal': True
    },

}


class Projectile:

    projectile_types = {
        'Projectile'    :   0,
        'Laser'         :   1,
        'Grenade'       :   2,
    }

    @classmethod
    def name_from_type(cls, type):
        if not hasattr(cls, 'projectile_names'):
            rev = [(b,a) for a,b in cls.projectile_types.items()]
            cls.projectile_names = dict(rev)
        return cls.projectile_names[type]

    def __init__(self, id, state=None, owner=None, *args, **kwargs): #more args
        self.id = id
        self._set_type()
        p = projectile_dat[self.type]

        self.state = map(float, state)
        self.owner = owner

        self.speed = p['speed'] / GameStateGlobal.fps
        self.damage = p['damage']
        self.ttl = 0
        self.ttl_max = p['ttl_max']
        self.penetrates = p.get('penetrates', False)
        self.suicidal = p.get('suicidal', False)

    def _set_type(self):
        self.type = self.projectile_types[self.__class__.__name__]

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
        if 'owner' in args:
            self.owner = args['owner']

    def delete(self):
        GameStateGlobal.projectileList.destroy(self)

    def check_life(self):
        self.ttl += 1
        if self.ttl > self.ttl_max:
            self.delete()
            return False
        return True

    def pos(self):
        return self.state[0:3]

    #run this once per frame for each projectile
    def tick(self):
        return

##deprecate

import default_settings as settings

if settings.pyglet:
    import pyglet
    from pyglet.gl import *
else:
    import SDL.gl

def draw_projectiles():
    #if settings.pyglet == False:
    #    SDL_global = SDL.gl.SDL_global

    v_num = 0
    v_list = []
    c_list = []

    l= 1
#    for p in GameStateGlobal.projectileList:
#        print str(p)
    for p in GameStateGlobal.projectileList.values():
        x,y,z,vx,vy,vz = p.state
        SDL.gl.draw_particle(5, 0.5, x,y,z)


class Laser(Projectile):

    def __init__(self, id, state=None, owner=None, *args, **kwargs):
        Projectile.__init__(self, id, state=state, owner=owner, *args, **kwargs)

    def tick(self):
        if not self.check_life():
            return

        [x,y,z,vx,vy,vz] = self.state

        x += vx * self.speed
        y += vy * self.speed
        z += vz * self.speed

        #if CubeGlobal.collisionDetection.collision(int(x), int(y), int(z)):
        if collisionDetection(int(x), int(y), int(z)):
            self.delete()

        self.state = [x,y,z,vx,vy,vz]

    def draw(self):
        x,y,z = self.pos()
        SDL.gl.draw_particle(5, 0.5, x,y,z)


class Grenade(Projectile):

    def __init__(self, id, state=None, owner=None, ttl=0, *args, **kwargs):
        Projectile.__init__(self, id, state=state, owner=owner, *args, **kwargs)
        self.ttl = ttl

    def tick(self):
        if not self.check_life():
            return

        x,y,z, vx, vy, vz = self.state

        # move grenade along trajectory here
        x += vx * self.speed
        y += vy * self.speed
        z += vz * self.speed

        if collisionDetection(*map(int, [x,y,z])):
            #bounce
            pass

        self.state = [x,y,z,vx,vy,vz]

    def draw(self):
        x,y,z = self.pos()
        SDL.gl.draw_particle(5, 0.5, x,y,z)

from game_state import GameStateGlobal
from cube_lib.terrain_map import collisionDetection
import draw_utils
