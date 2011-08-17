'''
Projectiles
'''

'''
Projectile data (will be moved to configurable external format)
'''

from cube_lib.terrain_map import collisionDetection

projectile_dat = {

    0   :   {   # generic projectile
        'speed' :   0,
        'damage':   0,
        'ttl_max':  0,
    },

    1 : {   # laser
        'speed' : 100,
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
        'speed' : 50,
        'damage' : 50,
        'ttl_max' : 300,
        'suicidal': True
    },

}


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

    projectile_types = {
        'Projectile'    :   0,
        'Laser'         :   1,
        'Grenade'       :   2,
    }

    def __init__(self, id, state, owner=None):
        self.id = id
        self.state = map(float, state)

        self._set_type()
        p = projectile_dat[self.type]

        self.speed = p['speed'] / GameStateGlobal.fps
        self.damage = p['damage']
        self.ttl = 0
        self.ttl_max = p['ttl_max']
        self.penetrates = p.get('penetrates', False)
        self.suicidal = p.get('suicidal', False)

        self.owner = owner

    def _set_type(self):
        self.type = self.projectile_types[self.__class__.__name__]

    def pos(self):
        return self.state[0:3]

    def tick(self):
        return

    def check_life(self):
        self.ttl += 1
        if self.ttl > self.ttl_max:
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


class Grenade(Projectile):

    def __init__(self, id, state, owner=None):
        Projectile.__init__(self, id, state, owner)
        self.initial_speed = [3,3,3]
        self.state[3:6] = [a*b for a,b in zip(self.state[3:6], self.initial_speed)]

    def tick(self):
        #if not self.check_life():
            ## explode
            #print 'boom'
            #return
        pass
        #x,y,z, vx, vy, vz = self.state
        #z_gravity = -.03

        ## move grenade along trajectory here
        #x += vx * self.speed
        #y += vy * self.speed
        #z += (vz + (z_gravity * self.ttl)) * self.speed

        #self.state = [x,y,z,vx,vy,vz]

        #if self.check_terrain_collision(delete=False):
            ##bounce
            #return

        #agent = self.check_agent_collision()
        #if agent:
            ##fall
            #self.state[5] = 0
            #return

