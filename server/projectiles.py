'''
Projectiles
'''

'''
Projectile data (will be moved to configurable external format)
'''

import c_lib.terrain_map as terrain_map
import c_lib.c_lib_particles as c_obj
import dats.loader as dat_loader
p_dat = dat_loader.p_dat

from math import sin, cos, pi, sqrt
from c_lib.terrain_map import collisionDetection
from vector_lib import distance
from net_out import NetOut
from object_lists import GenericMultiObjectList
from game_state import GameStateGlobal
from utils import filter_props


'''
Projectile Controller
'''
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

        self.g_index = c_obj._create_grenade(x,y,z, vx,vy,vz, ttl)

    def pos(self):
        return
        #return c_obj.get_grenade_position(self.g_index)

    def tick(self):
        pass
        #if not self.check_life(delete=False):
            #self.explode()
            #self.delete()

    def explode(self):
        pass
        #pos = self.pos()
        
        ## calculate distance for all damageable objects
        ## if <self.blast_radius, apply self.blast_damage
        #[agent.take_damage(self.splash_damage, self.owner, self.suicidal) \
         #for agent in GameStateGlobal.agentList.values() \
         #if distance(pos, agent.pos()) < self.splash_radius]

        ## destroy nearby blocks
        ## look in 8 adjacent spots around grenade (16 maybe)
        ## n distance away (probably n<1, maybe n=0.7

        #grenade_block_destroy_distance = 0.6    # make this a parameter later
        #gn = grenade_block_destroy_distance
        #diag_len = gn / sqrt(2) # the distance along 2 axes to form diagonal affine shift of len = gn

        ##print "Grenade destroying blocks around point ", pos
        #blocks = [
            ## x
            #map(int, [pos[0] + gn, pos[1], pos[2]]),
            #map(int, [pos[0] - gn, pos[1], pos[2]]),
            ## y
            #map(int, [pos[0], pos[1] + gn, pos[2]]),
            #map(int, [pos[0], pos[1] - gn, pos[2]]),
            ## z
            #map(int, [pos[0], pos[1], pos[2] + gn]),
            #map(int, [pos[0], pos[1], pos[2] - gn]),

            ## xy plane
            #map(int, [pos[0] + diag_len, pos[1] + diag_len, pos[2]]),
            #map(int, [pos[0] - diag_len, pos[1] + diag_len, pos[2]]),
            #map(int, [pos[0] + diag_len, pos[1] - diag_len, pos[2]]),
            #map(int, [pos[0] - diag_len, pos[1] - diag_len, pos[2]]),
            ## xz plane
            #map(int, [pos[0] + diag_len, pos[1], pos[2] + diag_len]),
            #map(int, [pos[0] - diag_len, pos[1], pos[2] + diag_len]),
            #map(int, [pos[0] + diag_len, pos[1], pos[2] - diag_len]),
            #map(int, [pos[0] - diag_len, pos[1], pos[2] - diag_len]),
        #]

        #changes = []
        #for b in blocks:
            #x,y,z = b
            #changed = terrain_map.set_notify(x,y,z, 0)
            #if changed:
                #changes.append([x,y,z,0])

        #NetOut.event.set_map(changes)
            
    def delete(self):
        pass
        #Projectile.delete(self)
        #c_obj._destroy_grenade(self.g_index)

Grenade.init()


def _dat_reload(dat):
    Projectile.dat.apply(Projectile)
    Projectile.dat.apply(Laser)
    Projectile.dat.apply(Grenade)
    
Projectile.dat.on_reload = _dat_reload
