from incoming.agent_update import Agent_update
from incoming.object_update import Object_update
from incoming.map_update import Map_update

class Message_handlers:
	
	def __init__(self):

		self.handler = {
	#delta handling
		#agent
		'agent_create_update' : Agent_update.agent_create,
		'agent_position_change' :Agent_update.agent_position_change,
		'agent_state_change' : Agent_update.agent_state_change,
		#object
		'object_create_update' : Agent_update.info_commands.get_agent,
		'object_position_change' : self.info_commands.get_object,
		'object_state_change' : self.info_commands.get_map,
				
	#info request handling
		'agent_info' : self.info_commands.get_agent_list,
		'agent_info' : self.info_commands.get_object_list
		
		#object message handling
		'move_0' : self.agent_commands.move_0,
		'dig' : self.agent_commands.dig,
		'construct_wall' : self.agent_commands.construct_wall,
		
		#map message handling
		'set_map' : Map_update.set_map,
		'get_map' : Map_update.get_map
		}
