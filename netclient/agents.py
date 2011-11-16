'''
Agents:
    Objects through which a Player experiences the game world
'''

import opts
opts = opts.opts

import random
import vox_lib
import vector_lib
import vox
import c_lib.c_lib_particles
import c_lib.c_lib_agents
import c_lib._ray_trace as ray_tracer
import c_lib.c_lib_agents as cAgents
import c_lib.terrain_map as terrainMap
import c_lib.c_lib_sdl as cSDL
if opts.sound:
    import sound.sounds as sounds

from math import sin, cos, pi
from math import floor, ceil, fabs, pow
from game_state import GameStateGlobal #Deprecate?
from weapons import LaserGun, Pick, BlockApplier
from game_modes import NoTeam
from c_lib.c_lib_agents import _update_agent_vox, _init_agent_vox, AgentWrapper, PlayerAgentWrapper, AgentListWrapper, set_player_agent_id, set_agent_control_state
from draw_utils import *
from net_out import NetOut
from input import InputGlobal

'''
Physics for agents

-- 7/27/11 Copied from /server/agents.py --
'''
class AgentPhysics(object):

    def compute_state(self):
        # only v_x and v_y are used
        v = 1
        d_x, d_y, v_x, v_y = 0,0,0,0
        u,d,l,r, jetpack, brake = self.button_state

        if u:
                v_x += v*cos( self.x_angle * pi)
                v_y += v*sin( self.x_angle * pi)
        if d:
                v_x += -v*cos( self.x_angle * pi)
                v_y += -v*sin( self.x_angle * pi)
        if l:
                v_x += v*cos( self.x_angle * pi + pi/2)
                v_y += v*sin( self.x_angle * pi + pi/2)
        if r:
                v_x += -v*cos( self.x_angle * pi + pi/2)
                v_y += -v*sin( self.x_angle * pi + pi/2)

        return [\
            d_x,
            d_y,
            v_x,
            v_y,
            jetpack,
            brake,
        ]

    #collision tests
    def point_collision_test(self, x_,y_,z_):
        x,y,z = self.pos()

        z_max = z + self.b_height
        z_min = z
        x_max = x + self.box_r
        x_min = x - self.box_r
        y_max = y + self.box_r
        y_min = y - self.box_r

        if x_min < x_ and x_ < x_max and y_min < y_ and y_ < y_max and z_min < z_ and z_ < z_max:
            return True
        else:
            return False

    def sphere_collision_test(self, x,y,z,r):
        pass

'''
Agent Voxel
'''

class AgentVoxRender(vox.VoxRender):

    def __init__(self):
        _init_agent_vox(self.id)

    def update_vox(self):
        _update_agent_vox(self.id)


'''
Render/Draw methods for agents
'''
class AgentRender(object):

    def draw(self):
        assert False
        #P.event("Draw 2")
        self.draw_aiming_direction()
        self.draw_bounding_box()

        self.update_vox()
        self.draw_vox()

    def draw_position(self, points, seperation):
        print "Draw position Deprecated!"
        return

    def draw_bounding_box(self):
        b_height = self.b_height;t_height = self.t_height;box_r = self.box_r
        x = self.x;y = self.y;z = self.z
        c_lib.c_lib_agents._draw_agent_bounding_box(x,y,z-b_height, box_r, 2.0, 3.0)

    def draw_aiming_direction(self, distance=50):
        c_lib.c_lib_agents._draw_agent_aiming_direction(self.x,self.y,self.z, self.x_angle, self.y_angle)

    def bleed(self):
        print 'BLEEDING'
        create_blood = c_lib.c_lib_particles._create_blood
        n = 100
        v = 15
        blood_pos = self.pos()
        blood_pos[0] += self.box_r
        blood_pos[1] += self.box_r
        blood_pos[2] += self.b_height * 0.75
        for i in range(n):
            
            x,y,z = [i + ((random.random()-0.5) / 20) for i in blood_pos]
            vx = v*(random.random() -0.5)
            vy = v*(random.random() -0.5)
            vz = random.randrange(-4, 4) + random.random()
            create_blood(x, y,z, vx, vy, vz)

            # need directional blood
            # take vector from killer, put vel in random bounded cone around vector


class AgentWeapons(object):

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
        for weapon in self.weapons:
            if weapon.type == weapon_type:
                return weapon
        return False

    def __len__(self):
        return len(self.weapons)

    def __iter__(self):
        return iter(self.weapons)


