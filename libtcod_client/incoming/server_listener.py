import Queue
from threading import Thread
import redis.client
import json
import sys

from incoming.message_handlers import Message_handlers

def message_worker():
	print "Message Worker Started"
	r_client = redis.Redis("localhost") #will eventually need multiple output buffer servers
	world_id = 0
	key = "world_%s_out" % (world_id)
	r_client.subscribe(key)
	
	for m in r_client.listen():
		try:
			print str(m)

		except Exception, err:
			print "message_worker: %s: %s" %(sys.stderr, err)
			print "crash msg = " + str(i)
			continue
		
class Server_listener:
	def __init__(self):
		t = Thread(target=message_worker, args=())
		t.daemon = True
		t.start()
