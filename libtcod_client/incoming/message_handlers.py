from incoming.delta_handler import Delta_handler
from incoming.info_handler import Info_handler

class Message_handlers:
	
	def __init__(self):
		pass
		self.handlers = {
	#delta handling
		#map
		'set_terrain_map' : Delta_handler.set_terrain_map,		
		#agent
		'agent_create' : Delta_handler.agent_create,
		'agent_delete' : Delta_handler.agent_delete,
		'agent_position_change' : Delta_handler.agent_position_change,
		'agent_state_change' : Delta_handler.agent_state_change,
		#object
		'object_create' : Delta_handler.object_create,
		'object_delete' : Delta_handler.object_delete,
		'object_position_change' : Delta_handler.object_position_change,
		'object_state_change' : Delta_handler.object_state_change,
	#info request handling
		#map
		'terrain_map' : Info_handler.terrain_map,
		#gets
		'agent_info' : Info_handler.agent_info,
		'object_info' : Info_handler.object_info,
		#lists
		'agent_list' : Info_handler.get_agent_list,
		'object_list' : Info_handler.get_object_list
}
