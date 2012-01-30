'''
Agents:
    Objects through which a Player experiences the game world
'''
import opts
opts = opts.opts

from game_state import GameStateGlobal
from object_lists import GenericObjectList
from net_out import NetOut

import init_c_lib

# datastore controller for agents
class AgentList(GenericObjectList):

    def __init__(self):
        GenericObjectList.__init__(self)
        self._object_type = Agent
        self._wrapper = init_c_lib.AgentListWrapper

    def create(self, client_id):
        return self._add(client_id)

    def destroy(self, agent):
        id = self._remove(agent)
        return id

class Agent(init_c_lib.AgentWrapper):

    def __init__(self, client_id):
        init_c_lib.AgentWrapper.__init__(self, client_id)
        init_c_lib.AgentWrapper.send_id_to_client(self, client_id)

    def __getattribute__(self, name):
        try:
            val = init_c_lib.AgentWrapper.__getattribute__(self, name)
        except AttributeError:
            val = object.__getattribute__(self, name)

        if name == 'team':
            val = init_c_lib.get_team(val)

        return val
