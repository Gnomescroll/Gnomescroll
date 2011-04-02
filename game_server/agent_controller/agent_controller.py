from interface.agent import Agent
from interface.agent import Nobject

class Agent_controller:
	
	def __init__(self):
		self.globals = None

		self.agent_goals = {}
		self.agent_behavior = {} #eat, jobs, wander
		#self.agents_tasks = {} #get next goal in task...

	def next_action(self, id):
		if not(id in self.agent_goals.keys()):
			return 0
		goal = self.agent_goals[id]
		agent = Agent(id)
		
		if(goal['goal'] == 'move'):
			return self.move_goal(id, goal['position'])
		if(goal['goal'] == 'FSM1'):
			return self.FSM1(id, goal)
		else:
			print "Agent_controller.next_action: goal type not valid"
			return 0 #stop

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
		
		#predicates
		('agent_holding_item', agent_id, item_id)
		('item_at_location', item_id, (t, x, y, z))
		('agent_at_location', agent_id, (t, x, y, z)

		#actions
		('move', agent_id, x, y, z)
		('pickup_item', agent_id, item_id)
		('move_item', agent_id, item_id, position)
		
		#atomic actions
		('atomic_move', agent_id, dx, dy, dz)
		('atomic_move_item', agent_id, item_id, (t, x, y, z)) #into chests, etc...
		('atomic_drop_item', agent_id, item_id)
		('atomic_pickup_item', agent_id, item_id)

		#production commands
		('assert', predicate)
		
		
	def set_prolog_agent_goal(self, goal_state):
		
		goal = {
			
		
		}
		
		
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
