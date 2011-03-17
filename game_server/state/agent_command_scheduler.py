from interface.agent import Agent

class Agent_command_scheduler:
	
	def __init__(self):
		self.globals = None
		self.world_time = None
		print "1"
		self.last_time = 0
		print "2"
		self.next_move = {}
		self.time_slot = {}

	def execute(self, agent_id, agent_command, params):
		#from pudb import set_trace; set_trace()
		#debug
		if not agent_id in self.next_move:	#execute immediately
			agent = Agent(agent_id)
			agent_command(agent, *params)
			self.schedule(agent_id, self.last_time + agent.action_delay())
			#set agent_next_move time
		elif self.next_move[agent_id] <= self.last_time: #execute immediately
			print "agent_command_scheduler, execute: this should never happen, race condition or off by one error"
			agent = Agent(agent_id)
			agent_command(agent, *params)
			self.schedule(agent_id, self.last_time + agent.action_delay())
			#next_move[agent_id] = self.last_time + agent.action_delay() #set agent_next_move time
		else: #pass command to agent controller
			print "need to implement scheduler!"
			pass
			
	def time_step(self, current_time):
		#current_time = self.world_time.time
		if self.last_time == 0:
			self.last_time = current_time

		## Debug ##
		
		debug = 0
		if debug == 1:
			if self.last_time - current_time == 0:
				print "agent_command_scheduler, time_step: wtf #1"
				#this may happen if timer thread is run more than one per click
			if current_time - self.last_time > 1:
				print "agent_command_scheduler, time_step: lagg, clicks are being skipped, fix this: " + str(current_time - self.last_time)
			else:
				print "working"
			## End Debug ##

		for t in range(self.last_time, current_time):
			if t in self.time_slot:
				for agent_id in self.time_slot[t]:
					value = self.agent_controller.process_next_action(agent_id)
					if value == 0: #this means no command executed or command execution failure
						del next_move[agent_id] #agent is free to move immediately next move
						##Agent is idle here
					else:
						self.schedule(agent_id, t + agent.action_delay())
						#next_move[agent_id] = t + agent.action_delay() #agent_action delay
				del time_slots[t] #cleanup
		self.last_time = current_time

	def schedule(self, agent_id, time):
		self.next_move[agent_id] = time
		if not time in self.time_slot:
			self.time_slot[time] = [time]
		else:
			self.time_slot[time].append(time)

	def process_next_action(agent_id):
		return 0
