'''
Agents:
    Objects through which a Player experiences the game world
'''

import init_c_lib

from game_state import GameStateGlobal
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

    def __getattribute__(self, name):
        try:
            val = init_c_lib.AgentWrapper.__getattribute__(self, name)
        except AttributeError:
            val = object.__getattribute__(self, name)
            
        return val

'''
Client's player's agent
'''
class PlayerAgent(Agent, init_c_lib.PlayerAgentWrapper):

    def __init__(self, id):
        Agent.__init__(self, id)
        init_c_lib.PlayerAgentWrapper.__init__(self, id)
        self.you = True

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

