'''
Agents:
    Objects through which a Player experiences the game world
'''

import math
from math import sin, cos, pi
from math import floor, ceil, fabs


from game_state import GameStateGlobal #Deprecate?

from opts import opts

from weapons import LaserGun, Pick, BlockApplier

import settings

import vox_lib
import vector_lib
import raycast_utils

if settings.pyglet == False:
    import SDL.gl

'''
Physics for agents
'''
class AgentPhysics:

    # apply physics to agent
    def tick(self):
        pass
        #if not self.dead:
            #self._tick_physics()

    def _tick_physics(self):
        assert False  #needs to do interopolation



class VoxRender:

    def init_vox(self, model = None):
        if False: ##FIX THIS; need vox file
            assert False
            vox_loader = vox_lib.Vox_loader()
            self.vox = vox_loader.load()
            self.vox.set_object(self)
        else:
            self.vox = vox_lib.Vox(0,0,5,0, 8,8,8)
            self.vox.set_object(self)

            self.vox.set(5,5,5,255,0,0,0)

            self.vox.set(4,4,0,255,0,255,0)
            self.vox.set(4,4,1,0,255,255,0)
            self.vox.set(4,4,2,0,0,255,0)
            self.vox.set(4,4,3,0,255,255,0)

            self.vox.set(4,4,4,0,255,255,0)
            self.vox.set(4,4,5,0,0,255,0)
            self.vox.set(4,4,6,0,255,255,0)
            self.vox.set(4,4,7,255,0,255,0)

            self.vox.set(0,0,0, 0,255,0,0)
            self.vox.set(0,7,0, 0,255,0,0)
            self.vox.set(7,0,0, 0,255,0,0)
            self.vox.set(7,7,0, 0,255,0,0)

            self.vox.set(0,0,7, 0,255,0,0)
            self.vox.set(0,7,7, 0,255,0,0)
            self.vox.set(7,0,7, 0,255,0,0)
            self.vox.set(7,7,7, 0,255,0,0)

    def update_vox(self):
        self.vox.move(self.x,self.y,self.z, 3.14159*self.x_angle)

    def draw_vox(self):
        self.vox.draw()

'''
Render/Draw methods for agents
'''
class AgentRender:

    def draw(self):
        #self.draw_aiming_direction()
        self.draw_bounding_box()

        self.update_vox()
        self.draw_vox()
        ##self.draw_selected_cube()
        ##self.draw_selected_cube2()
        self.draw_position(points=10, seperation = 0.10)
        #self.draw_velocity(point_density=15, units=200)
        #self.draw_acceleration(point_density=15, units=100000)

    def draw_position(self, points, seperation):
        v_num = 0
        v_list = []
        c_list = []
        for n in range(-points, points):
            temp = float(n)*float(seperation)
            v_list += [self.x+temp, self.y, self.z]
            v_list += [self.x,self.y+temp, self.z]
            v_list += [self.x,self.y, self.z+temp]
            c_list += [140,0,0]*3
            v_num +=3

        if settings.pyglet:
            pyglet.graphics.draw(v_num, GL_POINTS,
            ("v3f", v_list),
            ("c3B", c_list)
            )
        else:
            for i in range(0,v_num):
                x,y,z = v_list[3*i], v_list[3*i+1], v_list[3*i+2]
                r,g,b = c_list[3*i], c_list[3*i+1], c_list[3*i+2]
                SDL.gl.draw_point(r,g,b,x,y,z)

    def draw_bounding_box(self):
        #agent parameters
        b_height = self.b_height
        t_height = self.t_height
        box_r = self.box_r

        x = self.x
        y = self.y
        z = self.z
        #cordinates for corners
        x_neg = x-box_r
        x_pos = x+box_r
        y_neg = y-box_r
        y_pos = y+box_r

        z0 = z-b_height
        z1 = z
        z2 = z+t_height

        draw_box(x_neg, x_pos, y_neg, y_pos, z0, z1, [255,0,0])
        draw_box(x_neg, x_pos, y_neg, y_pos, z1, z2, [180,0,0])


