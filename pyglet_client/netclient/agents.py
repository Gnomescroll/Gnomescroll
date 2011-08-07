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
from game_modes import NoTeam

import default_settings as settings

import vox_lib
import vector_lib
import raycast_utils
import vox

if settings.pyglet == False:
    import SDL.gl

'''
Physics for agents

-- 7/27/11 Copied from /server/agents.py --
'''
class AgentPhysics:

    #collision tests
    def point_collision_test(self, x_,y_,z_):
        x,y,z = self.pos()
        b_height = self.b_height
        t_height = self.t_height
        box_r = self.box_r

        z_max = z + self.t_height
        z_min = z - self.b_height
        x_max = x + box_r
        x_min = x - box_r
        y_max = y + box_r
        y_min = y - box_r

        #print str((x_min, x_max, y_min, y_max, z_min, z_max))
        #print str((x_,y_,z_))

        if x_min < x_ and x_ < x_max and y_min < y_ and y_ < y_max and z_min < z_ and z_ < z_max:
            return True
        else:
            return False

    def sphere_collision_test(self, x,y,z,r):
        pass

    def _tick_physics(self):
        x,y,z, vx,vy,vz, ax,ay,az = self.state
        ax,ay,az = (0,0,0)
        vx,vy = (0,0)
        #constants
        tr = 10. #tick rate
        tr2 = tr**2 #tick rate squared
        xy_brake = math.pow(.50, 1/(float(tr))) #in percent per second
        xy_speed = 2. / tr
        z_gravity = -.40 / tr2
        z_jetpack = 0.80 / tr2
        #gravity
    #TODO: should turn gravity off if agent is in contact with ground
        #velocity from acceleration and inputs

        vx += ax + self.v_x*xy_speed
        vy += ay + self.v_y*xy_speed

        #print str((vx,vy))
        if self.brake != 0:
            vx *= xy_brake
            vy *= xy_brake
            vz *= xy_brake

    #constants for collision box
        b_height = self.b_height
        t_height = self.t_height
        box_r = self.box_r

### Collisions on X axis collision ###

        xc_pos_current = 0
        xc_pos_projected = 0

        xc_neg_current = 0
        xc_neg_projected = 0

        bx_pos_current = floor(x+box_r)
        bx_pos_projected = floor(x+vx+box_r)

        bx_neg_current = floor(x-box_r)
        bx_neg_projected = floor(x+vx-box_r)

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for by in range(int(floor(y-box_r)), int(floor(y+box_r)+1)):
            #x+
                if self.collisionDetection(bx_pos_current,by,bz):
                    xc_pos_current +=1
                if self.collisionDetection(bx_pos_projected,by,bz):
                    xc_pos_projected +=1
            #x-
                if self.collisionDetection(bx_neg_current,by,bz):
                    xc_neg_current +=1
                if self.collisionDetection(bx_neg_projected,by,bz):
                    xc_neg_projected +=1

### Collision on Y axis ###

        yc_pos_current = 0
        yc_pos_projected = 0

        yc_neg_current = 0
        yc_neg_projected = 0

        by_pos_current = floor(y+box_r)
        by_pos_projected = floor(y+vy+box_r)

        by_neg_current = floor(y-box_r)
        by_neg_projected = floor(y+vy-box_r)

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for bx in range(int(floor(x-box_r)), int(floor(x+box_r)+1)):
            #x+
                if self.collisionDetection(bx,by_pos_current,bz):
                    yc_pos_current +=1
                if self.collisionDetection(bx,by_pos_projected,bz):
                    yc_pos_projected +=1
            #x-
                if self.collisionDetection(bx,by_neg_current,bz):
                    yc_neg_current +=1
                if self.collisionDetection(bx,by_neg_projected,bz):
                    yc_neg_projected +=1

### XY Collision ###

        xyc_projected = 0

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for by in range(int(floor(y+vy-box_r)), int(floor(y+vy+box_r)+1)):
                for bx in range(int(floor(x+vx-box_r+vx)), int(floor(x+vx+box_r)+1)):
                    if self.collisionDetection(bx,by,bz):
                        xyc_projected += 1

        xyc_current = 0

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for by in range(int(floor(y-box_r)), int(floor(y+box_r)+1)):
                for bx in range(int(floor(x-box_r+vx)), int(floor(x+box_r)+1)):
                    if self.collisionDetection(bx,by,bz):
                        xyc_current += 1

        #dont do this right now

        if False and xyc_projected != 0:
            # print "Projected XY collision!"
            vx =0
            vy =0

