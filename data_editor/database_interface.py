import redis
import ast

## Public Interface

# get_object_list(type)
# add_to_index(type, id)
# remove_from_index(type, id)
# delete_object(type, id)

# dict_from_redis(id, object_type)
# dict_from_redis_by_key(object_key)

# dict_to_redis(type, id, input_dict):

def get_redis_client():
	r = redis.Redis('localhost')
	###!!! set database !!!###
	return r

### KEY computation functions ### 

""" Abstracts Repeated Code """
def get_index_key(otype):
	assert type(otype) == type("d ")
	return str(otype)+":index"

def get_index_counter_key(otype):
	assert type(otype) == type("d ")
	return str(otype)+":index_counter"

def get_object_key(otype, object_id):
	assert type(otype) == type("d ")
	assert type(object_id) == type(0)
	return str(otype) +":"+str(object_id)

def deleted_objects_key(otype, object_id):
	assert type(otype) == type("d ")
	assert type(object_id) == type(0)
	return str(otype) +":"+str(object_id)
	

### OBJECT INDEX manipulation interface ###

#do garabage detection/recycling eventually	
def get_free_id(object_type):
	r = get_redis_client()
	object_id =  r.incr(get_index_counter_key(object_type))
	assert type(object_id) == type(0)
	return object_id
	
def get_object_list(object_type):
	r = get_redis_client()
	index_key = get_index_key(object_type)
	key_list = r.smembers(index_key)
	return key_list
	
def add_object_to_index(object_type, object_id):
	r = get_redis_client()
	index_key = get_index_key(object_type)
	object_key = get_object_key(object_type, object_id)
	r.sadd(index_key, object_key)

def remove_object_from_index(object_type, object_id):
	r = get_redis_client()
	index_key = get_index_key(object_type)
	object_key = get_object_key(object_type, object_id)
	r.srem(index_key, object_key)

""" Stuff """ 

## Should never delete objects, only mark as deleted ##
def delete_object(type, object_id):
	r = get_redis_client()
	index_key = get_index_key(type)
	object_key = get_object_key(type, object_id)
	r.srem(index_key, object_key) #remove from index
	r.delete(object_key) #delete object data
	r.sadd(deleted_objects_key, object_key) #add to deleted keys (can be reclaimed later)
	
	
### DICTIONARY SERIALIZATION ###

#import pprint
#print "---"
#pp = pprint.PrettyPrinter(indent=4)
#pp.pprint(output_dict)	

import string 
import ast

def dict_to_redis(object_type, object_id, input_dict):
	map_dict = input_dict
	r = get_redis_client()
	object_key = get_object_key(object_type, object_id)
	r.hmset(object_key, map_dict)
	add_object_to_index(object_type, object_id)

def dict_from_redis(object_type, object_id):
	object_key = get_object_key(object_type, object_id)
	return dict_from_redis_by_key(object_key)

def dict_from_redis_by_key(object_key):
	r = get_redis_client()
	output_dict = r.hgetall(object_key)
	for k, v in output_dict.items():
		if k == "id":
			output_dict[k] = int(v)
		elif k == "name" or k == "type":
			output_dict[k] = str(v)
		else:
			output_dict[k] = ast.literal_eval(v)
	return output_dict

def raw_dict_from_redis(object_type, object_id):
	object_key = get_object_key(object_type, object_id)
	return raw_dict_from_redis_by_key(object_key)

def raw_dict_from_redis_by_key(object_key):
	r = get_redis_client()
	return r.hgetall(object_key)

### Name to ID pairing

def associate_name_with_id(object_type, object_id, name):
	pass ##IMPLEMENT
