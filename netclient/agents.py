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
        pass

    def __getattribute__(self, name):
        try:
            val = cAgents.AgentWrapper.__getattribute__(self, name)
        except AttributeError:
            val = object.__getattribute__(self, name)
            
        if name == 'team':
            val = cGame.get_team(val)
                    
        return val
            
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

class PlayerAgentWeapons():

    def __init__(self, agent):
        self.agent = agent
        self.set_hud_icons()

    def switch(self, weapon_index):
        if self.agent.team.viewers:
            return
        if weapon_index == 'up':
            i = -1
        elif weapon_index == 'down':
            i = -2
        else:
            i = weapon_index-1

        old = self.agent.active_weapon
        cAgents.PlayerAgentWrapper.switch_weapon(self.agent, i)
        if old != self.agent.active_weapon:
            camera.camera.unzoom()
        print 'weapon is: %s' % (self.agent.active_weapon,)

    def hud_slot(self):
        return self.agent.active_weapon

    def set_hud_icons(self):
        for i in range(4):
            cHUD.Equipment.set_equipment_icon(i, i+1)

'''
Client's player's agent
'''
class PlayerAgent(AgentModel, cAgents.PlayerAgentWrapper):

    def __init__(self, id=None):
        AgentModel.__init__(self, id)
        cAgents.PlayerAgentWrapper.__init__(self, id)

        self.weapons = PlayerAgentWeapons(self)
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
        cAgents.PlayerAgentWrapper.fire(self)

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
        cAgents.PlayerAgentWrapper.set_active_block(self, block_type)

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
