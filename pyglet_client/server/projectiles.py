'''
Projectiles
'''

'''
Projectile data (will be moved to configurable external format)
'''
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


'''
Projectile Controller
'''
from game_state import GenericObjectList
from game_state import GameStateGlobal

class ProjectileList(GenericObjectList):

    def __init__(self):
        from projectiles import Projectile
        GenericObjectList.__init__(self)
        self._metaname = 'ProjectileList'
        self._itemname = 'Projectile'
        self._object_type = Projectile

    def create(self, *args, **projectile):
        projectile = self._add(*args, **projectile)
        return projectile

    def destroy(self, projectile):
        self._remove(projectile)
        return projectile


'''
Projectile class
'''
from math import sin, cos, pi
from game_objects import GameObject
from cube_dat import CubeGlobals

class Projectile(GameObject):

    def __init__(self, state=None, type=None): #more args
        if None in (state, type,):
            print 'Projectile __init__ missing args'
            raise TypeError

        global projectile_dat
        assert projectile_dat.has_key(type)
        p = projectile_dat[type]
        #load projectile settings
        
        self.state = map(lambda k: float(k), state)
        x, y, z, vx, vy, vz = state

        self.id = GameStateGlobal.new_projectile_id()
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
            pass
        except AssertionError:
            print 'projectile update :: state is wrong length'
            return

    #run this once per frame for each projectile
    def tick(self):
        x,y,z,vx,vy,vz = self.state

        fps = 60. # frame per second
        speed = self.speed / fps

        self.ttl += 1
        if self.ttl > self.ttl_max:
            self.delete()
            return

        x += vx / fps
        y += vy / fps
        z += vz / fps

        if CubeGlobals.collisionDetection.collision(x, y, z):
            agent_hit = GameStateGlobal.agentList.at(*pos)
            if agent_hit != False:
                print 'DIEEEE'
                print agent_hit
                agent_hit.take_damage(self.damage)
            self.delete()
            return

        self.state = [x,y,z,vx,vy,vz]


    def delete(self):
        GameStateGlobal.projectileList.destroy(self)

    def json(self, properties=None): # json encodable string representation
        d = GameObject.json(self)
        if properties is None:
            d.update({
                'state' : self.state,
                'type'  : self.type,
            })
        else:
            for prop in properties:
                d[prop] = getattr(self, prop)
        return d