class AgentWeapons:

    def __init__(self, agent, weapons=None):
        self.agent = agent

        if weapons is None:
            weapons = []
            active_weapon = None
        else:
            if 'active' in weapons:
                active_weapon = weapons['active']
            else:
                active_weapon = None
            if 'weapons' in weapons:
                weapons = weapons['weapons']
            else:
                weapons = []

        weapon_objs = []
        for weapon in weapons:
            known_weapon = GameStateGlobal.weaponList.get(weapon['id'], None)
            if known_weapon is None:
                known_weapon = GameStateGlobal.weaponList.create(**weapon)
            weapon_objs.append(known_weapon)
        self.weapons = weapon_objs

        if active_weapon is None:
            self._active_weapon = None
            self._adjust_active_weapon()
        else:
            self._active_weapon = active_weapon #    which weapon is held

    def active(self):
        if self._active_weapon is None:
            return None
        return self.weapons[self._active_weapon]

    def update_info(self, **weapons_data):
        if 'weapons' in weapons_data:
            weapons = weapons_data['weapons']
            new_weapons = []
            for weapon in weapons:
                try:
                    wid = weapon['id']
                except KeyError:
                    print 'WARNING: Weapon updating via agent; weapon id missing'
                    continue
                known_weapon = GameStateGlobal.weaponList.get(wid, None)
                if known_weapon is None: # agent assigned unknown weapon
                    known_weapon = GameStateGlobal.weaponList.create(**weapon)
                else:
                    known_weapon.update_info(**weapon)
                new_weapons.append(known_weapon)
            self.weapons = new_weapons
            self._adjust_active_weapon()

        if 'active' in weapons_data:
            self._active_weapon = weapons_data['active']

    def drop(self, weapon):
        old_len = len(self.weapons)
        self.weapons = [w for w in self.weapons if w != weapon]
        if old_len != len(self.weapons):
            NetOut.sendMessage.drop_weapon(self.agent, weapon.id)
        self._adjust_active_weapon()

    def _adjust_active_weapon(self):
        n = len(self)                   # number of weapons
        aw = self._active_weapon        # active list index in weapons

        if aw is None:                 # no weapon equipped (default init state)
            if n > 0:                   # if there are weapons now
                aw = 0                  # set it to the first weapon

        else:                               # weapon is currently equipped
            last_weapon = n - 1
            if aw > last_weapon:                  # num weapons shrunk below active index
                aw = last_weapon                  # set active to last weapon
                if aw < 0:                  # if there are no weapons
                    aw = None               # active is None

        if self._active_weapon != aw:
            self._active_weapon = aw
            NetOut.sendMessage.change_weapon(self.agent, aw)

    def has(self, weapon_type):
        print self.weapons
        for weapon in self.weapons:
            if weapon.type == weapon_type:
                return weapon
        return False

    def __len__(self):
        return len(self.weapons)

    def __iter__(self):
        return iter(self.weapons)

