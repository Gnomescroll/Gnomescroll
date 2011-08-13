from math import sin, cos, pi

import default_settings as settings

if settings.pyglet:
    import pyglet
    from pyglet.gl import *
else:
    import SDL.gl


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

    def pos(self):
        return self.state[0:3]

    #run this once per frame for each projectile
    def tick(self):
        return

    def check_life(self):
        self.ttl += 1
        if self.ttl > self.ttl_max:
            self.delete()
            return False
        return True

    ''' This was copied from the server. The proximity check may be too much cpu. '''
    def check_agent_collision(self):
        x,y,z = self.pos()
        agent_list = GameStateGlobal.agentList.agents_near_point(x, y, z, 4.0)
        for agent in agent_list:
            if agent.point_collision_test(x,y,z):
                if not self.suicidal and agent.owner == self.owner: # bullet is hitting yourself, and bullet doesnt kill yourself
                    continue
                print "projectile collision"
                return agent
        return False

    #def hit_agent(self, agent):
        #agent.take_damage(self.damage, self.owner)
        #if not self.penetrates:
            #self.delete()
            #return True
        #return False

    def check_terrain_collision(self, delete=True):
        if collisionDetection(*map(int, self.pos())):
            print "collision with wall"
            if delete:
                self.delete()
            return True
        return False


class Laser(Projectile):

    def __init__(self, id, state=None, owner=None, *args, **kwargs):
        Projectile.__init__(self, id, state=state, owner=owner, *args, **kwargs)

    def tick(self):
        if not self.check_life():
            return

        x,y,z,vx,vy,vz = self.state

        x += vx * self.speed
        y += vy * self.speed
        z += vz * self.speed

        self.state = [x,y,z,vx,vy,vz]

        self.check_terrain_collision() or self.check_agent_collision()

    def draw(self):
        x,y,z = self.pos()
        SDL.gl.draw_particle(5, 0.5, x,y,z)


class Grenade(Projectile):

    def __init__(self, id, state=None, owner=None, ttl=0, *args, **kwargs):
        Projectile.__init__(self, id, state=state, owner=owner, *args, **kwargs)
        self.ttl = ttl

    def tick(self):
        if not self.check_life():
            # explode
            print 'boom'
            return

        if self.check_terrain_collision(delete=False):
            #bounce
            self.state[3:] = 0,0,0
            return


        x,y,z, vx, vy, vz = self.state
        z_gravity = -.03

        # move grenade along trajectory here
        x += vx * self.speed
        y += vy * self.speed
        z += (vz + (z_gravity * self.ttl)) * self.speed

        self.state = [x,y,z,vx,vy,vz]


        if self.check_agent_collision():
            #fall
            self.state[5] = 0
            return

    def draw(self):
        x,y,z = self.pos()
        SDL.gl.draw_particle(5, 0.5, x,y,z)

from game_state import GameStateGlobal
from cube_lib.terrain_map import collisionDetection
#import draw_utils