class AgentInventory(object):

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

    # checks for object a specific type (e.g. Flag)
    # rename to: has_object_type
    def has(self, obj_type):
        for obj in self.inv:
            if obj.type == obj_type:
                return obj
        return False

    def _add(self, item, index=None):
        if item not in self.inv:
            if index is None:
                self.inv.append(item)
            else:
                self.inv.insert(index, item)
            return item
        return False

    def add(self, item, index=None):
        if item.take(self.agent):
            return self._add(item, index)
        return False

    def can_add(self, item):
        return (item not in self.inv and item.can_take(self.agent))

    def can_drop(self, item):
        return (item in self.inv and item.can_drop(self.agent))

    def _drop(self, item):
        if item in self.inv:
            self.inv.remove(item)
            return item
        return False

    def drop(self, item):
        if item.drop(self.agent):
            return self._drop(item)
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
class AgentModel(AgentWrapper):

    HEALTH_MAX = 100
    _RESPAWN_TIME = 1000. # milliseconds
    _TICK_RATE = 30. # milliseconds
    RESPAWN_TICKS = int(_RESPAWN_TIME / _TICK_RATE)

    def __init__(self, owner, id, state=None, health=None, dead=False, team=None):
        if owner is None or id is None:
            print 'WARNING!! Creating agent with no owner or id'
            raise ValueError, "Attempted to create agent with no owner or id"

        AgentWrapper.__init__(self, id)
            
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

        self.button_state = [0 for i in range(11)]

        self.id = id

        self.last_control_tick = 0
        self.last_button_tick = 0

        self._x_angle = 0
        self._y_angle = 0

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

    def tick(self):
        if not self.dead:
            self._tick_physics()

    def update_info(self, **agent):
        old_health = health = self.health
        args = []
        was_alive = not self.dead
        if 'id' in agent:
            args.append(self.id)
            self.id = agent['id']
        if 'health' in agent:
            self.health = agent['health']
            health = self.health
        if 'dead' in agent:
            self.dead = bool(agent['dead'])
            if was_alive and self.dead:
                self.bleed()
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

        if not was_alive and not self.dead:
            print "REVIVING"
            print self.pos()

        GameStateGlobal.agentList.update(self, *args)

    def _update_team_object(self):
        t = self.__dict__['team']
        if not isinstance(t, NoTeam):
            t = GameStateGlobal.teamList[t]
            if t is not None:
                self.team = t

    def __getattribute__(self, name):
        try:
            val = AgentWrapper.__getattribute__(self, name)
        except AttributeError:
            val = object.__getattribute__(self, name)
            
        if name == 'team':
            AgentModel._update_team_object(self)
                    
        return val

    def pos(self, xyz=None):
        if xyz is None:
            return self.state[0:3]
        else:
            self.x, self.y, self.z = xyz

    def update_smoothed_position(self, ticks):
        self.update_smoothing(ticks)

    def smoothed_position(self):
        return self.smoothed()

    def update_interpolated_prediction_position(self, ticks):
        return self.update_interpolated_prediction(ticks)

    def interpolated_prediction_position(self):
        return self.client_side_prediction_interpolated()

    def update_prediction_position(self, ticks):
        return self.update_prediction(ticks)

    def prediction_position(self):
        return self.client_side_prediction()

    def server_snapshot_position(self):
        return self.last_snapshot()
            
    def camera_position(self):
        if opts.agent_motion == 'normal':
            p = self.state[0:3]
        elif opts.agent_motion == 'smoothed':
            p = self.smoothed_position()
        elif opts.agent_motion == 'interpolated_prediction':
            p = self.interpolated_prediction_position()
        elif opts.agent_motion == 'prediction':
            p = self.prediction_position()
        elif opts.agent_motion == 'server_snapshot':
            p = self.server_snapshot_position()
        
        p[2] += self.camera_height
        return p

    def velocity(self):
        return self.state[3:6]

    def forward(self):
        return vector_lib.forward_vector(self.x_angle)

    def upward(self):
        return [0,0,1]

    def listener_state(self):
        return [self.pos(), self.velocity(), self.forward(), self.upward()]

    def direction(self, normalize=True):
        v = vector_lib.angle2vector(self.x_angle, self.y_angle)
        if normalize:
            v = vector_lib.normalize(v)
        return v

    def angles(self):
        return [self.x_angle, self.y_angle]

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

    def set_velocity(self, v_x, v_y):
        self.v_x = v_x
        self.v_y = v_y

    @property
    def state(self):
        return [self.x, self.y, self.z, self.vx, self.vy, self.vz, self.ax, self.ay, self.az]
    @state.setter
    def state(self, val):
        self.x, self.y, self.z, self.vx, self.vy, self.vz, self.ax, self.ay, self.az = val

    def normalized_direction(self):
        vec = vector_lib.angle2vector(self.x_angle, self.y_angle)
        return vector_lib.normalize(vec)

# represents an agent under control of a player
class Agent(AgentModel, AgentPhysics, AgentRender, AgentVoxRender):

    def __init__(self, owner=None, id=None, state=None, weapons=None, health=None, dead=False, items=None, team=None):
        #self.init_vox()
        AgentModel.__init__(self, owner, id, state, health, dead, team)
        AgentVoxRender.__init__(self)
        print 'id %s' % (self.id,)
        print self
        self.inventory = AgentInventory(self, items)
        self.weapons = AgentWeapons(self, weapons)




#from profiler import P


