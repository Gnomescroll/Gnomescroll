'''
Agents:
    Objects through which a Player experiences the game world
'''

import opts
opts = opts.opts

import random
import vector_lib
import animations
import c_lib.c_lib_particles
import c_lib.c_lib_agents
import c_lib._ray_trace as ray_tracer
import c_lib.c_lib_agents as cAgents
import c_lib.terrain_map as terrainMap
import c_lib.c_lib_sdl as cSDL
import c_lib.c_lib_hud as cHUD
import c_lib.c_lib_game_modes as cGame

from math import sin, cos, pi
from math import floor, ceil, fabs, pow
from game_state import GameStateGlobal #Deprecate?
from weapons import Pick, BlockApplier
#from game_modes import NoTeam
from c_lib.c_lib_agents import AgentWrapper, PlayerAgentWrapper, AgentListWrapper, set_agent_control_state
from draw_utils import *
from net_out import NetOut
from input import InputGlobal
import camera

class AgentWeapons(object):

    max_weapons = 8

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

    #def __init__(self, owner, id, state=None, health=None, dead=False, team=None):
    def __init__(self, owner, id, state=None, health=None, dead=False):
        if state is None:
            state = [0,0,0,0,0,0,0,0,0]
        state = map(float, state)

        self.collisionDetection = terrainMap.collisionDetection

        self.state = state #position, velocity, acceleration
        self.xa = state[3]
        self.ya = state[4]

        #self.team = team

        self.button_state = [0 for i in range(11)]

        self.id = id

        self.x_int = int(state[0])
        self.y_int = int(state[1])
        self.z_int = int(state[2])

        self.you = False

    def tick(self):
        if not self.dead:
            self._tick_physics()

    def update_info(self, **agent):
        args = []

        if 'id' in agent:
            args.append(self.id)
            self.id = agent['id']

        if 'weapons' in agent:
            self.weapons.update_info(**agent['weapons'])

        if 'inventory' in agent:
            self.inventory.update_info(**agent['inventory'])

        if 'state' in agent:
            state = agent['state']
            if type(state) == list and len(state) == len(self.state):
                self.state = state

        #if 'team' in agent:
            #self.team = agent['team']

        GameStateGlobal.agentList.update(self, *args)

    def __getattribute__(self, name):
        try:
            val = AgentWrapper.__getattribute__(self, name)
        except AttributeError:
            val = object.__getattribute__(self, name)
            
        if name == 'team':
            val = cGame.get_team(val)
                    
        return val

    def pos(self, xyz=None):
        if xyz is None:
            return self.state[0:3]
        else:
            self.x, self.y, self.z = xyz
            
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
class Agent(AgentModel):

    #def __init__(self, owner=None, id=None, state=None, weapons=None, health=None, dead=False, items=None, team=None):
    def __init__(self, owner=None, id=None, state=None, weapons=None, health=None, dead=False, items=None):
        #AgentModel.__init__(self, owner, id, state, health, dead, team)
        AgentModel.__init__(self, owner, id, state, health, dead)
        print 'Python Agent creation: id %s' % (self.id,)
        self.inventory = AgentInventory(self, items)
        self.weapons = AgentWeapons(self, weapons)

class PlayerAgentWeapons(AgentWeapons):

    def __init__(self, agent, weapons=None):
        AgentWeapons.__init__(self, agent, weapons)
        self.set_hud_icons()

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
            camera.camera.unzoom()
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

    def hud_slot(self):
        if self._active_weapon is None:
            return -1
        return self._active_weapon

    def set_hud_icons(self):
        for i in range(self.max_weapons):
            try:
                weapon = self.weapons[i]
                cHUD.Equipment.set_equipment_icon(i, weapon.icon)
            except IndexError:
                cHUD.Equipment.set_equipment_icon(i, 0)

    def update_info(self, **weapon_data):
        ret = AgentWeapons.update_info(self, **weapon_data)
        self.set_hud_icons()
        return ret


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
class PlayerAgent(AgentModel, PlayerAgentWrapper):

    #def __init__(self, owner=None, id=None, state=None, weapons=None, health=None, dead=False, items=None, team=None):
    def __init__(self, owner=None, id=None, state=None, weapons=None, health=None, dead=False, items=None):
        #AgentModel.__init__(self, owner, id, state, health, dead, team)
        AgentModel.__init__(self, owner, id, state, health, dead)
        PlayerAgentWrapper.__init__(self, id)

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

    def __getattribute__(self, name):
        try:
            val = AgentWrapper.__getattribute__(self, name)
        except AttributeError:
            try:
                val = PlayerAgentWrapper.__getattribute__(self, name)
            except AttributeError:
                val = object.__getattribute__(self, name)

        if name == 'team':
            val = cGame.get_team(val)

        return val

    def set_button_state(self):
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

        if fire_command == 'hit_block':
            self.hit_block()
        elif fire_command == 'hitscan':
            self.fire_hitscan()
        elif fire_command == 'throw_grenade':
            self.throw_grenade()
        else:
            print "Agent.fire :: unknown command %s" % (fire_command,)

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
            print "Setting active block type to ", block_type
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

    def tick(self):
        AgentModel.tick(self)
        PlayerAgentWrapper.tick(self)
