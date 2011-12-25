'''
Agents:
    Objects through which a Player experiences the game world
'''

import opts
opts = opts.opts

import c_lib.c_lib_agents as cAgents
import c_lib._ray_trace as ray_tracer
import c_lib.terrain_map as terrainMap
import c_lib.c_lib_hud as cHUD
import c_lib.c_lib_game_modes as cGame

from game_state import GameStateGlobal
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


'''
Data model for agent
'''
class AgentModel(cAgents.AgentWrapper):

    def __init__(self, id):

        self.button_state = [0 for i in range(11)]

        self.id = id
        self.client_id = id

        self.you = False
        self.name = '' # fix later

    def update_info(self, **agent):
        if 'weapons' in agent:
            self.weapons.update_info(**agent['weapons'])

    def __getattribute__(self, name):
        try:
            val = cAgents.AgentWrapper.__getattribute__(self, name)
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
            
    @property
    def state(self):
        return [self.x, self.y, self.z, self.vx, self.vy, self.vz, self.ax, self.ay, self.az]
    @state.setter
    def state(self, val):
        self.x, self.y, self.z, self.vx, self.vy, self.vz, self.ax, self.ay, self.az = val

# represents an agent under control of a player
class Agent(AgentModel):

    def __init__(self, id=None, weapons=None):
        AgentModel.__init__(self, id)
        print 'Python Agent creation: id %s' % (self.id,)
        self.weapons = AgentWeapons(self, weapons)

class PlayerAgentWeapons(AgentWeapons):

    def __init__(self, agent, weapons=None):
        AgentWeapons.__init__(self, agent, weapons)
        self.set_hud_icons()

    def switch(self, weapon_index):
        if self.agent.team.viewers:
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

'''
Client's player's agent
'''
class PlayerAgent(AgentModel, cAgents.PlayerAgentWrapper):

    def __init__(self, id=None, weapons=None):
        AgentModel.__init__(self, id)
        cAgents.PlayerAgentWrapper.__init__(self, id)

        self.weapons = PlayerAgentWeapons(self, weapons)
        self.you = True
        self.camera = None

    def __getattribute__(self, name):
        try:
            val = cAgents.AgentWrapper.__getattribute__(self, name)
        except AttributeError:
            try:
                val = cAgents.PlayerAgentWrapper.__getattribute__(self, name)
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
        cAgents.set_agent_control_state(f,b,l,r, jet, jump, crouch, boost, misc1, misc2, misc3, theta, phi)

    def fire(self):
        if self.team.viewers:
            return

        weapon = self.weapons.active()
        if weapon is None:
            return

        fire_command = weapon.fire()

        if fire_command == 'set_block':
            NetOut.sendMessage.set_block(agent=self)
        elif fire_command == 'hit_block':
            self.hit_block()    # defined in C agent wrapper
        elif fire_command == 'hitscan':
            self.fire_hitscan()
        elif fire_command == 'throw_grenade':
            self.throw_grenade()
        else:
            print "Agent.fire :: unknown command %s" % (fire_command,)

    def reload(self):
        if self.team.viewers:
            return
        weapon = self.weapons.active()
        if weapon is None:
            return
        reload_command = weapon.reload()
        if reload_command:
            NetOut.sendMessage(reload_command, self)

    def set_active_block(self, block_type=None):
        if self.team.viewers:
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
