import sys
import random

from interface.object import Nobject
from interface.crop import Crop

#custom_agent_attributes = ['position'] #attributes with class defined get/set properties
custom_agent_attributes = []

class Agent:

	def move_0(self,dx,dy,dz):
		[type, x, y, z] = self.position
		[type, x, y, z] = [type, x+dx, y+dy, 0]
		self.position = [type, x, y, z]
		self.delta.agent_position_change(self.__dict__['id'], [0,x, y, z])

	def attack(self, target_id, attack_type = 'default'):
		#agent_id = self.id()
		agent_position = self.position
		target = Agent(target_id)
		target_position = target.position
		
		#check to see if agents are within attack range of each other
		if not target_position[3] == agent_position[3]:
			print "Agent Attack Fail: agents are on different z-levels"
			return
		if not -1 <= target_position[1] - agent_position[1] <= 1:
			print "Agent Attack Fail 1: agents are too far away"
			return
		if not -1 <= target_position[2] - agent_position[2] <= 1:
			print "Agent Attack Fail 2: agents are too far away"
			return
		
		#get attack information for agent	
		if not attack_type in self.attacks.values():
			attack_type = 'default'
		attack = self.attacks[attack_type]
		damage = 0
		for x in range(0, attack['damage'][0]):
			damage = damage + random.randint(0, attack['damage'][1])
		print "Attack: " + str(self.id) + " " + str(target_id) + " " + str(damage)
		
		target.health = target.health - damage
		message = "Attack: " + str(self.id) + " " + str(target_id) + " " + str(damage)
		self.delta.attack_log(self.id, target_id, damage, message)
		
		if target.health < 0:
			print "Agent " + str(target.id) + " is dead"
		
	def pickup_item(self, item_id):
		if self.holding == 1:
			print "Agent cannot hold multiple items!"
		else:
			item = Nobject(item_id)
			if self.position != item.position:
				print "Agent is too far from item!"
			else:
				item.position = (1, self.__dict__['id'])
				self.holding = item.id
				#update message
				type = 'agent_pickup_item'
				meta = {
				type : 'agent_pickup_item',
				agent_id : self.id(),
				item_id : item.id,
				position : self.position
				}
				self.delta.meta(type, meta)
				#meta = { type : "pickup_item", item_id: item.id, agent_id: self.__dict__['id'] }
				#self.delta.agent_state_change(self.__dict__['id'], self.position, self.verion, meta = meta )
				#meta = {type : 'drop_item'}
				#self.object_position_change(self.holding, self.position, meta=meta)
		pass

	def drop_item(self):
		if self.holding == 0:
			print "Agent is not holding anything!"
		else:
			item = Nobject(self.holding)
			item.position = self.position
			self.holding = 0
			#update message
			type = 'agent_drop_item'
			meta = {
			agent_id : self.id(),
			item_id : item.id,
			position : self.position
			}
			self.delta.meta(type, meta)			
			#self.delta.agent_state_change(self.__dict__['id'], self.position, self.version, meta = meta)
			#meta = {type : 'drop_item'}
			#self.object_position_change(self.holding, self.position, meta=meta)
			self.holding = 0
		pass

	#action will be wall, ditch, etc...
	def dig(self, position, action):
		if action == "wall":
		#	print "0: " + str(self.position)
		#	print "1: " + str(position)

			(type_a, x_a, y_a, z_a) = self.position
			(type_b, x_b, y_b, z_b) = position
			if not -1 <= x_a - x_b <=1:
				print "Agent Dig Error: Agent is too far away!"
				return
			if not -1 <= y_a - y_b <=1:
				print "Agent Dig Error: Agent is too far away!"
		#		print "2"
				return
			if not -1 <= z_a - z_b <=1:
				print "Agent Dig Error: Agent is too far away!"
		#		print "3"
				return
				
			tile_value = self.world_map.get(x_b, y_b ,z_b)
		#	print "tv: " + str(tile_value)
			tile_dict = self.dat.get_tile_by_value(tile_value)
			
			(type, x, y, z) = position
			if tile_dict['dig'] == 0:
				print "tile_type cannot be dug: " + str(tile_value)
			else:
		#		print "2: " + str(tile_dict)
				dig_into = tile_dict['dig_into']
				dig_into = self.dat.get_tile_by_name(dig_into)
				self.world_map.set(x,y,z, dig_into['id'])
				#roll for items
				dig_produces = tile_dict['dig_produces']
				for n in dig_produces:
					self.objects.create([0, x, y, z], 'item', n[2])
				
		if action == "ramp":
			print "Ramps not implemented yet!"
			return
		
	def till_soil(self):
		(type, x, y, z) = self.position
		tile_value = self.world_map.get(x,y,z)
		tile_dict = self.dat.get_tile_by_value(tile_value)
		
		if tile_dict['tillable'] != 1:
			print "tile_type cannot be tilled: " + str(tile_value)
		else:
			till_into = tile_dict['till_into']
			till_into = self.dat.get_tile_by_name[till_into]
			self.world_map.set(x,y,z, till_into['id'])

	def plant_crop(self, seed_id = 0):
		#pretend they have a seed
		(type, x, y, z) = self.position
		tile_value = self.world_map.get(x,y,z)
		if not tile_value == 1:	#check to see if tile can support crops and if this seed will grow
			print "Tile or seed type invalid"
			return
		#from pudb import set_trace; set_trace()
		self.objects.create(self.position, object_type = 'crop', template = 'generic_crop')
		pass
		
	def harvest_crop(self, crop_id = 0):
		if crop_id == 0:
			print "Needs crop id"
			return	#get crop at this location
		crop = Crop(crop_id)
		if self.position != crop.position:
			print "out: " + str(self.position) + " " + str(crop.position)
			print "Agent is too far away from the crop"
			return
		#if conditions are met, harvest
		result = crop.harvest()
		
		if result == 0:
			print "Crop harvest failure!"
			return
		exp = crop.template()['harvest_exp'] 
		#give exp
		pass

	def craft_item(self, agent_id, recipe):
		print "Debug: Start Crafting"
		recipe_d = self.dat.get_crafting_recipe(recipe)
		name = recipe_d['name']
		workshop = recipe_d['workshop']
		reagent = recipe_d['reagent']
		#product = recipe_d['product']
		agent = Agent(agent_id)
		#check to see if agent is standing on a workshop square
		(ptype, x, y, z) = agent.position
		list = self.objects.get_all(x, y, z)
		#print "list: " + str(list)
		#print "0: " + str(workshop)
		workshop_obj = None
		for ob in list:
		#	print "1: " + str(ob['type'])
		#	print "2: " + str(ob['workshop'])
			if 'workshop' in ob['type']:  #types are defined by required fields
		#		print "2: " + str(ob['workshop'])
				if ob['workshop'] == workshop: #workshop type object must have a workshop parameter
					workshop_obj = ob
		if workshop_obj == None:
			print "Crafting Error: required workshop for recipe does not exist on agent square"
			return		
		#compile list of recipe ingrediants on the square
		ingredients_list = []
		#print "4: " + str(reagent)
 		for re in reagent:
			for obj in list:
				if 'item' in obj['type'] and re == obj['name'] and not obj['id'] in ingredients_list:
					ingredients_list.append(obj['id'])			
		if len(ingredients_list) != len(reagent): #checks to see if all the ingrediants exist
			print "Crafting Error: missing reagents"
			print "ingrediants_list: " + str(ingrediants_list)
			print "recipe_d: " + str(recipe_d)
			return
		else:
			print "ingrediants list: " + str(ingredients_list)
		#delete crafting ingrediants
		for obj_id in ingredients_list:
			self.objects.delete(obj_id)
		#create crafting result
		for item_name in recipe_d['product']:
			self.objects.create([0, x, y, z], 'item', item_name)

	def construct_wall(self, position, wall_type):
		(type_a, x_a, y_a, z_a) = self.position
		(type_b, x_b, y_b, z_b) = position
		if not -1 <= x_a - x_b <=1:
			print "Agent construct_wall Error: Agent is too far away!"
			return
		if not -1 <= y_a - y_b <=1:
			print "Agent construct_wall Error: Agent is too far away!"
			return
		if not z_a == z_b:
			print "Agent construct_wall Error: Agent is on wrong z-level!"
			return
		
		#check to see if target square is blocked
		tile = self.dat.get_tile_by_value(self.world_map.get(x_b, y_b, z_b))
		if tile['blocking'] == 1:
			print "Agent construct_wall Error: square is already occupied"
		wall_tile = self.dat.get_tile_by_name(wall_type)
		if not 'buildable' in wall_tile.keys() or wall_tile['buildable'] == 0: #should short-circuit evaluation?
			print "Agent construct_wall Error: this tile cannot be build"
			return
