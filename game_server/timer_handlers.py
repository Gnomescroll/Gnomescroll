class Timer_handlers:
	
	def __init__(self):
		pass
		
	def define_handlers(self):
		
		self.agent_handler = {
		'move' : self.info_commands.get_agent,
		'get_object' : self.info_commands.get_object,
		'get_map' : self.info_commands.get_map
		}

	def process_timer_call_back(self, timer_meta):
		type = timer_meta['type']
		if type == "agent"
	
