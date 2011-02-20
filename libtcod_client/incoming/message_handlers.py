from incoming.delta_handler import Delta_handler
from incoming.info_handler import Info_handler

class Message_handlers:
	
	def __init__(self):

		self.handler = {
	#delta handling
		#agent
		'agent_create_update' : Delta_handler.agent_create_update,
		'agent_delete_update' : Delta_handler.agent_delete_update,
		'agent_position_change' : Delta_handler.agent_position_change,
		'agent_state_change' : Delta_handler.agent_state_change,
		#object
		'object_create_update' : Delta_handler.info_commands.get_agent,
		'object_delete_update' : Delta_handler.object_delete_update,
		'object_position_change' : Delta_handler.get_object,
		'object_state_change' : Delta_handler.get_map,
		#map
		'set_map' : Delta_handler.set_map,		
	#info request handling
		#gets
		'agent_info' : Info_handler.agent_info,
		'agent_info' : Info_handler.agent_info
		#lists
		'agent_list' : Info_handler.get_agent_list,
		'object_list' : Info_handler.get_object_list
		#map
		#object message handling
		'move_0' : self.agent_commands.move_0,
		'dig' : self.agent_commands.dig,
		'construct_wall' : self.agent_commands.construct_wall,
		
		#map message handling
		'set_map' : Map_update.set_map,
		'get_map' : Map_update.get_map
		}
