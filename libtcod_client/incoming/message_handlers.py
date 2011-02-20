from incoming.terrain_map_handler import Terrain_map_handler
from incoming.agent_handler import Agent_handler
from incoming.object_handler import Object_handler

class Message_handlers:

    def __init__(self):
        self.terrain_map_handler = None
        self.agent_handler = None
        self.object_handler = None
        pass

    def start(self):
        self.handlers = {
    #delta handling
        #map
        'set_terrain_map' : self.terrain_map_handler.set_terrain_map,
        'terrain_map' : self.terrain_map_handler.init_terrain_map,
        #agent
        'agent_info' : self.agent_handler.agent_info,
        'agent_list' : self.agent_handler.agent_list,                
        'agent_create' : self.agent_handler.agent_create,
        'agent_delete' : self.agent_handler.agent_delete,
        'agent_position_change' : self.agent_handler.agent_position_change,
        'agent_state_change' : self.agent_handler.agent_state_change,
        #object
        'object_info' : self.object_handler.object_info,
        'object_list' : self.object_handler.object_list,
        'object_create' : self.object_handler.object_create,
        'object_delete' : self.object_handler.object_delete,
        'object_position_change' : self.object_handler.object_position_change,
        'object_state_change' : self.object_handler.object_state_change
    }
