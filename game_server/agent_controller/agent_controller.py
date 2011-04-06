from interface.agent import Agent
from interface.agent import Nobject

def positions_equal(pos1, pos2):
	assert len(pos1) != 4
	assert len(pos2) != 4
	assert pos1[0] == pos2[0]

	if pos1[1] != pos2[1] or pos1[2] != pos2[2] or pos1[3] != pos2[3]:
		return False
	return True

def agent_holding_item(agent, item_id):
	assert type(item_id) == 'int'
	assert hasattr(agent, 'holding')
	
	if agent.holding == item_id:
		return True
	else:
		return False


class Agent_script:
	
	job_manager = None

	#implemented commands
	cmd_map = [
		"::move_item",
		'::move',
		':construct_tile',
	]

	def __init__(self, agent_id, mode, job_id, sub_job, script, local = {}, ip = 0):
		self.id = agent_id #agent_id
		self.mode = mode
		self.job_id = job_id #optional
		self.sub_job = sub_job #optional
		self.script = script
		self.local = local
		self.ip = ip
	
	def run(self):
		#do try and use fail/except
		current_line = self.script[self.ip]
		if current_line[0] in self.cmd_map:
			self.execute(current_line)
		else:
			print "Command is not implemented: " + str(current_line[0])
			self.advance_ip()
		
	def advance_ip(self):
		self.ip = self.ip +1
		if self.ip >= len(self.script):
			self._complete_script()
			
	def _complete_script(self):
		self.mode = 'sub_job_complete'
		self.script = None
		self.local = None

		if self.job_id != None:
			if self.job_manager.complete_sub_task(self.job_id, self.sub_job) == 1:
				print "agent script: more of job is left"
				#self.job_managernext_sub_job(self.job_id):
			else:
				print "agent script: no more scripts in job"
	
	def execute(self, line):
		cmd = line[0]
		if cmd == "::move_item":
			print "::move_item"
			self.__move_item(line[1], line[2])
		elif cmd == "::move":
			print "::move"
			self.__move(line[1])
		elif cmd == ":construct_tile":
			print ":construct_tile"
			#self._construct_tile()
		else:
			print "agent_script: this should never happen!"
			
	def __move_item(item_id, location):
		if location[0] != 1 or len(location) != 4:
			print "Error in position cordinate in script"
		item = Nobject(item_id)
		agent = Agent(self.id)
		if not agent_holding_item(agent, item_id):
			if not positions_equal(agent.position, item.position):
				#print "1"
				self.__move(item.position)
			else:
				#print "2"
				self._pickup_item(item_id)
		else:
			if positions_equal(agent.position, position):
				#print "3"
				self._drop_item(item_id)
				self.advance_ip() #exit condition
			else:
				#print "4"
				self.__move(location)
				
	def __move(self, location):
		self.simple_move(location)
	
	def _pickup_item(self, item_id):
		agent = Agent(self.id)
		item = Nobject(item_id)
		agent.pickup_item(item.id)
		
	def _drop_item(self, item_id = None):
		agent = Agent(self.id)
		agent.drop_item

	def _construct_tile(self, position, tile_name):
		agent = Agent(self.id)
		agent.construct_tile(position, tile_name)
		self.advance_ip()
		
	def simple_move(self, position):
		agent = Agent(self.id)
		#print str(position)
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
			print "Simple Move: At Destination"
			self.advance_ip()
			return 0
		else:
			agent.move_0(dx, dy, 0)
			return 1 #keep going	
	
class Agent_controller:
	
	def __init__(self):
		self.globals = None
		self.job_manager = None

		self.state = {}
	
	def load_job(self, id, job_id):
		print "agent_controller, loading job: " + str((id, job_id))
		(script, job_id, sub_job) = self.job_manager.claim_job(id, job_id)
		if script == None:
			print "load job error"
			return
		
		state = Agent_script(**{
		'agent_id' : id,
		'mode' : 'job_script',
		'job_id' : job_id,
		'sub_job' : sub_job,
		'script' : script,			
		})
		self.state[id] = state
		
	def next_action(self, id):
		
		if not id in self.state.keys():
			print "No agent_state loaded"
			return 0
		state = self.state[id]
		if state.mode == 'job_script':
			print "run job script: " + str(id)
			#run_script(id, state )
			self.state[id].run()
			return 1
		if state.mode == 'custom_script':
			print "run custom script: " + str(id)
		if state.mode == 'sub_job_complete':
			print "Subjob complete: select next behavior"
			return 0
		else:
			print "Mode not valid"
			return 0

	#loads a user defined script
	def load_script(id, script):
		print " agent_controller load_script: custom script"
		self.state[id] = {
		'mode' : 'custom_script',
		'script' : script,
		'local' : {},
		'ip' : 0, #instruction pointer
		}
###
###
### DEPRECATE BELOW LINE
###
###		

	def command_index():
		
		#symbols *,@,#,$,%
		
		#predicates
		('?agent_holding_item', agent_id, item_id)
		('?item_at_location', item_id, position)
		('?agent_at_location', agent_id, position)

		#actions
		('::move', position)
		('::pickup_item', item_id)
		('::move_item', item_id, position)
		
		#atomic agent actions
		(':move', position)
		(':drop_item', item_id)
		(':pickup_item', item_id)

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
