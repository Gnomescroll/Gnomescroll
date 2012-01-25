'''
Agents:
    Objects through which a Player experiences the game world
'''
import opts
opts = opts.opts

from game_state import GameStateGlobal
from object_lists import GenericObjectList
from net_out import NetOut

import c_lib.c_lib_agents as cAgents
import c_lib.c_lib_game_modes as cGame

# datastore controller for agents
class AgentList(GenericObjectList):

    def __init__(self):
        GenericObjectList.__init__(self)
        self._object_type = Agent
        self._wrapper = cAgents.AgentListWrapper

    def create(self, client_id):
        return self._add(client_id)

    def destroy(self, agent):
        id = self._remove(agent)
        return id

class Agent(cAgents.AgentWrapper):

    def __init__(self, client_id):
        cAgents.AgentWrapper.__init__(self, client_id)
        cAgents.AgentWrapper.send_id_to_client(self, client_id)

    def __getattribute__(self, name):
        try:
            val = cAgents.AgentWrapper.__getattribute__(self, name)
        except AttributeError:
            val = object.__getattribute__(self, name)

        if name == 'team':
            val = cGame.get_team(val)

        return val