'''
Data model for agent
'''
class AgentModel:

    HEALTH_MAX = 100
    _RESPAWN_TIME = 1. # seconds
    RESPAWN_TICKS = int(_RESPAWN_TIME / opts.tick)

    def __init__(self, owner=None, id=None, state=None, weapons=None, health=None, dead=False, active_block=1):
        if owner is None or id is None:
            return
        if state is None:
            state = [0,0,0,0,0,0,0,0,0]
        state = map(lambda k: float(k), state)

        self.state = state #position, velocity, acceleration
        self.xa = state[3]
        self.ya = state[4]

        #self.terrainMap = GameStateGlobal.terrainMap

        self.id = id

        self.d_x = 0
        self.d_y = 0
        self.d_xa = 0
        self.d_za = 0

        self.x_angle = 0
        self.y_angle = 0

        self.last_control_tick = 0
        self.jetpack = 0
        self.brake = 0

        self.x_int = int(state[0])
        self.y_int = int(state[1])
        self.z_int = int(state[2])

        if health is None:
            self.health = self.HEALTH_MAX
        else:
            self.health = health
        self.dead = bool(dead)

        self.weapons = AgentWeapons(self, weapons)
        self.owner = owner

        self.you = False

        self.active_block = active_block   # which block to create

        #settings
        self.b_height = 1.5
        self.t_height = .75
        self.box_r = .30

    def update_info(self, **agent):
        args = []
        if 'id' in agent:
            args.append(self.id)
            self.id = agent['id']
        if 'health' in agent:
            self.health = agent['health']
        if 'dead' in agent:
            self.dead = bool(agent['dead'])
            if self.dead:
                print 'you are now dead'

        if 'weapons' in agent:
            self.weapons.update_info(**agent['weapons'])

        if 'owner' in agent:
            self.owner = agent['owner']
        if 'state' in agent:
            state = agent['state']
            if type(state) == list and len(state) == len(self.state):
                self.state = state

        GameStateGlobal.agentList.update(self, *args)

    def pos(self):
        return self.state[0:3]

    def direction(self, normalize=True):
        v = vector_lib.angle2vector(self.x_angle, self.y_angle)
        if normalize:
            v = vector_lib.normalize(v)
        return v

    # set agent state explicitly
    def set_agent_control_state(self, *args):
        d_x, d_y, d_xa, d_za, jetpack, brake, tick = args
        self.last_control_tick = tick
        self.d_x = d_x #a byte
        self.d_y = d_y #a byte
        self.d_xa = d_xa
        self.d_za = d_za
        self.jetpack = jetpack
        self.brake = brake

    @property
    def x(self):
        return self.state[0]
    @x.setter
    def x(self, val):
        self.state[0] = val
    @property
    def y(self):
        return self.state[1]
    @y.setter
    def y(self, val):
        self.state[1] = val
    @property
    def z(self):
        return self.state[2]
    @z.setter
    def z(self, val):
        self.state[2] = val
    @property
    def vx(self):
        return self.state[3]
    @vx.setter
    def vx(self, val):
        self.state[3] = val
    @property
    def vy(self):
        return self.state[4]
    @vy.setter
    def vy(self, val):
        self.state[4] = val
    @property
    def vz(self):
        return self.state[5]
    @vz.setter
    def vz(self, val):
        self.state[5] = val
    @property
    def ax(self):
        return self.state[6]
    @ax.setter
    def ax(self, val):
        self.state[6] = val
    @property
    def ay(self):
        return self.state[7]
    @ay.setter
    def ay(self, val):
        self.state[7] = val
    @property
    def az(self):
        return self.state[8]
    @az.setter
    def az(self, val):
        self.state[8] = val

    def normalized_direction(self):
        return vector_lib.normalize(vector_lib.angle2vector(self.x_angle, self.y_angle))

# represents an agent under control of a player
class Agent(AgentModel, AgentPhysics, AgentRender, VoxRender):

    def __init__(self, owner=None, id=None, state=None, weapons=None, health=None, dead=False, active_block=1):
        self.init_vox()
        AgentModel.__init__(self, owner, id, state, weapons, health, dead, active_block)

