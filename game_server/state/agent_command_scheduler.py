
class Agent_command_scheduler:
	
	def __init__(self):
		globals = None
		world_time = None

		last_time = 0

		next_move = {}
		time_slot = {}

	def execute(self, agent_id, agent_command, params):
		if not agent_id in next_move:	#execute immediately
			agent = Agent(agent_id)
			agent_command(agent, *params)
			#set agent_next_move time
		elif next_move[agent_id] <= self.last_time: #execute immediately
			print "agent_command_scheduler, execute: this should never happen, race condition or off by one error"
			agent = Agent(agent_id)
			agent_command(agent, *params)
			self.schedule(agent_id, self.last_time + agent.action_delay())
			#next_move[agent_id] = self.last_time + agent.action_delay() #set agent_next_move time
		else: #pass command to agent controller
			print "need to implement scheduler!"
			pass
			
	def time_step(self, current_time):
		if self.last_time = 0:
			self.last_time = current_time

		## Debug ##
		if len(last_time, current_time) == 0:
			print "agent_command_scheduler, time_step: wtf #1"
			#this may happen if timer thread is run more than one per click
		if len(last_time, current_time) > 1:
			print "agent_command_scheduler, time_step: lagg, clicks are being skipped, fix this"
		## End Debug ##

		for t in range(last_time, current_time):
			if t in time_slots:
				for agent_id in time_slots[t]:
					value = self.agent_controller.process_next_action(agent_id)
					if value == 0: #this means no command executed or command execution failure
						del next_move[agent_id] #agent is free to move immediately next move
						##Agent is idle here
					else
						self.schedule(agent_id, t + agent.action_delay())
						#next_move[agent_id] = t + agent.action_delay() #agent_action delay
				del time_slots[t] #cleanup
		
		
	def schedule(self, agent_id, time)
		next_move[agent_id]
		if not time in time_slot:
			time_slot[time] = [time]
		else:
			time_slot[time].append(time)
