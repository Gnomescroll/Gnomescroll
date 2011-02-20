import Queue
from threading import Thread
import redis.client
import sys
import simplejson

#from incoming.message_handlers import Message_handlers

#need to subsribe and unsubscribe in the future	
class Server_listener:
	def __init__(self):
		self.globals = None
		self.message_handlers = None

	def start(self):
		t = Thread(target=self.message_worker, args= (self.globals.world_id,))
		t.daemon = True
		t.start()

	def message_worker(self, world_id):
		print "Message Worker Started"
		r_client = redis.Redis("localhost") #will eventually need multiple output buffer servers
		key = "world_%s_out" % (world_id)
		r_client.subscribe(key)
		
		for m in r_client.listen():
			try:
				pattern = m['pattern']
				type = m['type']
				channel = m['channel']
				i = m['data']
				
				if type == 'message':
					print str(i)
					i = simplejson.loads(i)
					msg = i['msg']
					#print str(i)
					if msg in self.message_handlers.handlers.keys():
						print "valid"
						self.message_handlers.handlers[msg](**i)
					else:
						print "msg does not exist in handler: " + str(msg)
				#print str(m)
				#(pattern, subscribe, channel, msg) = m
				#print "listener: " + str(msg)
				
			except Exception, err:
				print "message_worker: %s: %s" %(sys.stderr, err)
				print "crash msg = " + str(i)
				continue
