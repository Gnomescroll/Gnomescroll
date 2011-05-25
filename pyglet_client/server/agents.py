'''
Agents:
    Objects through which a Player experiences the game world
'''

import math

from math import floor

from game_state import GameStateGlobal
from game_state import GenericObjectList
from net_out import NetOut

from cube_dat import CubeGlobals

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

        self.terrainMap = GameStateGlobal.terrainMap
        self.collisionDetection = CubeGlobals.collisionDetection

        assert self.collisionDetection != None

        if id is None:
            id = GameStateGlobal.new_agent_id()
        self.id = id

        self.d_x = 0
        self.d_y = 0
        self.d_xa = 0
        self.d_za = 0

        self.last_control_tick = 0
        self.jetpack = 0
        self.brake = 0

        self.x_int = int(x)
        self.y_int = int(y)
        self.z_int = int(z)

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

### Parameters for collision box ###
        box_height = 1.5
        box_r = .30

### Collisions on X axis ###
        xc_pos = 0 #number of collisions with north blocks
        xc_neg = 0 #number of collisions with south blocks

        bz = floor(z)
        for bx in range(floor(x+vx-box_r), floor(x+vx+box_r)+1):
        #x+
            by = floor(y+vy+box_r)
            if self.collisionDetection.collision(bx,by,bz):
                xc_pos +=1
        #x-
            by = floor(y+vy-box_r)
            if self.collisionDetection.collision(bx,by,bz):
                xc_neg +=1

### Collisions on Y axis ###

        yc_pos = 0 #number of collisions with north blocks
        yc_neg = 0 #number of collisions with south blocks

        bz = floor(z)
        for by in range(floor(y+vy-box_r), floor(y+vy+box_r)+1):
        #x+
            bx = floor(x+vx+box_r)
            if self.collisionDetection.collision(bx,by,bz):
                yc_pos +=1
        #x-
            bx = floor(x+vx-box_r)
            if self.collisionDetection.collision(bx,by,bz):
                yc_neg +=1

### Collision on Z axis
        zc_neg = 0
        zc_pos = 0
        z_margin = .01
        z_bounce = .90

        bz0 = floor(z - z_margin)
        bz1 = floor(z+box_height)
        for by in range(floor(y+vy-box_r), floor(y+vy+box_r)+1):
            for by in range(floor(y+vy-box_r), floor(y+vy+box_r)+1):
                if self.collisionDetection.collision(bx,by,bz0):
                    zc_neg +=1
                if self.collisionDetection.collision(bx,by,bz1):
                    zc_pos +=1


        if zc_neg >0:
            z += 0.01
            if vz < 0:
                vz *= -1 *z_bounce
        else:
            z += vz
        #print str(z)

        xy_bounce = 0.90
## handle y collisions
        if xc_pos != 0  or xc_neg !=0:
            if xc_pos != 0 and xc_neg !=0:
                x += vx
            elif xc_pos != 0:
                if vx > 1:
                    vx *= -1 * xy_bounce
            elif xc_neg != 0:
                if vx < 1:
                    vx *= -1 * xy_bounce
        else:
            x += vx
## handle x collisions
        if yc_pos != 0  or yc_neg !=0:
            if yc_pos != 0 and yc_neg !=0:
                y += vy
            elif yc_pos != 0:
                if vy > 1:
                    vy *= -1 * xy_bounce
            elif yc_neg != 0:
                if vy < 1:
                    vy *= -1 * xy_bounce
        else:
            y += vy

        ## z collision detection
        z_margin = 0.1

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