#		if wall_tile['buildable'] == 0:
#			print "Agent construct_wall Error: this tile cannot be build"
#			return
		
		#check material requirements
		requires = wall_tile['build_requires']
		
		objects = self.objects.get_all(x_b, y_b, z_b, 'item')
		list = []
		
		for req in requires:
			for obj in objects:
				if obj['name'] == req and obj['id'] not in list:
					list.append(obj['id'])
		
		#eventually check to see if there are other items on the square
		if len(list) != len(requires):
			print "Agent construct_wall Error: required items are missing"
			return

		for obj_id in list:
			self.objects.delete(obj_id)
		self.world_map.set(x_b, y_b, z_b)
		
	##internal commands	
	def id(self):
		return self.__dict__['id']
	
	### Generic Stuff ###
	world_map = None
	delta = None
	agents = None
	objects = None
	world_time = None
	dat = None
	
	##custom get/set methods
	#def _get_position(self):
		#return self.position
	#def _set_position(self, position):
		#self.position = position
	#position = property(_get_position, _set_position)

	#override handling
	def __setattr__(self, name, value):
		if name in custom_agent_attributes:
			property = Agent.__dict__[name] #Agent == self.__class__
			property.fset(self, value)
		else:
			self.__dict__['agent_dict'][name] = value

	# does not over right the property methods, unlike setattr
	def __getattr__(self, name):
		#print "oops"
		if name in custom_agent_attributes:
			print "Agent, __getattr__, Should never occur! Revise assumptions"
			property = Agent.__dict__[name]
			value = property.fget(self)
			return value
		else:
			return self.__dict__['agent_dict'][name]

	def __init__(self, id):
		#print "id = " + str(id)
		try:
			id = int(id)
			self.__dict__['agent_dict'] = Agent.agents.agent_list[id]
			self.__dict__['id'] = id
		except Exception, err:
			print "Agent.__init__ , agent id does not exist %s: %s" %(sys.stderr, err)
			
	def serialize(self):
		return self.__dict__['agent_dict']

	#a simple serializer
	def simple_serialize(self):
		msg = {}
		msg['id'] = self.__dict__['id']
		msg['type'] = self.type
		msg['position'] = self.position
		msg['player_id'] = self.player_id
		msg['version'] = self.version
		return msg
