import redis
from marshal import dumps
#import sys
#import json

class Admin_commands:
	
	def __init__(self):
		self.r_client = redis.Redis('localhost')

	#sends message to server
	def send_message(self,msg, world_id = 0):
		world_id = msg['world_id']
		self.r_client.lpush("world_"+str(world_id), dumps(msg))

	# required = ['position']
	# optional = ['player_id']
	## command for creating a new agent at position (x,y,z)
	def create_agent(self, x, y, z=0, player_id = 0):
		#create msg to be sent to server
		msg = {}
		#specify command type and command
		msg['type'] = 'admin'
		msg['cmd'] = 'create_agent'
		#command parameters
		msg['world_id'] = 0
		msg['position'] = (0,x,y,z) #position is a 4, tuple (position_type, x, y, z)
		msg['player_id'] = player_id
		#send message to server
		self.send_message(msg)

	def create_item(self, x, y, z=0):
		msg = {}
		msg['type'] = 'admin'
		msg['cmd'] = 'create_item'
		msg['world_id'] = 0
		msg['position'] = (0,x,y,z) #position is a 4, tuple (position_type, x, y, z)
		self.send_message(msg)		

	def create_object(self, x, y, z, object_type = None, template = None):
		msg = {}
		msg['type'] = 'admin'
		msg['cmd'] = 'create_object'
		msg['object_type'] = object_type
		msg['template'] = template
		msg['world_id'] = 0
		msg['position'] = (0,x,y,z) #position is a 4, tuple (position_type, x, y, z)
		self.send_message(msg)	
	
	def create_crop(self, x, y, z, template = None):
		self.create_object(x,y,z, 'crop', template)
		
	#required ['position', 'value', 'world_id']
	#optional []		
	def set_map(self, x, y, z, value):
		msg = {}
		msg['type'] = 'admin'
		msg['cmd'] = 'set_map'
		msg['world_id'] = 0
		msg['position'] = (0,x,y,z) #position is a 4, tuple (position_type, x, y, z)
		msg['value'] = value
		self.send_message(msg)			
	
class Info_commands:
	
	def __init__(self):
		self.r_client = redis.Redis('localhost')

	def send_message(self,msg, world_id = 0):
		"""Sends message to redis server."""
		world_id = msg['world_id']
		self.r_client.lpush("world_"+str(world_id), dumps(msg))

	#required = ['z', 'client_id', 'world_id']
	#optional = ['z']
	def get_map(self, world_id = 0, z = 0):
		msg = {}	
		msg['type'] = 'info'
		msg['cmd'] = 'get_map'
		msg['world_id'] = world_id
		msg['z'] = z
		msg['client_id'] = 0 #will need this
		self.send_message(msg)

	#required = ['agent_id', 'client_id', 'world_id']
	#optional = []			
	def get_agent(self, id):
		msg = {}	
		msg['type'] = 'info'
		msg['cmd'] = 'get_agent'
		msg['world_id'] = 0
		msg['client_id'] = 0
		msg['agent_id'] = id
		self.send_message(msg)

	#required = ['object_id', 'client_id', 'world_id']
	#optional = []			
	def get_object(self, id):
		msg = {}	
		msg['type'] = 'info'
		msg['cmd'] = 'get_object'
		msg['world_id'] = 0
		msg['client_id'] = 0
		msg['object_id'] = id
		self.send_message(msg)		
		
	#required = ['client_id', 'world_id']
	#optional = []
	def get_agent_list(self, world_id = 0):
		msg = {}	
		msg['type'] = 'info'
		msg['cmd'] = 'get_agent_list'
		msg['world_id'] = world_id
		msg['client_id'] = 0 #will need this
		self.send_message(msg)

	#required = ['client_id', 'world_id']
	#optional = []
	def get_object_list(self, world_id = 0):
		msg = {}	
		msg['type'] = 'info'
		msg['cmd'] = 'get_object_list'
		msg['world_id'] = world_id
		msg['client_id'] = 0 #will need this
		self.send_message(msg)		


class Agent_commands:
	
	def __init__(self):
		self.r_client = redis.Redis('localhost')

	def send_message(self,msg, world_id = 0):
		world_id = msg['world_id']
		self.r_client.lpush("world_"+str(world_id), dumps(msg))

	# required = ['dp, agent_id']
	# optional = ['player_id']
	def move_0(self, agent_id, dx, dy, dz=0):
		msg = {}	
		msg['type'] = 'agent'
		msg['cmd'] = 'move_0'
		msg['world_id'] = 0
		msg['agent_id'] = agent_id
		msg['dp'] = (dx,dy,dz) #position is a 4, tuple (position_type, x, y, z)
		self.send_message(msg)

	# required = ['agent_id', 'target_id']
	# optional = ['target_type', 'attack_type']
	def attack(self, agent_id, target_id, target_type = None, attack_type = None):
		msg = {}	
		msg['type'] = 'agent'
		msg['cmd'] = 'attack'
		msg['agent_id'] = agent_id
		msg['target_id'] = target_id
		msg['world_id'] = 0
		if target_type != None:
			msg['target_type'] = target_type
		if attack_type != None:
			msg['attack_type'] = attack_type
		self.send_message(msg)			
	
	# required = ['position', agent_id']
	# optional = ['action']
	def dig(self, agent_id, x, y, z, action = None):
		msg = {}	
		msg['type'] = 'agent'
		msg['cmd'] = 'dig'
		msg['position'] = (0, x, y, z)
		msg['world_id'] = 0
		msg['agent_id'] = agent_id
		if action != None:
			msg['action'] = action
		self.send_message(msg)		

	#required = ['client_id', 'world_id']
	#optional = []
	def till_soil(self, agent_id):
		msg = {}	
		msg['type'] = 'agent'
		msg['cmd'] = 'till_soil'
		msg['world_id'] = 0
		msg['agent_id'] = agent_id
		self.send_message(msg)		

	#required = ['client_id', 'world_id']
	#optional = []
	def plant_crop(self, agent_id):
		msg = {}	
		msg['type'] = 'agent'
		msg['cmd'] = 'plant_crop'
		msg['world_id'] = 0
		msg['agent_id'] = agent_id
		self.send_message(msg)		
		
	def harvest_crop(self, agent_id, crop_id = 0):
		msg = {}	
		msg['type'] = 'agent'
		msg['cmd'] = 'harvest_crop'
		msg['world_id'] = 0
		msg['agent_id'] = agent_id
		msg['crop_id'] = crop_id
		self.send_message(msg)
	
	def craft_item(self, agent_id, recipe):
		msg = {}
		msg['type'] = 'agent'
		msg['cmd'] = 'craft_item'
		msg['world_id'] = 0
		msg['agent_id'] = agent_id
		msg['recipe'] = recipe
		self.send_message(msg)	

	# required = ['position, agent_id', 'wall_type']
	# optional = []		
	def construct_wall(self, agent_id, x, y, z, wall_type = 'generic_wall'):
		msg = {}
		msg['type'] = 'agent'
		msg['cmd'] = 'construct_walls'
		msg['world_id'] = 0
		msg['agent_id'] = agent_id
		msg['position'] = (0, x, y, z)
		msg['wall_type'] = wall_type
		self.send_message(msg)			
