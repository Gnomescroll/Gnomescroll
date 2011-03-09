class Agents:
	"""Agent object"""
	def __init__(self, agent_id, gx, gy, gz, player_id):
		self.gx = gx
		self.gy = gy
		self.gz = gz
		self.id = agent_id
		self.inventory = []
		self.name = 'Agent '+str(agent_id)
		self.player_id = player_id
		self.previous_player_id = None
		
	def position(self):
		return (self.gx,self.gy,self.gz)
		
	def set_position(self, gx,gy,gz):
		self.gx = gx
		self.gy = gy
		self.gz = gz
		
	def delta_move(self,dx,dy,dz):
		self.gx += dx
		self.gy += dy
		self.gz += dz

	def pickup_item(self, item_id):
		self.inventory.append(item_id)
		
	def drop_item(self, item_id):
		if item_id in self.inventory:
			self.inventory.remove(item_id)
		
	def set_name(self, name):
		self.name = name

	def transfer_owner(self, player_id):
		self.previous_player_id = self.player_id
		self.player_id = player_id
		if self.previous_player_id != self.player_id:
			print 'Somebody stole player_id:', self.previous_player_id, '\'s dwarf!!'

	def player_owns_agent(self, player_id):
		if self.player_id == player_id:
			return True
		else:
			return False
	
	def key(self):
		px = self.gx % 256
		py = self.gy % 256
		cx = (self.gx - px) / 256
		cy = (self.gy - py) / 256
		key = (cx, cy)
		return key
	
	def map_n(self):
		px = self.gx % 256
		py = self.gy % 256
		n = 256*py + px
		return n
		
	def serialize(self):
		serialized = {}
		serialized['agent_id'] = self.id
		serialized['gx'] = gx
		serialized['gy'] = gy
		serialized['gz'] = gz
		serialized['inventory'] = self.inventory
		serialized['player_id'] = player_id
		serialized['previous_player_id'] = previous_player_id
		return serialized