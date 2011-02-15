import redis
from marshal import dumps
import marshal
import sys
import json

class Admin_commands:
	
	def __init__(self):
		r_client = redis.Redis('localhost')

	#sends message to server
	def send_message(self,msg, world_id = 0):
		world_id = msg['world_id']
		this.r_client.lpush("world_"+str(world_id), dumps(msg))

	# required = ['position']
	# optional = ['player_id']
	## command for creating a new agent at position (x,y,z)
	def create_agent(self, x, y, z=0):
		#create msg to be sent to server
		msg = {}
		msg['world_id'] = 0
		msg['position'] = (0,x,y,z) #position is a 4, tuple (position_type, x, y, z)
		#send message to server
		send_message(msg)
	
class Agent_commands:
	
	def __init__(self):
		r_client = redis.Redis('localhost')

	def send_message(self,msg, world_id = 0):
		world_id = msg['world_id']
		this.r_client.lpush("world_"+str(world_id), dumps(msg))

	def create_agent(self, x, y, z=0):
		#create msg to be sent to server
		msg = {}
		msg['world_id'] = 0
		msg['position'] = (0,x,y,z) #position is a 4, tuple (position_type, x, y, z)
		#send message to server
		send_message(msg)
