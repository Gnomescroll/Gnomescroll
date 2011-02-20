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
		'set_terrain_map' : Terrain_map_handler.set_terrain_map,
		'terrain_map' : Terrain_map_handler.terrain_map,
		#agent
		'agent_info' : Agent_handler.agent_info,
		'agent_list' : Agent_handler.agent_list,				
		'agent_create' : Agent_handler.agent_create,
		'agent_delete' : Agent_handler.agent_delete,
		'agent_position_change' : Agent_handler.agent_position_change,
		'agent_state_change' : Agent_handler.agent_state_change,
		#object
		'object_info' : Object_handler.object_info,
		'object_list' : Object_handler.object_list,
		'object_create' : Object_handler.object_create,
		'object_delete' : Object_handler.object_delete,
		'object_position_change' : Object_handler.object_position_change,
		'object_state_change' : Object_handler.object_state_change
	}
