class Message_handlers:
	
	def __init__(self):
		self.info_commands = None
		self.agent_commands = None
		self.admin_commands = None
		pass
		
	def define_handlers(self):
		
		self.info_handler = {
		'get_agent' : self.info_commands.get_agent,
		'get_object' : self.info_commands.get_object,
		'get_map' : self.info_commands.get_map,
		'get_agent_list' : self.info_commands.get_agent_list,
		'get_object_list' : self.info_commands.get_object_list
		}
		
		self.agent_handler = {
		'move_0' : self.agent_commands.move_0,
		'dig' : self.agent_commands.dig,
		'construct_wall' : self.agent_commands.construct_wall,
		
		'pickup_item' : self.agent_commands.pickup_item,
		'drop_item' : self.agent_commands.drop_item,
		#'plant_tree' : self.agent_actions.plant_tree,
		'till_soil' : self.agent_commands.till_soil,
		'plant_crop' : self.agent_commands.plant_crop,
		'harvest' : self.agent_commands.harvest,
		#'store_item_in_container' : self.agent_actions.store_item_in_container,
		#'remove_item_from_container' : self.agent_actions.remove_item_from_container
		}
		
		self.admin_handler = {
		'create_agent' : self.admin_commands.create_agent,
		'create_item' : self.admin_commands.create_item,
		'create_object' : self.admin_commands.create_object,
		'set_map' : self.admin_commands.set_map
		}
