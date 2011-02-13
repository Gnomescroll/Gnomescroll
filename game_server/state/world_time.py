
import time
import heapq
import itertools
from threading import Thread

#import agent

def process_timer_call_back(timer_id, timer_type, id):
	if timer_type == "agent":
		agent = agent.Agent(id)
		agent.handle_timer(timer_id)
#	if timer_type == "plant_maturity":
#		crop = agent.Crop(id)
#		crop.timer(timer_id)

class World_time:

	def __init__(self):
		self.base_time = time.time() #base time for calculating ticks
		self.time = 0 #game time in ticks
		self.next_event_time = float("inf") #3600*3600*3600 #this needs to be large number or there is a bug on init
		self.timer_index = itertools.count(1)
		self.timer_heap = [] #for scheduled events
		self.timers = {}
		
	def start(self):
		#should init intial time from persistant
		t = Thread(target=self.time_worker)
		t.daemon = True
		t.start()
	
	def time_worker(self):
		while True:
			self.update_world_time()
			time.sleep(0.025)

	def get_time(self):
		return self.time

	def update_world_time(self):
		old_time = self.time
		self.time = int(100*(time.time() - self.base_time)) #updates ticks
		print str((int(100*(time.time() - self.base_time)), time.time(), self.base_time))
		print "ticks = " + str(self.time)
		self.check_scheduler()

	def check_scheduler(self):
		while self.timer_heap and self.next_event_time <= self.time:
			(scheduled_time, timer_id) = min(self.timer_heap)
			if scheduled_time > self.time:
				self.next_event_time = scheduled_time
				return
			#remove lowest element on heap
			self.timer_heap.remove((scheduled_time, timer_id))
			timer = self.timers.pop(timer_id, (0, 0, 0, 0))
			(scheduled_time, timer_id, timer_type, id) = timer
			if timer_id == 0:
				#print "Deleted Timer Processed"
				continue
			print str(timer)
			process_timer_call_back(timer_id, timer_type, id)

	def print_time(self):
		print "Time= " + str(self.time)

	def add_timer(self, timer_type, id, ticks):
		timer_id = next(self.timer_index)
		scheduled_time = ticks + self.time
		if scheduled_time < self.next_event_time:
			self.next_event_time = scheduled_time
		timer = (scheduled_time, timer_id, timer_type, id)
		self.timers[timer_id] = timer
		self.timer_heap.append((scheduled_time, timer_id))
		print str(timer)
		return timer_id #lets object know the timer id
	
	def remove_timer(self, timer_id):
		del self.timers[timer_id]
		#deleted timer will remain on the timer_heap, taking up memory until removed
