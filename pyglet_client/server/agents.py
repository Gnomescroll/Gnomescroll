'''
Agents:
    Objects through which a Player experiences the game world
'''

import math

from game_state import GameStateGlobal
from game_state import GenericObjectList
from net_out import NetOut

# datastore for agents
class AgentList(GenericObjectList):

    def __init__(self):
        GenericObjectList.__init__(self)
        self._metaname = 'AgentList'
        self._itemname = 'Agent'
        self._object_type = Agent

    def create(self, player_id, x=0, y=0, z=0, xa=0, ya=0):
        self._add(x, y, z, xa, ya, player_id)

    def destroy(self, agent):
        self._remove(agent)

# represents an agent under control of a player
class Agent:

    HEALTH_MAX = 100
    _RESPAWN_TIME = 1. # seconds
    RESPAWN_TICKS = int(_RESPAWN_TIME / GameStateGlobal.TICK)

    def __init__(self, x, y, z, xa, ya, player_id, id=None):
        x,y,z = [float(i) for i in (x,y,z)]
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

        self.owner = player_id

    # set agent state explicitly
    def set_agent_control_state(self, *args):
        d_x, d_y, d_xa, d_za, jetpack, brake, tick = args
        self.last_control_tick = tick
        self.d_x = d_x #a byte
        self.d_y = d_y #a byte
        self.d_xa = d_xa
        self.d_za = d_za
        self.jetpack = jetpack
        self.brake = brake

    # apply physics to agent
    def tick(self):
        if not self.dead:
            self._tick_physics()
        else:
            self._tick_respawn()


    def _tick_physics(self):
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
        NetOut.event.agent_state_change(self)

    def _tick_respawn(self):
        if self.dead:
            self.respawn_countdown -= 1
        if self.respawn_countdown <= 0:
            self.respawn()

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
        self.respawn_countdown = self.RESPAWN_TICKS

    def _spawn_point(self):
        # later, add spawn zones/ boundaries to spawn in
        return [10, 10, 100]

    def _set_position(self, pos=None):
        if pos is None:
            pos = self._spawn_point()
        else:
            assert len(pos) <= 3
            self.state[0:len(pos)] = pos

    def respawn(self): # or can destroy / recreate Agent
        self.revive()
        self.set_position()
