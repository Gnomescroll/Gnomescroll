import math

from terrain_map import TerrainMap

class PlayerAgent:
    eventOut = None
    gameState = None

    def __init__(self, id, x, y, z, xa, ya):
        assert self.eventOut != None
        assert self.gameState != None
        [x, y, z] = [float(x), float(y), float(z)]
        self.state = [x,y,z, 0.,0.,0., 0.,0.,0.] #position, velocity, acceleration
        self.xa = xa
        self.ya = ya
        self.id = id

        self.last_control_tick = 0
        self.d_x = 0
        self.d_y = 0
        self.d_xa = 0
        self.d_za = 0
        self.jetpack = 0
        self.brake = 0

    # set agent state explicitly
    def set_agent_control_state(self, tick, d_x, d_y, d_xa, d_za, jetpack, brake):
        self.last_control_tick = tick
        self.d_x = d_x #a byte
        self.d_y = d_y #a byte
        self.d_xa = d_xa
        self.d_za = d_za
        self.jetpack = jetpack
        self.brake = brake

    # apply physics to agent
    def tick(self):
        x,y,z, vx,vy,vz, ax,ay,az = self.state
        tr = 100. #tick rate
        tr2 = tr**2 #tick rate squared
        if z <= 0.:
            az = .10 / tr2
        else:
            az = -0.10 / tr2
        if self.jetpack:
            az += 0.15 / tr2

        xy_speed = 0.1 / tr2
        ax = xy_speed * self.d_x
        ay = xy_speed * self.d_y

        vz += az

        xy_brake = math.pow(.50, 1/(float(tr))) #in percent per second
        vx += ax
        vy += ay
        if self.brake != 0:
            vx *= xy_brake
            vz *= xy_brake

        x += vx
        y += vy
        z += vz

        self.state = [x,y,z, vx,vx,vz, ax,ay,az]
        self.eventOut.agent_state_change(self.id, self.gameState.time, self.state)

# datastore for agents
class AgentList:
    def __init__(self, gameState):
        self.gameState = gameState
        AgentList.gameState = self.gameState
        self.agents = {}

    def create_agent(self, x,y,z,xa,ya):
        #(x,y,z,xa,ya) = position
        id = self.gameState.new_id()
        agent = PlayerAgent(id, x,y,z, xa,ya)
        self.agents[id] = agent
        print "AgentList: Agent Created, id= %i" % (id,)

    def get_agent(self,id):
        if not self.agents.has_key(id):
            print "Agentlist.set_agent_control_state: Agent does not exist: %i" % (id,)
            return None
        return self.agents[id]

# main game state wrapper
class GameState:

    def __init__(self):
        self.time = 0
        self.id = 0
        self.agentList = AgentList(self)
        self.terrainMap = TerrainMap()
    def new_id(self):
        self.id += 1
        return self.id

    # tick all agents
    def tick(self):
        for agent in self.agentList.agents.values():
            agent.tick()
        self.time += 1
        if self.time % 100 == 0:
            print "time= %i" % (self.time,)
