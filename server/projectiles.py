'''
Projectiles
'''

'''
Projectile data (will be moved to configurable external format)
'''

from c_lib.terrain_map import collisionDetection
from vector_lib import distance
import c_lib.c_lib_objects as c_obj

import dats.loader as dat_loader
p_dat = dat_loader.p_dat

'''
Projectile Controller
'''
from object_lists import GenericMultiObjectList
from game_state import GameStateGlobal

class ProjectileList(GenericMultiObjectList):

    def __init__(self):
        GenericMultiObjectList.__init__(self)
        self._allow_klasses([\
            Laser,
            Grenade,
        ])
        self._metaname = 'ProjectileList'
        self._itemname = 'Projectile'


'''
Projectile class
'''
from math import sin, cos, pi
#from game_objects import GameObject
from utils import filter_props
#from cube_dat import CubeGlobal

class Projectile:

    _types = {
        'Projectile'    :   0,
        'Laser'         :   1,
        'Grenade'       :   2,
    }

    dat = p_dat

    @classmethod
    def init(cls):
        cls.type = cls._types[cls.__name__]
        cls.dat.apply(cls)

    def __init__(self, id, state, owner=None):
        self.id = id
        self.owner = owner
        self.state = map(float, state)
            
        self.speed = self.speed / GameStateGlobal.fps
        self.ttl = 0

    def pos(self):
        return self.state[0:3]

    def tick(self):
        return

    def check_life(self, delete=True):
        self.ttl += 1
        if self.ttl > self.ttl_max:
            if delete:
                self.delete()
            return False
        return True

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

    def hit_agent(self, agent, delete=True):
        agent.take_damage(self.damage, self.owner)
        if not self.penetrates and delete:
            self.delete()
            return True
        return False

    def check_terrain_collision(self, delete=True):
        if collisionDetection(*map(int, self.pos())):
            print "collision with wall"
            if delete:
                self.delete()
            return True
        return False

    def delete(self):
        GameStateGlobal.projectileList.destroy(self)

    def json(self, properties=None): # json encodable string representation
        d = {
            'id'    :   self.id,
            'type'  :   self.type,
        }
        if properties is None:
            d.update({
                'state' : self.state,
                'owner' : self.owner,
            })
        else:
            d.update(filter_props(self, properties))
        return d

Projectile.init()

class Laser(Projectile):

    def __init__(self, id, state, owner=None):
        Projectile.__init__(self, id, state, owner)

    def tick(self):
        if not self.check_life():
            return

        x,y,z,vx,vy,vz = self.state

        x += vx * self.speed
        y += vy * self.speed
        z += vz * self.speed

        self.state = [x,y,z,vx,vy,vz]

        if not self.check_terrain_collision():
            agent = self.check_agent_collision()
            if agent:
                self.hit_agent(agent)

Laser.init()


class Grenade(Projectile):

    def __init__(self, id, state, owner=None, ttl=0, *args, **kwargs):
        self.id = id
        self.owner = owner
        #self.speed = self.speed / GameStateGlobal.fps

        self.ttl = ttl
        x,y,z, vx,vy,vz = state
        vx,vy,vz = [i*self.speed for i in [vx,vy,vz]]   # state is loaded as [x,y,z, dx,dy,dz], need to apply velocity

        self.state = x,y,z, vx,vy,vz  # this is static!

        self.g_index = c_obj._create_grenade(x,y,z, vx,vy,vz, ttl, self.ttl_max)

    def pos(self):
        return c_obj.get_grenade_position(self.g_index)

    def tick(self):
        if not self.check_life(delete=False):
            self.explode()
            self.delete()

    def explode(self):
        pos = self.pos()
        # calculate distance for all damageable objects
        # if <self.blast_radius, apply self.blast_damage
        [agent.take_damage(self.splash_damage, self.owner, self.suicidal) \
         for agent in GameStateGlobal.agentList.values() \
         if distance(pos, agent.pos()) < self.splash_radius]
        
    
    def delete(self):
        Projectile.delete(self)
        c_obj._destroy_grenade(self.g_index)

Grenade.init()


def _dat_reload(dat):
    Projectile.dat.apply(Projectile)
    Projectile.dat.apply(Laser)
    Projectile.dat.apply(Grenade)
    
Projectile.dat.on_reload = _dat_reload
