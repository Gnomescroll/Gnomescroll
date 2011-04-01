from interface.agent import Agent
from state.objects import Objects

class Agent_controller:
	
	def __init__(self):
		self.globals = None

		self.agent_goals = {}
		#self.agents_tasks = {} #get next goal in task...

	def next_action(self, id):
		if not(id in self.agent_goals.keys()):
			return 0
		goal = self.agent_goals[id]
		agent = Agent(id)
		
		if(goal['goal'] == 'move'):
			return self.move_goal(id, goal)
		else:
			return 0 #stop

	### goal implementation
	def move_goal(self, id, goal)
		agent = Agent(id)
		(ptype, x, y, z) = agent.position
		(ptype_, x_, y_, z_) = goal['position']
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
		
	### goal creation code
	def create_move_goal(self, id, x, y, z):
		goal = {
		'goal' : 'move',
		'position' : (0, x, y, z)
		}
		self.agent_goals[id] = goal
	
	def create_move_item_goal(self, agent_id, item_id, position):
		agent = Agent(agent_id)
		item = Nobject(item_id)

		goal = {
			'active_goal' : 0,

			0 : {
				'goal' : 'move',
				'position' : (0, x, y, z),
				'next' : 1,
				},
			1 : {
				'goal' : 'pickup_item',
				'item_id' : item_id,
				'position' : item.position,
				'next' : 2,
				},
			2 : {
				'goal' : 'move',
				'position' : position,
				'next' : 1,
				},
			3 : {
				'goal' : 'drop_item',
				'item_id' : item_id,
				'position' : position,			
				},
			}

		(a_type, a_x, a_y, a_z) = agent.position
		(i_type, i_x, i_y, i_z) = item.position		
		(type, x, y, z) = position
		
		if i_type != 0:
			print "Need support for moving items which are in containers"
			return

		goal = {
		'goal' : 'move_item',
		'item_position' : (0, x, y, z)
		}		
		
