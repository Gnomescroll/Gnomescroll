
import redis


## Public Interface

# get_object_list(type)
# add_to_index(type, id)
# remove_from_index(type, id)
# delete_object(type, id)

# dict_from_redis(id, object_type, output_dict = {}, meta_info_dict = {})
# dict_to_redis(type, id, input_dict):

def get_redis_client():
	r = redis.Redis('localhost')
	###!!! set database !!!###
	return r

### KEY computation functions ### 

""" Abstracts Repeated Code """
def get_index_key(type):
	return str(type)+":index"

def get_index_counter_key(type):
	return str(type)+":index_counter"

def get_object_key(type, id):
	return str(type) +":"+str(id)

def deleted_objects_key(type, id):
	return str(type) +":"+str(id)
	

### OBJECT INDEX manipulation interface ###

#do garabage detection/recycling eventually	
def get_free_id(object_type):
	r = get_redis_client()
	id =  r.incr(get_index_counter_key(object_type))
	assert type(id) == type(0)
	return id
	
def get_object_list(object_type):
	r = get_redis_client()
	index_key = get_index_key(object_type)
	key_list = r.smembers(index_key)
	return key_list

def add_object_to_index(object_type, id):
	r = get_redis_client()
	index_key = get_index_key(object_type)
	object_key = get_object_key(object_type, id)
	r.sadd(index_key, object_key)

def remove_object_from_index(object_type, id):
	r = get_redis_client()
	index_key = get_index_key(object_type)
	object_key = get_object_key(object_type, id)
	r.srem(index_key, object_key)

""" Stuff """ 

## Should never delete objects, only mark as deleted ##
def delete_object(type, id):
	r = get_redis_client()
	index_key = get_index_key(type)
	object_key = get_object_key(type, id)
	r.srem(index_key, object_key) #remove from index
	r.delete(object_key) #delete object data
	r.sadd(deleted_objects_key, object_key) #add to deleted keys (can be reclaimed later)
	
	
### DICTIONARY SERIALIZATION ###

import pprint
import string 

#def new_dict_to_redis(type, default_dict):
#	id = get_free_id(type)
#	return dict_to_redis(type, id, default_dict)
	
def dict_to_redis(object_type, id, input_dict):
	#serialize
	map_dict = {}
	serialize_dict(input_dict, map_dict)
	#now set database from dictionary
	r = get_redis_client()
	object_key = get_object_key(object_type, id)
	r.hmset(object_key, map_dict)
	add_object_to_index(object_type, id)
	
def serialize_dict(input_dict, map_dict, prefix = ""):
	supported_types = [ 'str', 'int', 'tuple', 'list']
	for key, value in input_dict.items():
		vtype = type(value).__name__
		if vtype == 'dict':
			el = prefix + "/" + str(key)
			serialize_dict(input_dict[key], map_dict, el)
		elif vtype in supported_types:
				el = prefix + "/" + str(key) + ":" + str(vtype)
				map_dict[el] = str(value)
		else:
				print "serialize_dictionary: type not supported! " + vtype + ", " + str(key) + ", " + str(value)




def dict_from_redis_by_id(id, object_type, output_dict = {}, meta_info_dict = {}):
	object_key = get_object_key(id, object_type)
	dict_from_redis_by_key(object_key, object_type, output_dict, meta_info_dict)
	
def dict_from_redis_by_key(key, object_type, output_dict = {}, meta_info_dict = {}):
	output_dict = {}
	meta_info_dict = {}
	r = get_redis_client()
	temp = r.hgetall(object_key)
	#r = get_redis_client()
	#temp = r.hgetall(get_key(id, object_type))
	tlist = []
	for k,value in temp.items():
		(key, type) = string.split(k, ':')
		tlist.append((key, value, type, get_key(id, object_type) + "/" + k))
	for (key, value, type, k) in tlist:
		dict_from_list(k, key, value, type, output_dict, meta_info_dict)
	
	pp = pprint.PrettyPrinter(indent=4)  #Debugging
	pp.pprint(output_dict)
	print "---"
	pp.pprint(meta_info_dict)

def dict_from_list(ref, key, value, type, output_dict, meta_info_dict):
	tlist = string.split(key, "/")
	td1 = output_dict
	td2 = meta_info_dict
	while True:
		keya = tlist.pop(0)
		if len(tlist) != 0:
			if not keya in td1.keys():
				td1[keya] = {}
			if not keya in td2.keys():
				td2[keya] = {}
			td1 = td1[keya]
			td2 = td2[keya]
		elif len(tlist) == 0:
			if type == 'int':
				td1[keya] = int(value)
			elif type == 'str':
				td1[keya] = value
			elif type == 'tuple':
				td1[keya] = eval(value)
			elif type == 'list':
				td1[keya] = eval(value)
			else:
				print "dict_from_list, type not implemented: " + typea + ", " + str(s)
			td2[keya] = (value, type, ref)			
			break

