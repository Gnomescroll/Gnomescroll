

#creates an agent from a template
def create_agent(id, x, y, z, template = None, player_id = 0, world_id = 0):
	#if prototype == None:
	#	pass
	a = {}
	a['id'] = id
	a['types'] = ['agent']
	a['name'] = 'generic agent type'
#	a['position'] = {}
#	a['position']['x'] = x
#	a['position']['y'] = y
#	a['position']['z'] = z
#	a['position']['type'] = 'ground'
	a['x'] = x
	a['y'] = y
	a['z'] = z
	a['position_type'] = 'ground' #position type
	a['player_id'] = player_id
	a['world_id'] = world_id
	a['version'] = 0
	#a['version'] = 0 #versioning counter
	
class Agents()

	def __init__(self):
		agent_list = {}

	def init(self, world_id =0):
		world_id = self.world_id
		#grab state from persistant store
		pass

	def create(self, x, y, z=0, player_id=0):
		id = self.unique_id
		self.unique_id = self.unique_id + 1
		a = create_agent(id, x, y, z, player_id=player_id, world_id = self.world_id)
		agent_list[id] = a
		self.delta.agent_create(id, x, y, z, player_id=None):
		
	def delete(self, id):
		pass
