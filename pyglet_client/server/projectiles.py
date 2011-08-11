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

        self.speed = p['speed']
        self.damage = p['damage']
        self.ttl = 0
        self.ttl_max = p['ttl_max']
        self.penetrates = p.get('penetrates', False)
        self.suicidal = p.get('suicidal', False)

        self.owner = owner

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
            pass
        except AssertionError:
            print 'projectile update :: state is wrong length'
            return

    #run this once per frame for each projectile
    def tick(self):
        x,y,z,vx,vy,vz = self.state

        fps = 30. # frame per second
        speed = self.speed / fps

        self.ttl += 1
        if self.ttl > self.ttl_max:
            self.delete()
            return

        x += vx / fps
        y += vy / fps
        z += vz / fps

        if collisionDetection(int(x), int(y), int(z)):
            print "collision with wall"
            self.delete()
            return

        #slow way, will be bottle neck later
#        for agent in GameStateGlobal.agentList.values():
#            if agent.point_collision_test(x,y,z):
#                print "projectile collision"
#                agent.take_damage(self.damage)

        #faster way; needs to choose a large radius and only update every n-frames
        agent_list = GameStateGlobal.agentList.agents_near_point(x, y, z, 4.0)
        for agent in agent_list:
            if agent.point_collision_test(x,y,z):
                if not self.suicidal and agent.owner == self.owner: # bullet is hitting yourself, and bullet doesnt kill yourself
                    continue
                print "projectile collision"
                agent.take_damage(self.damage, self.owner)
                if not self.penetrates:
                    self.delete()
                    return
        #agent_hit = GameStateGlobal.agentList.at((x, y, z,))
        #if agent_hit != False:
        #    agent_hit.take_damage(self.damage)

        self.state = [x,y,z,vx,vy,vz]

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


class Grenade(Projectile):

    def __init__(self, id, state, owner=None):
        Projectile.__init__(self, id, state, owner)
