#interfaces 
from interface.agent import Agent
from interface.object import Nobject
from interface.crop import Crop

#creates an agent from a template
def create_agent(id, x, y, z, template = None, player_id = 0, world_id = 0):
	#if prototype == None:
	#	pass
	a = {}
	a['id'] = id
	a['type'] = ['agent']
	a['name'] = 'generic agent type'
	#position is (type, x, y, z)
	a['position'] = [0, x, y, z]
	a['holding'] = 0
	a['inventory'] = []
	a['health'] = 15
	a['attacks'] = {
	'default' : {
		'name' : 'unarmed',
		'damage' : (1, 3) # 1d3	
#		'damage_type' : 'bludgeon'
		}
	}
	
	a['visual'] = {
				'tilemap' : {
					'tilemap_id' : 0,
					'draw_style' : 1,
					'symbol' : 219,
					'symbol_rgb' : (256, 256, 256), #139-69-19
					'background_rgb' : (0,0,0)
				}
	},
	#meta
	a['player_id'] = player_id
	a['world_id'] = world_id
	a['version'] = 0
	return a
	
class Agents:

	def __init__(self):
		self.globals = None
		self.delta = None
		#
		self.agent_list = {}

	def init(self, world_id =0):
		world_id = self.globals.world_id
		#grab state from persistant store
		pass

	def create(self, x, y, z, player_id=0):
		id = self.globals.get_unique_id()
		a = create_agent(id, x, y, z, player_id=player_id, world_id = self.globals.world_id)
		self.agent_list[id] = a
		self.delta.agent_create(id, [0, x, y, z], player_id=None)

	def delete(self, id):
		print "Delete agent: " + str(id)
		del self.agent_list[id]
		self.delta.agent_delete(id)
