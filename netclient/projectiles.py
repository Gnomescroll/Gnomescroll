from math import sin, cos, pi
import math
from vector_lib import distance, vector_between_points, normalize, vector_components, reflect
import default_settings as settings

import c_lib.c_lib_objects as c_obj

from animations import GrenadeExplodeAnimation

import sound.sounds as sounds

if settings.pyglet:
    import pyglet
    from pyglet.gl import *
else:
    import SDL.gl

from dat_loader import p_dat


class Projectile:

    _types = {
        'Projectile'    :   0,
        'Laser'         :   1,
        'Grenade'       :   2,
    }

    _names = None

    dat = p_dat

    @classmethod
    def name_from_type(cls, type):
        if cls._names is None:
            rev = [(b,a) for a,b in cls._types.items()]
            cls._names = dict(rev)
        return cls._names[type]

    @classmethod
    def init(cls):
        cls.type = cls._types[cls.__name__]

    def __init__(self, id, state=None, owner=None, *args, **kwargs): #more args
        self.id = id

        self.state = map(float, state)
        self.last_state = self.state
        self.owner = owner

        self.speed = self.speed / GameStateGlobal.fps
        self.ttl = 0

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
        print 'BULLET DEAD!!'
        GameStateGlobal.projectileList.destroy(self)

    def pos(self):
        return self.state[0:3]

    def last_pos(self):
        return self.last_state[0:3]

    def velocity(self):
        return self.state[3:6]

    #run this once per frame for each projectile
    def tick(self):
        return

    def check_life(self, delete=True):
        self.ttl += 1
        if self.ttl > self.ttl_max:
            if delete:
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

    def check_terrain_collision(self, delete=True, pos=None):
        if pos is None:
            pos = self.pos()
        if collisionDetection(*map(int, pos)):
            #print "collision with wall"
            if delete:
                self.delete()
            return True
        return False

    def draw(self):
        return

    def update_sound(self):
        return

Projectile.init()


class Laser(Projectile):

    def __init__(self, id, state=None, owner=None, *args, **kwargs):
        Projectile.__init__(self, id, state=state, owner=owner, *args, **kwargs)
        self.sample_rate = 10.
        self.sample_range = range(int(self.sample_rate))
        self.sample_delta = 0.10
        self.snd_id  = sounds.play_3d('warp2.wav', self.pos(), self.velocity())

    def tick(self):
        if not self.check_life():
            return

        self.last_state = self.state
        x,y,z,vx,vy,vz = self.state

        x += vx * self.speed
        y += vy * self.speed
        z += vz * self.speed

        self.state = [x,y,z,vx,vy,vz]

        # Skip block protection:
        # Calculate vector between last pos and curr pos
        # Divide by sample rate (alternatively, could use fixed delta)
        # Iteratively move last pos by divided vector
        # If still at last block, continue
        # If at curr block, break
        # Else, found intermediate block, add to list of blocks passed thru

        # Use this ordered list of blocks passed thru to check collision

        pos = self.pos()
        last_pos = self.last_pos()
        last_block = map(int, last_pos)
        if self.check_terrain_collision(pos=last_block):
            return

        curr_block = map(int, pos)
        blocks = [last_block]
        if curr_block != last_block:
            vec = vector_between_points(last_pos, pos)
            nor_vec = normalize(vec)
            dx, dy, dz = [a*self.sample_delta for a in nor_vec]
            x,y,z = last_pos

            d = distance(pos, last_pos)
            pts = math.ceil(d/self.sample_delta)
            r = range(int(pts))
            for i in r:
                x += dx
                y += dy
                z += dz
                mid_block = map(int, [x,y,z])
                if mid_block == last_block:
                    continue
                elif mid_block == curr_block:
                    break
                elif mid_block != blocks[-1]:
                    blocks.append(mid_block)
                    if self.check_terrain_collision(pos=mid_block):
                        return

        if self.check_terrain_collision(pos=curr_block):
            return

        self.check_agent_collision()

    def draw(self):
        x,y,z = self.pos()
        SDL.gl.draw_particle(5, 0.5, x,y,z) #was 5

    def update_sound(self):
        sounds.update_3d(self.snd_id, self.pos(), self.velocity())

Laser.init()


class Grenade(Projectile):

    def __init__(self, id, state=None, owner=None, ttl=0, *args, **kwargs):
        self.id = id
        self.owner = owner
        self.speed = self.speed / GameStateGlobal.fps
        self.ttl = ttl
        x,y,z, vx,vy,vz = state
        #self.g_index = c_obj._create_grenade(x,y,z, vx,vy,vz, ttl, self.ttl_max)
        self.g_index = c_obj._create_grenade(x,y,z, vx,vy,vz, ttl)

    def pos(self):
        return c_obj.get_grenade_position(self.g_index)

    def tick(self):
        if not self.check_life(delete=False):
            self.explode()
            self.delete()

    def explode(self):
        GrenadeExplodeAnimation(self.pos()).play()

    def delete(self):
        Projectile.delete(self)
        c_obj._destroy_grenade(self.g_index)

Grenade.init()


def _dat_callback(*args, **kwargs):
    Projectile.dat.apply(Projectile)
    Projectile.dat.apply(Laser)
    Projectile.dat.apply(Grenade)

Projectile.dat.on_change = _dat_callback


from game_state import GameStateGlobal
from c_lib.terrain_map import collisionDetection, isSolid
#import draw_utils
