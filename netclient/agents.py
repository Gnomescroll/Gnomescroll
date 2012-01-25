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
import c_lib.c_lib_animations as cAnimations

from game_state import GameStateGlobal
from net_out import NetOut
from input import InputGlobal
import camera

'''
Data model for agent
'''
class Agent(cAgents.AgentWrapper):

    def __init__(self, id, name='undefined-default'):
        self.id = id
        self.client_id = id
        self.you = False
        self.name = name
        print 'Python Agent creation: id %s' % (self.id,)

    def update_info(self, **agent):
        pass
        #if 'name' in agent:
            #self.name = agent['name']

    def __getattribute__(self, name):
        try:
            val = cAgents.AgentWrapper.__getattribute__(self, name)
        except AttributeError:
            val = object.__getattribute__(self, name)
            
        if name == 'team':
            val = cGame.get_team(val)
                    
        return val

# decorators
def noViewer(f):
    def outer(*args, **kwargs):
        self = args[0]
        if not self.team.viewers:
            return f(*args, **kwargs)
    return outer

def requireCamera(f):
    def outer(*args, **kwargs):
        self = args[0]
        if self.camera is not None:
            return f(*args, **kwargs)
    return outer

'''
Client's player's agent
'''
class PlayerAgent(Agent, cAgents.PlayerAgentWrapper):

    def __init__(self, id, name='playerAgent-undefined-default'):
        Agent.__init__(self, id, name)
        cAgents.PlayerAgentWrapper.__init__(self, id)

        self.button_state = [0 for i in range(11)]

        self.set_hud_icons()
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

    @requireCamera
    def set_button_state(self):
        f,b,l,r, jump, jet, crouch, boost, misc1, misc2, misc3 = self.button_state
        theta, phi = self.camera.angles()
        cAgents.set_agent_control_state(f,b,l,r, jet, jump, crouch, boost, misc1, misc2, misc3, theta, phi)

    @requireCamera
    @noViewer
    def fire(self):
        cAgents.PlayerAgentWrapper.fire(self)

    @requireCamera
    @noViewer
    def set_active_block(self, block_type=None):
        if block_type is None:
            block = ray_tracer.nearest_block(self.camera_position(), self.camera.forward())
            if block is None:
                return
            block_type = terrainMap.get(*block)
        print "Setting active block type to ", block_type

        InputGlobal.cube_selector.active_id = block_type
        cAgents.PlayerAgentWrapper.set_active_block(self, block_type)

    def active_block(self):
        return InputGlobal.cube_selector.active_id

    def bleed(self):
        cAnimations.bleed(self.x, self.y, self.z)

    @noViewer
    def switch_weapons(self, weapon_index):
        if weapon_index == 'up':
            i = -1
        elif weapon_index == 'down':
            i = -2
        else:
            i = weapon_index-1

        old = self.active_weapon
        cAgents.PlayerAgentWrapper.switch_weapon(self, i)
        if old != self.active_weapon:
            camera.camera.unzoom()
        #print 'weapon is: %s' % (self.active_weapon,)

    def hud_equipment_slot(self):
        return self.active_weapon

    def set_hud_icons(self):
        for i in range(4):
            cHUD.Equipment.set_equipment_icon(i, i+1)