'''
Client's player's agent draw methods
'''
class PlayerAgentRender(AgentRender):


    def draw(self):
        self.draw_aiming_direction()
        self.draw_bounding_box()
        #self.draw_selected_cube()
        #self.draw_selected_cube2()
        self.draw_position(points=10, seperation = 0.10)
        self.draw_velocity(point_density=15, units=200)
        self.draw_acceleration(point_density=15, units=100000)
        #vox models
        self.update_vox()
        self.draw_vox()

        pos = ray_cast_farest_empty_block(self.x,self.y,self.z,self.x_angle,self.y_angle)
        if pos != None:
            #print str(pos)
            (x,y,z) = pos
            draw_cube(x,y,z,[0,155,0])
        #collides at (dx*n,dy*n,dz*n)
        #free block at (dx*(n-1), dy*(n-1), dz*(n-1) )

    def draw_position(self, points, seperation):
        v_num = 0
        v_list = []
        c_list = []
        for n in range(-points, points):
            temp = float(n)*float(seperation)
            v_list += [self.x+temp, self.y, self.z]
            v_list += [self.x,self.y+temp, self.z]
            v_list += [self.x,self.y, self.z+temp]
            c_list += [150,0,0]*3
            v_num +=3
        #deprecate
        if settings.pyglet:
            pyglet.graphics.draw(v_num, GL_POINTS,
            ("v3f", v_list),
            ("c3B", c_list)
            )
        else:
            for i in range(0,v_num):
                x,y,z = v_list[3*i], v_list[3*i+1], v_list[3*i+2]
                r,g,b = c_list[3*i], c_list[3*i+1], c_list[3*i+2]
                SDL.gl.draw_point(r,g,b,x,y,z)


    def draw_velocity(self, point_density, units):
        v_num = 0
        v_list = []
        c_list = []
        vlen = units*math.sqrt(self.vx*self.vx + self.vy*self.vy + self.vz*self.vz)
        #print "vlen= " + str(vlen)
        num_points = int(math.floor(point_density*vlen))
        c = 1.0 / float(point_density)
        for n in range(0, num_points):
            v_list += [self.x+units*n*c*self.vx, self.y+units*n*c*self.vy, self.z+units*n*c*self.vz]
            c_list += [0,0,255]
            v_num +=1
        #deprecate
        if settings.pyglet:
            pyglet.graphics.draw(v_num, GL_POINTS,
            ("v3f", v_list),
            ("c3B", c_list)
            )
        else:
            for i in range(0,v_num):
                x,y,z = v_list[3*i], v_list[3*i+1], v_list[3*i+2]
                r,g,b = c_list[3*i], c_list[3*i+1], c_list[3*i+2]
                SDL.gl.draw_point(r,g,b,x,y,z)


    def draw_acceleration(self, point_density, units):
        v_num = 0
        v_list = []
        c_list = []
        vlen = units*math.sqrt(self.ax*self.ax + self.ay*self.ay + self.az*self.az)
        #print "alen= " + str(vlen)
        num_points = int(math.floor(point_density*vlen))
        c = 1.0 / float(point_density)
        for n in range(0, num_points):
            v_list += [self.x+units*n*c*self.ax, self.y+units*n*c*self.ay, self.z+units*n*c*self.az]
            c_list += [0,255,0]
            v_num +=1
        #deprecate
        if settings.pyglet:
            pyglet.graphics.draw(v_num, GL_POINTS,
            ("v3f", v_list),
            ("c3B", c_list)
            )
        else:
            for i in range(0,v_num):
                x,y,z = v_list[3*i], v_list[3*i+1], v_list[3*i+2]
                r,g,b = c_list[3*i], c_list[3*i+1], c_list[3*i+2]
                SDL.gl.draw_point(r,g,b,x,y,z)

    def draw_aiming_direction(self, distance=50):
        dx = cos( self.x_angle * pi) * cos( self.y_angle * pi)
        dy = sin( self.x_angle * pi) * cos( self.y_angle * pi)
        dz = sin( self.y_angle)

        ep = 0.33
        v_list = []
        v_num = 0
        cf = 0.
        while cf < distance:
            v_num += 1
            x= self.x + cf*dx
            y= self.y + cf*dy
            z= self.z + cf*dz
            cf += ep
            v_list += [x,y,z]
        #print str(v_list)
        #print str(v_num)
        c_list = [200,0,0]*v_num

        if settings.pyglet:
            pyglet.graphics.draw(v_num, GL_POINTS,
            ("v3f", v_list),
            ("c3B", c_list)
            )
        else:
            for i in range(0,v_num):
                x,y,z = v_list[3*i], v_list[3*i+1], v_list[3*i+2]
                r,g,b = c_list[3*i], c_list[3*i+1], c_list[3*i+2]
                SDL.gl.draw_point(r,g,b,x,y,z)

    def draw_selected_cube(self):
        dx = cos( self.x_angle * pi) * cos( self.y_angle * pi)
        dy = sin( self.x_angle * pi) * cos( self.y_angle * pi)
        dz = sin( self.y_angle)

        l_list = []
        for ix in range (0, 5):
            if dx*dx < 0.00000001: #prevent division by zero errors
                continue
            l = (ix-self.x)/fabs(dx)
            (x_,y_,z_) = (dx*l+self.x, dy*l+self.y, dz*l+self.z)
            if dx < 0:
                draw_cube(x_-1, floor(y_), floor(z_), 2) #north
            elif dx >= 0:
                draw_cube(x_, floor(y_), floor(z_), 3) #south

        for iy in range(0,5):
            if dy*dy < 0.00000001: #prevent division by zero errors
                continue
            l = (iy-self.x)/fabs(dy)
            (x_,y_,z_) = (dx*l+self.x, dy*l+self.y, dz*l+self.z)
            if dy < 0:
                draw_cube(floor(x_), y_-1, floor(z_), 4) #west
            elif dy >= 0:
                draw_cube(floor(x_), y_, floor(z_), 5) #east

        if dx < 0:
            pass #north side

    def draw_selected_cube2(self, distance = 20):
        dx = cos( self.x_angle * pi) * cos( self.y_angle * pi)
        dy = sin( self.x_angle * pi) * cos( self.y_angle * pi)
        dz = sin( self.y_angle)

        cube_sides = []

        l= 0.
        ix = 0.
        while l < distance:
            if dx*dx < 0.00000001: #prevent division by zero errors
                break
            l = (ix-self.x)/fabs(dx)
            (x_,y_,z_) = (dx*l+self.x, dy*l+self.y, dz*l+self.z)
            if dx < 0:
                cube_sides.append((x_-1, floor(y_), floor(z_), 2)) #north
            elif dx >= 0:
                cube_sides.append((x_, floor(y_), floor(z_), 3)) #south
            ix += 1

        l=0.
        iy = 0.
        while l < distance:
            if dy*dy < 0.00000001: #prevent division by zero errors
                break
            l = (iy-self.x)/fabs(dy)
            (x_,y_,z_) = (dx*l+self.x, dy*l+self.y, dz*l+self.z)
            if dy < 0:
                cube_sides.append((floor(x_), y_-1, floor(z_), 4)) #west
            elif dy >= 0:
                cube_sides.append((floor(x_), y_, floor(z_), 5)) #east
            iy += 1

        l=0.
        iz = 0.
        while l < distance:
            if dz*dz < 0.00000001: #prevent division by zero errors
                break
            l = (iz-self.x)/fabs(dz)
            (x_,y_,z_) = (dx*l+self.x, dy*l+self.y, dz*l+self.z)
            if dz < 0:
                cube_sides.append((floor(x_), floor(y_), z_-1, 0)) #top
            elif dz >= 0:
                cube_sides.append((floor(x_), floor(y_), z_, 1)) #bottom
            iz += 1

        draw_sides(cube_sides)


