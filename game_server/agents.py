

#creates an agent from a template
def create_agent(id, x, y, z, template = None, player_id = 0, world_id = 0):
	if prototype == None:
		pass
	a = {}
	a['id'] = id
	a['types'] = ['agent']
	a['x'] = x
	a['y'] = y
	a['z'] = z
	a['player_id'] = player_id
	a['world_id'] = world_id
	
class Agents()

	def __init__(self):
		agent_list = {}
		
	def create(self, x, y, z=0, player_id=0):
		id = self.unique_id
		self.unique_id = self.unique_id + 1
		a = create_agent(id, x, y, z, player_id=player_id, world_id = self.world_id)
		
		pass
		
	def delete(self, id):
		pass
