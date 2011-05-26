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

    def create(self, player_id, x=0, y=0, z=0):
        self._add(x, y, z, player_id)

    def destroy(self, agent):
        self._remove(agent)

# represents an agent under control of a player
class Agent:

    HEALTH_MAX = 100
    _RESPAWN_TIME = 1. # seconds
    RESPAWN_TICKS = int(_RESPAWN_TIME / GameStateGlobal.TICK)

    def __init__(self, x, y, z, player_id, id=None):
        x,y,z = [float(i) for i in (x,y,z)]
        self.state = [x,y,z, 0.,0.,0., 0.,0.,0.] #position, velocity, acceleration

        self.terrainMap = GameStateGlobal.terrainMap
        self.collisionDetection = CubeGlobals.collisionDetection

        assert self.collisionDetection != None

        if id is None:
            id = GameStateGlobal.new_agent_id()
        self.id = id

        #b_height
        #t_height
        self.b_height = 1.5
        self.t_height = .75
        self.box_r = .30

        self.d_x = 0 #yaw?
        self.d_y = 0 #pitch?
        self.v_x = 0
        self.v_y = 0

        self.last_control_tick = 0
        self.jump = 0 #also need to record last jump
        self.jetpack = 0
        self.brake = 0

        self.health = self.HEALTH_MAX
        self.dead = False

        self.weapons = []

        self.owner = player_id

    # set agent state explicitly
    def set_agent_control_state(self, tick, *args):
        d_x, d_y, v_x, v_y, jetpack, jump, brake = args
        #print str(args)
        self.last_control_tick = tick
        self.d_x = d_x #a byte
        self.d_y = d_y #a byte
        #self.d_xa = d_xa
        #self.d_za = d_za
        self.v_x = v_x
        self.v_y = v_y
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
        ax,ay,az = (0,0,0)
        vx,vy = (0,0)
        #constants
        tr = 100. #tick rate
        tr2 = tr**2 #tick rate squared
        xy_brake = math.pow(.50, 1/(float(tr))) #in percent per second
        xy_speed = 2. / tr
        z_gravity = -.40 / tr2
        z_jetpack = 0.80 / tr2
        #gravity
    #TODO: should turn gravity off if agent is in contact with ground
        #velocity from acceleration and inputs
        vx += ax + self.v_x*xy_speed
        vy += ay + self.v_y*xy_speed

        if self.brake != 0:
            vx *= xy_brake
            vy *= xy_brake
            vz *= xy_brake

    #constants for collision box
        b_height = self.b_height
        t_height = self.t_height
        box_r = .30

    #XY-collision

        box_r = self.box_r

### Collisions on X axis ###
        xc_pos_projected = 0 #number of collisions with north blocks
        xc_neg_projected = 0 #number of collisions with south blocks
        xc_pos_current = 0
        xc_neg_current = 0

        for bz in range(floor(z - b_height), floor(z +t_height)+1):
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

        yc_pos = 0
        yc_neg = 0
        yc_pos_current = 0
        yc_neg_current = 0

        for bz in range(floor(z - b_height), floor(z +t_height)+1):
            for by in range(floor(y+vy-box_r), floor(y+vy+box_r)+1):
            #y+
                bx = floor(x+vx+box_r)
                if self.collisionDetection.collision(bx,by,bz):
                    yc_pos +=1
            #y-
                bx = floor(x+vx-box_r)
                if self.collisionDetection.collision(bx,by,bz):
                    yc_neg +=1

    #Z-collision

        z_margin = .01
        z_bounce = .65

        zc_neg_soft = 0
        zc_neg_hard = 0
        zc_current = 0

        bz_current = float(z - b_height)
        bz_soft_floor = floor(z - b_height - z_margin)
        bz_hard_floor = floor(z+vz-b_height)

        #for z, use current position
        #over lowest z level z
        for bx in range(floor(x-box_r), floor(x+box_r)+1):
            for by in range(floor(y-box_r), floor(y+box_r)+1):
                if self.collisionDetection.collision(bx,by,bz_soft_floor):
                     zc_neg_soft += 1
                if self.collisionDetection.collision(bx,by,bz_hard_floor):
                    zc_neg_hard +=1
                if self.collisionDetection.collision(bx,by,bz_current):
                    zc_current +=1

        #Hard collision predicted and not inside of something already
        if zc_neg_hard != 0 and zc_current == 0:
            if vz < 0:
                if vz < -0.01: #vertical velocity bounce treshold
                    vz *= -1 *z_bounce
                else:
                    vz = 0

        if zc_neg_soft == 0: #agent on ground
            pass
            az += (z_gravity) if z>0 else (-z_gravity) #[value_false, value_true][<test>]
        else:
            #z = floor(z) + z_margin/2
            if vz < 0:
                if vz < -0.01: #vertical velocity bounce treshold
                    vz *= -1 *z_bounce
                else:
                    vz = 0
        if zc_current != 0:
            z += .50 / tr

        if zc_neg_soft != 0:
            print "On ground!"
        if zc_neg_hard != 0:
            print "Hard Predicted Z-Collision!"
        if zc_current != 0:
            print "Hard current Z-Collision!"

        #jetpack adjustment to gravity
        if self.jetpack != 0: az += z_jetpack

        vz += az
        z += vz
## Position Change ##
        x += vx
        y += vy

        self.state = [x,y,z, vx,vy,vz, ax,ay,az]
        NetOut.event.agent_state_change(self)
        return


    ## DEPRECATE BELOW LINE ##
        ax,ay,az = (0,0,0)

        tr = 100. #tick rate
        tr2 = tr**2 #tick rate squared

        xy_speed = 0.1 / tr2
        ax = xy_speed * self.d_x
        ay = xy_speed * self.d_y

        xy_brake = math.pow(.50, 1/(float(tr))) #in percent per second
        vx += ax
        vy += ay
        if self.brake != 0:
            vx *= xy_brake
            vz *= xy_brake


### Collision on Z axis
        zc_neg = 0
        zc_pos = 0
        zc_floor = 0

        z_margin = .01
        z_bounce = .65

        bz_floor = floor(z - z_margin)
        bz0 = floor(z) #lowest level
        bz1 = floor(z+box_height) #highest level
        #over multiple z
        for by in range(floor(y+vy-box_r), floor(y+vy+box_r)+1):
            for by in range(floor(y+vy-box_r), floor(y+vy+box_r)+1):
                if self.collisionDetection.collision(bx,by,bz0):
                    zc_neg +=1
                if self.collisionDetection.collision(bx,by,bz1):
                    zc_pos +=1
        #over lowest z
        for by in range(floor(y+vy-box_r), floor(y+vy+box_r)+1):
            for by in range(floor(y+vy-box_r), floor(y+vy+box_r)+1):
                if self.collisionDetection.collision(bx,by,bz_floor):
                    zc_floor +=1

        #gravity, conditional upon being in contact with floor
        if zc_floor == 0: #is in contract with floor, no gravity
            if z <= 0.:
                az = .10 / tr2
            else:
                az = -0.10 / tr2
        else:
            pass
        #jetpack effect on gravity
        if self.jetpack != 0:
            print "jetpack"
            az += 0.15 / tr2
        #velocity update
        vz += az
        #collision detection
        if zc_neg != 0:
            z = floor(z+1)+ 0.099
            if vz < 0:
                vz *= -1 *z_bounce
        else:
            z += vz
        #print str(z)

        xy_bounce = 0.30
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