### XY collision constants
        xy_bounce = .65
        xy_bounce_v_threshold = 0.35 / tr

    ## handle x collisions
        #xc_pos_current, xc_pos_projected , xc_neg_current, xc_neg_projected

        if xc_pos_current == 0 and xc_neg_current == 0:
            if xc_pos_projected != 0:
                vx = 0
            if xc_neg_projected != 0:
                vx = 0
        else:
            pass
            #print "X collision error!!"

    ## handle y collisions
        #yc_pos_current, yc_pos_projected , yc_neg_current, yc_neg_projected

        if yc_pos_current ==0 and yc_neg_current ==0:
            if yc_pos_projected != 0:
                vy = 0
            if yc_neg_projected != 0:
                vy = 0
        else:
            pass
            #print "Y collision error!!"


        #environmental and input controls
        on_ground = self.on_ground #environmental state
        jetpack = self.jetpack #control state

        if on_ground == 0: #agent on ground
            az += (z_gravity) if z>0 else (-z_gravity)
        #jetpack adjustment to gravity
        if jetpack != 0: az += z_jetpack
    ## calculate velocity from acceleration inputs
        vz += az

##parameters for collision
        z_margin = .01
        z_bounce = .65
        z_bounce_v_threshold = 0.35 / tr

## determine if agent is on ground and if they are colliding with anything at current position
        zc_current = 0
        zc_ground = 0
        zc_neg_projected = 0
        bz_current = float(z - b_height)
        bz_ground = floor(z - b_height - z_margin)
        bz_neg_projected = floor(z+vz-b_height)

        for bx in range(int(floor(x-box_r)), int(floor(x+box_r)+1)):
            for by in range(int(floor(y-box_r)), int(floor(y+box_r)+1)):
                if self.collisionDetection(bx,by,bz_current):
                    zc_current +=1
                if self.collisionDetection(bx,by,bz_ground):
                     zc_ground += 1
                if self.collisionDetection(bx,by,bz_neg_projected):
                    zc_neg_projected +=1
    ##  calculate environmental state
        #agent ground state
        if zc_ground != 0:
            if self.on_ground != 1:
                self.on_ground = 1
                #print "On ground!"
        else:
            if self.on_ground == 1:
                #print "Off ground!"
                self.on_ground = 0

    ## apply velocity
        #Hard collision predicted and not inside of something already
        if zc_neg_projected != 0 and zc_current == 0:
            if vz < 0:
                if vz < -z_bounce_v_threshold: #vertical velocity bounce treshold
                    vz *= -1 *z_bounce
                else:
                    vz = 0

        if zc_current != 0: #if agent is inside of block, float them out the top
            z += .50 / tr

        if zc_neg_projected != 0:
            #print "Predicted neg Z-Collision!"
            pass
        if zc_current != 0:
            pass
            #print "Hard current neg Z-Collision!"

## Position Change ##
        z += vz
        x += vx
        y += vy

        self.state = [x,y,z, vx,vy,vz, ax,ay,az]
        #print 'agent state:'
        #if not self.you:
            #print self.state

'''
Agent Voxel
'''

class AgentVoxRender(vox.VoxRender):

    def __init__(self, model=None):
        if model is not None:
            vox.VoxRender.__init__(self, model)
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

'''
Render/Draw methods for agents
'''
class AgentRender:

    def draw(self):
        self.draw_aiming_direction()
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
        self.weapons.remove(weapon)
        self._adjust_active_weapon()

    def add(self, weapon, index=None):
        if index is None:
            self.weapons.append(weapon)
        else:
            self.weapons.insert(index, weapon)
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

        self._active_weapon = aw

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


class AgentInventory:

    def __init__(self, agent, items=None):
        self.agent = agent
        self.size = 100
        self.inv = []
        if items is not None:
            self.update_info(**items)

    def update_info(self, **inv_data):
        if 'inventory' in inv_data:
            inv = inv_data['inventory']
            new_inv = []
            for item in inv:
                try:
                    iid = item['id']
                except KeyError:
                    print 'WARNING: Object update via agent inventory; object id missing [ object :: %s ]' % (item,)
                    continue
                known_item = GameStateGlobal.itemList.get(iid, None)
                if known_item is None:
                    known_item = GameStateGlobal.itemList.create(**item)
                else:
                    known_item.update_info(**item)
                new_inv.append(known_item)
            self.inv = new_inv

    def has(self, obj_type):
        for obj in self.inv:
            if obj.type == obj_type:
                return obj
        return False

    def add(self, item, index=None):
        if item not in self.inv and item.take(self.agent):
            if index is None:
                self.inv.append(item)
            else:
                self.inv.insert(index, item)
            return item
        return False

    def drop(self, item):
        if item in self.inv and item.drop(self.agent):
            self.inv.remove(item)
            return item
        return False

    def __len__(self):
        return len(self.inv)
    def __iter__(self):
        return iter(self.inv)
    def __str__(self):
        return repr(self)
    def __repr__(self):
        return repr(self.inv)

