import simplejson
import redis

### USE TO SERIALIZE DICTIONARIES ###
import string

def serialize_dictionary(dicta, prefix = "/", temp = None):
	supported_types = [ 'str', 'int', 'tuple'] #types supported/implemented
	if temp == None:
		temp = []
	for key, value in dicta.items():
		vtype = type(value).__name__
		el = prefix + "/" + str(vtype) + ":" + str(key)
		if vtype == 'dict':
			serialize_dictionary(value, el, temp)
		elif vtype in suppored_types:
			#print el + " = " + str(value)
			temp.append((el, str(value) ))
		else:
			print "serialize_dictionary: type not supported!"
	#print str(temp)
	return temp

### USE TO DESERIALIZE
def deserialize_dictionary(lista):
	d = {}
	for (key, value) in lista:
		#print "key = " + key + " , value= " + value
		key = string.lstrip(key, "//")		
		li = string.split(key, '/')
		_atd(li, value, d)		
	return d

def _atd(klist, value, d): #deserialization helper function for a single entry
	local = d	
	for s in klist:
		(typea, keya) = string.split(s, ":")
		if typea == 'dict':
			if not keya in local.keys():
				local[keya] = {}
			local = local[keya]
			#print "local: " + str(type(local)) + ", " + str(local)
		elif typea == 'int':
			local[keya] = int(value)
		elif typea == 'str':
			local[keya] = value
		elif typea == 'tuple':
			local[keya] = eval(value)
		else:
			print "atd type not implemented: " + typea + ", " + str(s)
			

###
	

def tile_redis():
	r = redis.Redis('localhost')
	return r

class Tiles:
	
	prefix = "tiles_"
	id_counter = "tiles_id"
	index = "tile_index"
	
	def __init__(self):
		self.r = redis.Redis('localhost')

	def load(self, id):
		key = self.prefix + str(id)
		self.r.get(key)
		
		
	def save(self, id, value):
		key = self.prefix + str(id)
		data = simplejson.loads(value, encoding = 'utf-8')
		self.r.set(key, data)
	
	def get_id(self):
		
		
	def create_new(self):
		
		
#get all keys		
	def get_all(self, id):
		pass


#backup stuff
	def save_to_dist(self):
		pass
	
	def load_from_disc(self):
		pass
