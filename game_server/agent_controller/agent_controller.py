from interface.agent import Agent

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
			(ptype, x, y, z) = agent.position
			(ptype_, x_, y_, z_) = goal['position']
			dx = 0
			dy = 0
			if x > x_:
				dx = -1
			if x < x_:
				dx = 1
			if y > y_:
				dy = 1
			if y < y_:
				dy = -1
			if dx == 0 and dy == 0:
				return 0
			else:
				agent.move_0(dx, dy, 0)
				return 1 #keep going
		else:
			return 0 #stop

	def create_move_goal(self, id, x, y, z):
		goal = {
		'goal' : 'move',
		'position' : (0, x, y, z)
		}
		
		self.agent_goals[id] = goal
	