'''
Data model for agent
'''
class AgentModel(object):

    HEALTH_MAX = 100
    _RESPAWN_TIME = 1. # seconds
    RESPAWN_TICKS = int(_RESPAWN_TIME / opts.tick)

    def __init__(self, owner=None, id=None, state=None, health=None, dead=False, active_block=1, team=None):
        if owner is None or id is None:
            return
        if state is None:
            state = [0,0,0,0,0,0,0,0,0]
        state = map(float, state)

        #self.collisionDetection = CubeGlobal.collisionDetection
        #assert False #fix
        self.collisionDetection = terrainMap.collisionDetection

        self.state = state #position, velocity, acceleration
        self.xa = state[3]
        self.ya = state[4]

        self.team = team

        #self.terrainMap = GameStateGlobal.terrainMap

        self.button_state = [0 for i in range(6)]

        self.id = id

        self.last_control_tick = 0
        self.last_button_tick = 0

        self.x_angle = 0
        self.y_angle = 0

        self.d_x = 0
        self.d_y = 0
        self.v_x = 0
        self.v_y = 0

        self.jetpack = 0
        self.brake = 0

        self.x_int = int(state[0])
        self.y_int = int(state[1])
        self.z_int = int(state[2])

        self.on_ground = 0

        if health is None:
            self.health = self.HEALTH_MAX
        else:
            self.health = health
        self.dead = bool(dead)

        self.owner = owner

        self.you = False

        self.active_block = active_block   # which block to create

        #settings
        self.b_height = 1.5
        self.t_height = .75
        self.box_r = .30

    def tick(self):
        if not self.dead:
            self._tick_physics()

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

        if 'inventory' in agent:
            self.inventory.update_info(**agent['inventory'])

        if 'owner' in agent:
            self.owner = agent['owner']
        if 'state' in agent:
            state = agent['state']
            if type(state) == list and len(state) == len(self.state):
                self.state = state

        if 'team' in agent:
            self.team = GameStateGlobal.teamList[agent['team']]

        GameStateGlobal.agentList.update(self, *args)

#experimental
    def __getattribute__(self, name):
        val = object.__getattribute__(self, name)
        if name == 'team':
            if not isinstance(val, NoTeam):
                val = GameStateGlobal.teamList[val]
                self.team = val
        return val

    def pos(self, xyz=None):
        if xyz is None:
            return self.state[0:3]
        else:
            self.state[0:3] = xyz
            #self.x, self.y, self.z = xyz

    def nearby_objects(self):
        for obj in GameStateGlobal.itemList.values():
            if vector_lib.distance(self.pos(), obj.pos()) < obj.radius:
                self.near_object(obj)

    def near_object(self, obj):
        pass

    def direction(self, normalize=True):
        v = vector_lib.angle2vector(self.x_angle, self.y_angle)
        if normalize:
            v = vector_lib.normalize(v)
        return v

    def angles(self):
        return [self.x_angle, self.y_angle]

    # set agent state explicitly
    def set_control_state(self, control_state, angle=None, tick=None):
        d_x, d_y, v_x, v_y, jetpack, brake = control_state
        self.d_x = d_x
        self.d_y = d_y
        self.v_x = v_x
        self.v_y = v_y
        self.jetpack = jetpack
        self.brake = brake

        if tick is not None:
            self.last_control_tick = tick
        if angle is not None:
            self.set_angle(angle)

    def set_angle(self, angle):
        self.x_angle, self.y_angle = angle

    def control_state(self):
        return [\
            self.d_x,
            self.d_y,
            self.v_x,
            self.v_y,
            self.jetpack,
            self.brake
        ]

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
class Agent(AgentModel, AgentPhysics, AgentRender, AgentVoxRender):

    def __init__(self, owner=None, id=None, state=None, weapons=None, health=None, dead=False, active_block=1, items=None, team=None):
        #self.init_vox()
        AgentVoxRender.__init__(self)
        AgentModel.__init__(self, owner, id, state, health, dead, active_block, team)
        self.inventory = AgentInventory(self, items)
        self.weapons = AgentWeapons(self, weapons)

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
        if self.agent.team.is_viewers():
            return
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

    def drop(self, weapon):
        old_len = len(self.weapons)
        AgentWeapons.drop(self, weapon)
        if old_len != len(self.weapons):
            NetOut.sendMessage.drop_weapon(self.agent, weapon.id)

    def _adjust_active_weapon(self):
        prev = self._active_weapon
        AgentWeapons._adjust_active_weapon(self)
        if self._active_weapon != prev:
            NetOut.sendMessage.change_weapon(self.agent, self._active_weapon)


