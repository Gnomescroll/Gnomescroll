from interface.agent import Agent
from interface.agent import Nobject

class Agent_script:
	
	def __init__(self):
		self.agent_id = None
		self.script = None
		
	def load_job(self, job_id):
		pass
		
	def next(self):
		pass

class Agent_controller:
	
	def __init__(self):
		self.globals = None
		self.job_manager = None
		
		self.script = {}
		self.state = {}
	
	def load_job(self, id, job_id):
		print "agent_controller, loading job: " + str((id, job_id))
		(script, job_id, sub_job) = self.job_manager.claim_job(agent_id, job_id)
		self.script[id] = script
		self.state[id] = {
		'mode' : 'job_script',
		'job_id' : job_id,
		'sub_job' : sub_job
		}
		
	def next_action(self, id):
		
		if not id in self.state.keys():
			print "No agent_state loaded"
			return 0
		state = self.state[id]
		if state['mode'] == 'job_script':
			print "run job script: " + str(id)
			self.run_job_script(id)
			return 1
		else:
			print "Mode not valid"
			return 0


###
###
### DEPRECATE BELOW LINE
###
###		
	def run_agent_script(self, id):
		pass
	
	def agent_get_job(self, agent_id, job_id):
		self.agent_behavior_state[agent_id]
	
	def next_action_deprecated(self, id):
		if id in self.agent_behavior_state.keys():
			return behavior_state(id)
		else:
			if not(id in self.agent_goals.keys()):
				return 0
			goal = self.agent_goals[id]
			agent = Agent(id)
			
			if(goal['goal'] == 'move'):
				return self.move_goal(id, goal['position'])
			if(goal['goal'] == 'FSM1'):
				return self.FSM1(id, goal)
			if(goal['job'] == 'job'):
				return self.execute_job_scipt(id, goal)
			else:
				print "Agent_controller.next_action: goal type not valid"
				return 0 #stop

	def behavior_state(self, id):
		behavior_state = self.agent_behavior_state[id]
		
		if behavior_state == 'job_script':
			script = self.script[id]
		
	def execute_job_script(self, id, goal):
		pass
		
	def load_job_script(self, agent_id, job_id, **finish):
		pass
	
	### goal implementation
	def move_goal(self, id, position):
		agent = Agent(id)
		(ptype, x, y, z) = agent.position
		(ptype_, x_, y_, z_) = position
		if ptype != 0:
			print "agent_controller.move_goal: ptype error"
			return 0
		dx = 0
		dy = 0
		if x > x_:
			dx = -1
		if x < x_:
			dx = 1
		if y > y_:
			dy = -1
		if y < y_:
			dy = 1
		if dx == 0 and dy == 0:
			return 0
		else:
			agent.move_0(dx, dy, 0)
			return 1 #keep going		

	def command_index():
		
		#symbols *,@,#,$,%
		
		#predicates
		('?agent_holding_item', agent_id, item_id)
		('?item_at_location', item_id, (t, x, y, z))
		('?agent_at_location', agent_id, (t, x, y, z))

		#actions
		('::move', agent_id, position)
		('::pickup_item', agent_id, item_id)
		('::move_item', agent_id, item_id, position)
		
		#atomic agent actions
		(':move', agent_id, dx, dy, dz)
		(':move_item', agent_id, item_id, (t, x, y, z)) #into chests, etc...
		(':drop_item', agent_id, item_id)
		(':pickup_item', agent_id, item_id)

		#flow control commands
		('@label', string)
		('@assert', predicate, label)
		('@jump', label)
		('@if', predicate, label) #same as assert
		('@fail', label) #what do on failure
		
		#job management
		('&pause', string) #failure message
		('&error', string) #failure message

		('&lock_item', item_id) 
		('&unlock_item', item_id)

		#functions
		('%agent_position', agent_id)
		('%item_position', item_id)

		
	def FSM1(self, id, goal):
		active_goal = goal['active_goal']
		
		if not active_goal in goal.keys():
			print "term 0"
			return 0
		
		if goal[active_goal]['goal'] == 'pickup_item':
			#object = Noject(goal[active_goal]['item_id'])
			position = goal[active_goal]['position']
			agent = Agent(id)
			
			(a_type, ax, ay, az) = agent.position
			(type, x, y, z) = position
			if (ax, ay, az) != (x, y, z):
				print str(agent.position) + " " + str(position)
				self.move_goal(id, position)
				return 1
			else:
				print "pickup attempt"
				agent.pickup_item(goal[active_goal]['item_id'])
				if not 'next' in goal[active_goal].keys():
					print "term 1"
					return 0
				else: 
					goal['active_goal'] = goal[active_goal]['next']
					print "term 2"
					return 1
		print "term 3"
		return 0

	### goal creation code
	def create_move_goal(self, id, x, y, z):
		goal = {
		'goal' : 'move',
		'position' : (0, x, y, z)
		}
		self.agent_goals[id] = goal
	
	def create_move_item_goal(self, agent_id, item_id, position):
		agent = Agent(agent_id)
		print "item_id: " + str(item_id)
		item = Nobject(item_id)

		(a_type, a_x, a_y, a_z) = agent.position
		(i_type, i_x, i_y, i_z) = item.position		
		(type, x, y, z) = position

		goal = {
			'goal' : 'FSM1',
			'active_goal' : 1,

#			'type' : "create_move_item_goal",
#			'agent_id' : agent_id,
#			'item_id'  : item_id,
#			'position' : position,

#			0 : {
#				'goal' : 'move',
#				'position' : (0, i_x, i_y, i_z), #move to item position
#				'next' : 1,
#				},
			1 : {
				'goal' : 'pickup_item',
				'item_id' : item_id,
				'position' : (0, i_x, i_y, i_z), #pickup item at position
				'next' : 2,
				},
			2 : {
				'goal' : 'move',
				'position' : position, #move to dropoff position
				'next' : 3,
				},
			3 : {
				'goal' : 'drop_item', #drop item
				'item_id' : item_id,
				'position' : position,			
				},
			}
		
		if i_type != 0:
			print "Need support for moving items which are in containers"
			return

		self.agent_goals[agent_id] = goal
