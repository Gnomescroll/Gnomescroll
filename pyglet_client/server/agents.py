from game_state import GenericObjectList
from game_state import GameStateGlobal


# datastore for agents
class AgentList(GenericObjectList):

    def __init__(self):
        self._metaname = 'AgentList'
        self._itemname = 'Agent'
        self._object_type = Agent

    def create(self, x, y, z, xa, ya):
        self._add(x, y, z, xa, yz)

    def destroy(self, id):
        self._remove(id)

# represents an agent under control of a player
class Agent:

    HEALTH_MAX = 100

    def __init__(self, x, y, z, xa, ya, id=None):
        [x, y, z] = [float(x), float(y), float(z)]
        self.state = [x,y,z, 0.,0.,0., 0.,0.,0.] #position, velocity, acceleration
        self.xa = xa
        self.ya = ya

        if id is None:
            id = GameStateGlobal.new_agent_id()
        self.id = id 

        self.last_control_tick = 0
        self.d_x = 0
        self.d_y = 0
        self.d_xa = 0
        self.d_za = 0
        self.jetpack = 0
        self.brake = 0

        self.health = self.HEALTH_MAX
        self.dead = False

        self.weapons = []

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
        GameStateGlobal.eventOut.agent_state_change(self.id, GameStateGlobal.gameState.time, self.state)

    def take_damage(self, damage):
        self.health -= damage
        if self.health <= 0:
            self.die()

    def heal(self, amount):
        if not self.dead:
            self.health = min(self.health + amount, self.HEALTH_MAX)
            
    def die(self):
        self.dead = True

    def revive(self):
        self.health = self.HEALTH_MAX
        self.dead = False

    def respawn(self):
        if self.dead:
            # wait
            # revive
            pass
