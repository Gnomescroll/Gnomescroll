import sys
import marshal
from threading import Thread
import redis

class Message_listener:
    
    def __init__(self):
        self.message_handlers = None
        self.globals = None
        pass
    
    def start(self):
        t = Thread(target=self.message_worker, args= (self.globals.world_id,))
        t.daemon = True
        t.start()
    
    def single_threaded_start(self):
        print "Starting Server in Single Threaded Mode"
        self.message_worker(self.globals.world_id)
        
    def message_worker(self, world_id):
		debug = 1
		r_in = redis.Redis("localhost") #going to be local host for now
		key = "world_%s" % (str(world_id),)
		i = ''
        
		if debug == 0:
			while True:
				try:
					j = r_in.brpop(key)
					i=marshal.loads(j[1])
					if not i:
						continue
					cmd = i['cmd']
					type = i['type']
					world_id = i['world_id']
					#if self.world_id != world id: #this should not occur
					#    print "world id error"
					if type == "info":
						#print 'info: ',self.message_handlers.info_handler[cmd]
						self.message_handlers.info_handler[cmd](**i)
						pass
					if type == "agent":
						#print 'agent: ',self.message_handlers.agent_handler[cmd]
						self.message_handlers.agent_handler[cmd](**i)
						pass
					if type == "admin":
						#print 'admin: ',self.message_handlers.admin_handler[cmd]
						self.message_handlers.admin_handler[cmd](**i)
						pass
						#print 'msg_to_function',self.handlers.msg_to_function[cmd]
						#self.handlers.msg_to_function[cmd](**i)
						
				except Exception, err:
					print "message_worker: %s: %s" %(sys.stderr, err)
					print "crash msg = " + str(i)
					continue
				
		elif debug == 1:
			while True:
				j = r_in.brpop(key)
				i=marshal.loads(j[1])
				if not i:
					continue
				cmd = i['cmd']
				type = i['type']
				world_id = i['world_id']
				if type == "info":
					self.message_handlers.info_handler[cmd](**i)
				if type == "agent":
					self.message_handlers.agent_handler[cmd](**i)
				if type == "admin":
					self.message_handlers.admin_handler[cmd](**i)