class PlayerAgentInventory(AgentInventory):

    def __init__(self, agent, items=None):
        AgentInventory.__init__(self, agent, items)

    def add(self, item, index=None):
        item = AgentInventory.add(self, item, index)
        return item

    def drop(self, item):
        item = AgentInventory.drop(self, item)
        return item


'''
Client's player's agent
'''
class PlayerAgent(AgentModel, AgentPhysics, PlayerAgentRender, AgentVoxRender):

    def __init__(self, owner=None, id=None, state=None, weapons=None, health=None, dead=False, active_block=1, items=None, team=None):
        AgentVoxRender.__init__(self)
        AgentModel.__init__(self, owner, id, state, health, dead, active_block, team)

        self.weapons = PlayerAgentWeapons(self, weapons)
        self.inventory = PlayerAgentInventory(self, items)

        self.you = True
        #self.control_state = [0,0,0,0,0,0]
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
        if self.team.is_viewers():
            return
        weapon = self.weapons.active()
        if weapon is None:
            return
        fire_command = weapon.fire()
        if fire_command:
            if weapon.hitscan:
                self.hitscan(weapon)
            else:
                NetOut.sendMessage(fire_command, self)

    def hitscan(self, weapon=None):
        if self.team.is_viewers():
            return
        #print 'HITSCAN!!'
        if weapon is not None:
            weapon.animation(agent=self).play()

        # check agent
        ignore_vox = [player.agent.vox.id for player in GameStateGlobal.teamList.get_viewers().values()]
        ignore_vox.append(self.vox.id)
        print 'ignoring %s' % (ignore_vox,)
        (ag, adistance, vox) = vox_lib.hitscan2(self.x,self.y,self.z,self.x_angle, self.y_angle, ignore_vox=ignore_vox)
        body_part_id = 1
        block = raycast_utils.ray_nearest_block(self.x, self.y, self.z, self.x_angle, self.y_angle)
        bdistance = None
        if block is not None:
            bdistance = vector_lib.distance(self.pos(), block)
        #check block
        # if both agent & block got hit, check which is closer

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
        print 'hitscan'
        print target
        NetOut.sendMessage.hitscan(target)

    def add_ammo(self, amt, weapon_type):
        if self.team.is_viewers():
            return
        for weapon in weapons:
            if weapon.type == weapon_type:
                weapon.restock(amt)
                break

    def reload(self):
        if self.team.is_viewers():
            return
        weapon = self.weapons.active()
        if weapon is None:
            return
        reload_command = weapon.reload()
        if reload_command:
            NetOut.sendMessage(reload_command, self)

    def set_active_block(self, block_type=None):
        if self.team.is_viewers():
            return
        if block_type is None:
            pos = ray_nearest_block(self.x,self.y,self.z,self.x_angle,self.y_angle)
            block_type = self.facing_block()
        if not block_type:
            return
        self.active_block = block_type
        #print 'set active block to ', self.active_block

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

    def pickup_item(self, item, index=None):
        if self.team.is_viewers():
            return
        item = self.inventory.add(item, index)
        if item:
            NetOut.sendMessage.pickup_item(self, item, index)

    def drop_item(self, item):
        if self.team.is_viewers():
            return
        item = self.inventory.drop(item)
        if item:
            NetOut.sendMessage.drop_item(self, item)

    def nearby_objects(self):
        for obj in GameStateGlobal.itemList.values():
            if vector_lib.distance(self.pos(), obj.pos()) < obj.radius:
                self.pickup_item(obj)


import cube_lib.terrain_map as terrainMap
from net_out import NetOut
from raycast_utils import *
from draw_utils import *
#from cube_dat import CubeGlobal
#import cube_lib.VBO