class PlayerAgentWeapons(AgentWeapons):

    def switch(self, weapon_index):
        old = self._active_weapon
        num_weapons = len(self.weapons)
        if num_weapons == 0:
            self._active_weapon = None
            if old is not None:
                NetOut.sendMessage.change_weapon(self.agent, self._active_weapon)
            return

        if type(weapon_index) == int:
            weapon_index += -1

        if self._active_weapon is None:
            active = 0

        if weapon_index == 'up':
            if self._active_weapon is None:
                self._active_weapon = 0
            else:
                self._active_weapon = (self._active_weapon + 1) % num_weapons
        elif weapon_index == 'down':
            if self._active_weapon is None:
                self._active_weapon = num_weapons - 1
            else:
                self._active_weapon = (self._active_weapon - 1) % num_weapons
        elif weapon_index < num_weapons:
                self._active_weapon = weapon_index

        if old != self._active_weapon:
            NetOut.sendMessage.change_weapon(self.agent, self._active_weapon)

        print 'weapon is: %s' % (self.active(),)


'''
Client's player's agent
'''
class PlayerAgent(AgentModel, AgentPhysics, PlayerAgentRender, VoxRender):

    def __init__(self, owner=None, id=None, state=None, weapons=None, health=None, dead=False, active_block=1):
        self.init_vox()
        AgentModel.__init__(self, owner, id, state, weapons, health, dead, active_block)

        self.weapons = PlayerAgentWeapons(self, weapons)

        self.you = True
        self.control_state = [0,0,0,0,0,0,0]
        self.x_angle = 0
        self.y_angle = 0

        self.vx = 0
        self.vy = 0
        self.vz = 0
        self.ax = 0
        self.ay = 0
        self.az = 0

        #settings
        self.b_height = 1.5
        self.t_height = .75
        self.box_r = .30

    def fire(self):
        weapon = self.weapons.active()
        if weapon is None:
            return
        fire_command = weapon.fire()
        if fire_command:
            if weapon.hitscan:
                print 'HITSCAN!!'
                weapon.animation(agent=self).play()

                (ob, distance, vox) = vox_lib.hitscan2(self.x,self.y,self.z,self.x_angle, self.y_angle)
                if distance > 0:
                    if ob == None:
                        print "Hit None, distance %f" % (distance/256.0)
                        ttype = 'empty'
                    else:
                        print "Hit Object, distance %f" % (distance/256.0)
                else:
                    print "Hit nothing, distance %f" % (distance/256.0)
                    ttype = 'empty'

                return None ## DEBUG

                print 'HITSCAN!!'
                weapon.animation(agent=self).play()

                # check agent
                (ag, adistance) = vox_lib.hitscan2(self.x,self.y,self.z,self.x_angle, self.y_angle)
                if ob == None:
                    body_part_id = 1
                    block = raycast_utils.ray_nearest_block(self.x, self.y, self.z, self.x_angle, self.y_angle)
                    bdistance = None
                if block is not None:
                    bdistance = vector_lib.distance(self.pos(), block)
                #check block
                # if both agent & block got hit, check which is closer
                    ttype = 'empty'

                if ag is not None and block is not None:
                    if bdistance < adistance:
                        ttype = 'block'
                        loc = block
                    else:
                        ttype = 'agent'
                        loc = (ag.id, body_part_id)
                elif ag is not None:
                    ttype = 'agent'
                    loc = (ag.id, body_part_id)
                elif block is not None:
                    ttype = 'block'
                    loc = block
                else:
                    ttype = 'empty'
                    loc = self.normalized_direction()

                # determine target w/ ray cast
                #target = ray_cast_from(agent)
                target = {
                    'type'  :   ttype,
                    'loc'   :   loc
                }
                NetOut.sendMessage.hitscan(target)
            else:
                NetOut.sendMessage(fire_command, self)

    def add_ammo(self, amt, weapon_type):
        for weapon in weapons:
            if weapon.type == weapon_type:
                weapon.restock(amt)
                break

    def reload(self):
        weapon = self.weapons.active()
        if weapon is None:
            return
        reload_command = weapon.reload()
        if reload_command:
            NetOut.sendMessage(reload_command, self)

    def set_active_block(self, block_type=None):
        if block_type is None:
            pos = ray_nearest_block(self.x,self.y,self.z,self.x_angle,self.y_angle)
            block_type = self.facing_block()
        if not block_type:
            return
        self.active_block = block_type
        print 'set active block to ', self.active_block

    def facing_block(self):
        block = self.nearest_block_position()
        if block is None:
            return None
        #block = GameStateGlobal.terrainMap.get(*block)
        block = terrainMap.get(*block)
        return block

    def facing_block_position(self):
        return ray_cast_farest_empty_block(self.x, self.y, self.z, self.x_angle, self.y_angle)

    def nearest_block_position(self):
        return ray_nearest_block(self.x,self.y,self.z,self.x_angle,self.y_angle)

    def pan(self, dx_angle, dy_angle):
        self.x_angle += dx_angle
        self.y_angle += dy_angle
        if self.y_angle < -0.499:
            self.y_angle = -0.499
        if self.y_angle > 0.499:
            self.y_angle = 0.499

import cube_lib.terrain_map as terrainMap
from net_out import NetOut
from raycast_utils import *
from draw_utils import *
