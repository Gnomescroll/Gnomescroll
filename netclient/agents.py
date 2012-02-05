'''
Agents:
    Objects through which a Player experiences the game world
'''

import opts
opts = opts.opts


import init_c_lib
import c_lib.terrain_map as terrainMap

from game_state import GameStateGlobal
from net_out import NetOut
from input import InputGlobal
import camera
from object_lists import GenericObjectList, GenericObjectListWrapper

'''
Data model for agent
'''
class Agent(init_c_lib.AgentWrapper):

    def __init__(self, id):
        self.id = id
        self.client_id = id
        self.you = False
        print 'Python Agent creation: id %s' % (self.id,)

    def update_info(self, **agent):
        pass

    def __getattribute__(self, name):
        try:
            val = init_c_lib.AgentWrapper.__getattribute__(self, name)
        except AttributeError:
            val = object.__getattribute__(self, name)
            
        return val

# decorators
def noViewer(f):
    def outer(*args, **kwargs):
        self = args[0]
        if self.team is not None and not self.is_viewer():
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
class PlayerAgent(Agent, init_c_lib.PlayerAgentWrapper):

    def __init__(self, id):
        Agent.__init__(self, id)
        init_c_lib.PlayerAgentWrapper.__init__(self, id)

        self.button_state = [0 for i in range(11)]

        self.you = True
        self.camera = None

    def __getattribute__(self, name):
        try:
            val = init_c_lib.AgentWrapper.__getattribute__(self, name)
        except AttributeError:
            try:
                val = init_c_lib.PlayerAgentWrapper.__getattribute__(self, name)
            except AttributeError:
                val = object.__getattribute__(self, name)

        return val

    def is_viewer(self):
        return self.team == 0

    @requireCamera
    def set_button_state(self):
        f,b,l,r, jump, jet, crouch, boost, misc1, misc2, misc3 = self.button_state
        theta, phi = self.camera.angles()
        init_c_lib.set_agent_control_state(f,b,l,r, jet, jump, crouch, boost, misc1, misc2, misc3, theta, phi)

    def active_block(self):
        return InputGlobal.cube_selector.active_id

    #@noViewer
    #def switch_weapons(self, weapon_index):
        #if weapon_index == 'up':
            #i = -1
        #elif weapon_index == 'down':
            #i = -2
        #else:
            #i = weapon_index-1

        #old = self.active_weapon
        #init_c_lib.PlayerAgentWrapper.switch_weapon(self, i)
        #if old != self.active_weapon:
            #self.camera.unzoom()

    def hud_equipment_slot(self):
        return self.active_weapon

# datastore for agents
class AgentList(GenericObjectListWrapper):

    def __init__(self):
        GenericObjectListWrapper.__init__(self)
        GenericObjectList.__init__(self)
        self._metaname = 'AgentList'
        self._itemname = 'Agent'
        self._object_type = Agent
        self._wrapper = init_c_lib.AgentListWrapper

    def create(self, *args, **agent):
        a = self._add(*args, **agent)
        return a

    def by_client(self, id):
        print self.values()
        for agent in self.values():
            if id == agent.client_id:
                return agent

    def create_player_agent(self, *args, **agent):
        if agent['id'] in self and not isinstance(self[agent['id']], PlayerAgent):
            #remove Agent created during a C update, replace with a player agent
            self._remove(agent['id'])
        self._object_type = PlayerAgent
        player_agent = self._add(*args, **agent)
        self._object_type = Agent
        print "Created python player agent", player_agent,player_agent.id
        GameStateGlobal.agent = player_agent
        # switch from camera to agent input
        init_c_lib.cy_input_state.input_mode = 0
        init_c_lib.cy_input_state.camera_mode = 0
        return player_agent

    def destroy(self, agent):
        self._remove(agent)
        return agent

    def load_list(self, objs):
        _objs = []
        for obj in objs:
            _objs.append(self.create(**obj))
        return _objs

    def update_from_c(self):
        ids = self._wrapper.ids()
        for id in ids:
            if id not in self:
                self.create(**{'id':id})
                added.append(id)

        for id in self:
            if id not in ids:
                self.destroy(id)
                
    def check_for_player_agent(self):
        self.update_from_c()
        for id in self._wrapper.ids():
            if id == init_c_lib.get_player_agent_id():
                return self.create_player_agent(**{'id':id})
        
    def values(self):
        self.update_from_c()
        return GenericObjectListWrapper.values(self)

