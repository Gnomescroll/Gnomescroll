class Agent_handler:

    def __init__(self):
        self.agents = []
        self.world_id = None
        self.client_id = None
	self.agents_changed = False

    def agent_info(self, id, client_id, value, **msg):
        print "agent_info called."  

    def agent_position_change(self, id, position, **msg):
        for element in self.agents:
            if element['id'] == id:
                element['position'] = position
		self.agents_changed = True
                break

    def agent_state_change(self, id, version ,**msg):
        for element in self.agents:
            if element['id'] == id:
                element['version'] = version
		self.agents_changed = True
                break

    def agent_list(self, list, world_id, client_id, **msg):
        self.agents = list
        self.world_id = world_id
        self.client_id = client_id

    def agent_create(self, id, position, player_id, **msg):
        self.agents.append({"player_id": player_id, "position": position, "version": 0, "type": ["agent"], "id": id})
	self.agents_changed = True
        
    def agent_delete(self, id, **msg):
        for element in self.agents:
            if element['id'] == id:
                del element
		self.agents_changed = True
                break