'''
Client's player's agent draw methods
'''
class PlayerAgentRender(AgentRender):


    def draw(self):
        self.draw_aiming_direction()
        self.draw_bounding_box()
        self.update_vox()
        self.draw_vox()

        dx = cos( self.x_angle * pi) * cos( self.y_angle * pi)
        dy = sin( self.x_angle * pi) * cos( self.y_angle * pi)
        dz = sin( self.y_angle)
        max_l = 20
        pos = c_lib._ray_trace.ray_cast6(self.x,self.y,self.z, dx,dy,dz, max_l)
        if pos != None:
            pass
            #print str(pos)
            (x,y,z, px,py,pz, sx,sy,sz) = pos
            #cube selected
            r,g,b = 150,150,150
            c_lib.c_lib_agents._draw_agent_cube_selection(px,py,pz, r,g,b)
            #cube side selected
            r,g,b = 0,0,255
            c_lib.c_lib_agents._draw_agent_cube_side_selection(x,y,z, sx,sy,sz, r,g,b)

    def draw_position(self, points, seperation):
        print "agents.py draw position deprecated"
        return

    def draw_velocity(self, point_density, units):
        print "agents.py draw velocity deprecated"
        return

    def draw_acceleration(self, point_density, units):
        print "agents.py draw acceleration deprecated"
        return

    def draw_selected_cube(self):
        print "agents.py draw_selected_cube deprecated"
        return


    def draw_selected_cube2(self, distance = 20):
        print "agents.py draw_selected_cube2 deprecated"
        return


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
class PlayerAgent(AgentModel, AgentPhysics, PlayerAgentRender, AgentVoxRender, PlayerAgentWrapper):

    def __init__(self, owner=None, id=None, state=None, weapons=None, health=None, dead=False, items=None, team=None):
        AgentModel.__init__(self, owner, id, state, health, dead, team)
        PlayerAgentWrapper.__init__(self, id)
        self._control_state_id_set = False

        #if id:
            #self.id = id

        self.weapons = PlayerAgentWeapons(self, weapons)
        self.inventory = PlayerAgentInventory(self, items)

        self.you = True

        self.vx = 0
        self.vy = 0
        self.vz = 0
        self.ax = 0
        self.ay = 0
        self.az = 0

        self.camera = None
        
        AgentVoxRender.__init__(self)

    def __setattr__(self, name, val):
        self.__dict__[name] = val
        if name == 'id':
            set_player_agent_id(val)
            self._control_state_id_set = True

    def __getattribute__(self, name):
        try:
            val = AgentWrapper.__getattribute__(self, name)
        except AttributeError:
            try:
                val = PlayerAgentWrapper.__getattribute__(self, name)
            except AttributeError:
                val = object.__getattribute__(self, name)

        if name == 'team':
            AgentModel._update_team_object(self)

        return val

    def set_button_state(self):
        if not self._control_state_id_set:
            return
        if self.camera is None:
            return

        f,b,l,r, jump, jet, crouch, boost, misc1, misc2, misc3 = self.button_state
        theta, phi = self.camera.angles()
        set_agent_control_state(f,b,l,r, jet, jump, crouch, boost, misc1, misc2, misc3, theta, phi)

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

        if weapon is not None:
            weapon.animation(agent=self).play()

        # check agent
        ignore_vox = []
        (ag, adistance, vox) = vox_lib.hitscan2(self.x,self.y,self.z,self.x_angle, self.y_angle, ignore_vox=ignore_vox)
        print ag, adistance, vox
        body_part_id = 1
        block = ray_tracer.nearest_block(self.camera_position(), self.camera.forward())
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

        # short circuit shooting voxel items
        # can add 'item' ttype to API later
        if ttype == 'agent' and not isinstance(ag, Agent):
            ttype = 'empty'
            loc = self.normalized_direction()

        if ttype == 'agent':
            if ag.team == self.team and not GameStateGlobal.game.team_kills:
                return
            ag.bleed()

        # determine target w/ ray cast
        #target = ray_cast_from(agent)
        target = {
            'type'  :   ttype,
            'loc'   :   loc
        }
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
        if self.camera is None:
            return
        if block_type is None:
            block_type = self.facing_block()
            print "Gonna set block type to", block_type
        if not block_type:
            return
        InputGlobal.cube_selector.active_id = block_type

    def active_block(self):
        return InputGlobal.cube_selector.active_id

    def facing_block(self):
        block = self.nearest_block_position()
        if block is None:
            return None
        block = terrainMap.get(*block)
        return block

    def facing_block_position(self):
        if self.camera is None:
            return
        return ray_tracer.farthest_empty_block(self.camera_position(), self.camera.forward())

    def nearest_block_position(self):
        if self.camera is None:
            return
        return ray_tracer.nearest_block(self.camera_position(), self.camera.forward())

    def pickup_item(self, item, index=None):
        if self.team.is_viewers():
            return
        if self.inventory.can_add(item):
            NetOut.sendMessage.pickup_item(self, item, index)

    def drop_item(self, item):
        if self.team.is_viewers():
            return
        if self.inventory.can_drop(item):
            NetOut.sendMessage.drop_item(self, item)

    def nearby_objects(self):
        for obj in GameStateGlobal.itemList.values():
            if vector_lib.distance(self.pos(), obj.pos()) < obj.radius:
                self.pickup_item(obj)
                if obj.proximity_effect:
                    NetOut.sendMessage.near_item(self, obj)

