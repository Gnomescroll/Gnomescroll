'''
Agents:
    Objects through which a Player experiences the game world
'''

import math

from math import floor
from math import pi, cos, sin
from random import randrange

from game_state import GameStateGlobal
from game_state import GenericObjectList
from net_out import NetOut
from net_server import NetServer

from cube_dat import CubeGlobal

from weapons import LaserGun, Pick, BlockApplier

from opts import opts

# datastore controller for agents
class AgentList(GenericObjectList):

    def __init__(self):
        GenericObjectList.__init__(self)
        self._object_type = Agent

    def create(self, player_id, position=None, id=None):
        return self._add(player_id, position=position, id=id)

    def destroy(self, agent):
        if agent is None:
            return
        return self._remove(agent)

    def agents_near_point(self, x,y,z, radius):
        l=[]
        for agent in self.values():
            x_, y_, z_ = agent.pos()
            r2 = float(radius)**2
            if r2 > (x_-x)**2 + (y_-y)**2 + (z_-z)**2:
                l.append(agent)
        return l


class AgentPhysics:

    #collision tests
    def point_collision_test(self, x_,y_,z_):
        x,y,z = self.pos()
        b_height = self.b_height
        t_height = self.t_height
        box_r = self.box_r

        z_max = z + self.t_height
        z_min = z - self.b_height
        x_max = x + box_r
        x_min = x - box_r
        y_max = y + box_r
        y_min = y - box_r

        #print str((x_min, x_max, y_min, y_max, z_min, z_max))
        #print str((x_,y_,z_))

        if x_min < x_ and x_ < x_max and y_min < y_ and y_ < y_max and z_min < z_ and z_ < z_max:
            return True
        else:
            return False

    def sphere_collision_test(self, x,y,z,r):
        pass

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

        #print str((vx,vy))
        if self.brake != 0:
            vx *= xy_brake
            vy *= xy_brake
            vz *= xy_brake

    #constants for collision box
        b_height = self.b_height
        t_height = self.t_height
        box_r = self.box_r

### Collisions on X axis collision ###

        xc_pos_current = 0
        xc_pos_projected = 0

        xc_neg_current = 0
        xc_neg_projected = 0

        bx_pos_current = floor(x+box_r)
        bx_pos_projected = floor(x+vx+box_r)

        bx_neg_current = floor(x-box_r)
        bx_neg_projected = floor(x+vx-box_r)

        for bz in range(floor(z - b_height), floor(z +t_height)+1):
            for by in range(floor(y-box_r), floor(y+box_r)+1):
            #x+
                if self.collisionDetection.collision(bx_pos_current,by,bz):
                    xc_pos_current +=1
                if self.collisionDetection.collision(bx_pos_projected,by,bz):
                    xc_pos_projected +=1
            #x-
                if self.collisionDetection.collision(bx_neg_current,by,bz):
                    xc_neg_current +=1
                if self.collisionDetection.collision(bx_neg_projected,by,bz):
                    xc_neg_projected +=1

### Collision on Y axis ###

        yc_pos_current = 0
        yc_pos_projected = 0

        yc_neg_current = 0
        yc_neg_projected = 0

        by_pos_current = floor(y+box_r)
        by_pos_projected = floor(y+vy+box_r)

        by_neg_current = floor(y-box_r)
        by_neg_projected = floor(y+vy-box_r)

        for bz in range(floor(z - b_height), floor(z +t_height)+1):
            for bx in range(floor(x-box_r), floor(x+box_r)+1):
            #x+
                if self.collisionDetection.collision(bx,by_pos_current,bz):
                    yc_pos_current +=1
                if self.collisionDetection.collision(bx,by_pos_projected,bz):
                    yc_pos_projected +=1
            #x-
                if self.collisionDetection.collision(bx,by_neg_current,bz):
                    yc_neg_current +=1
                if self.collisionDetection.collision(bx,by_neg_projected,bz):
                    yc_neg_projected +=1

### XY Collision ###

        xyc_projected = 0

        for bz in range(floor(z - b_height), floor(z +t_height)+1):
            for by in range(floor(y+vy-box_r), floor(y+vy+box_r)+1):
                for bx in range(floor(x+vx-box_r+vx), floor(x+vx+box_r)+1):
                    if self.collisionDetection.collision(bx,by,bz):
                        xyc_projected += 1

        xyc_current = 0

        for bz in range(floor(z - b_height), floor(z +t_height)+1):
            for by in range(floor(y-box_r), floor(y+box_r)+1):
                for bx in range(floor(x-box_r+vx), floor(x+box_r)+1):
                    if self.collisionDetection.collision(bx,by,bz):
                        xyc_current += 1

        #dont do this right now

        if False and xyc_projected != 0:
            # print "Projected XY collision!"
            vx =0
            vy =0

### XY collision constants
        xy_bounce = .65
        xy_bounce_v_threshold = 0.35 / tr

    ## handle x collisions
        #xc_pos_current, xc_pos_projected , xc_neg_current, xc_neg_projected

        if xc_pos_current == 0 and xc_neg_current == 0:
            if xc_pos_projected != 0:
                vx = 0
            if xc_neg_projected != 0:
                vx = 0
        else:
            pass
            #print "X collision error!!"

    ## handle y collisions
        #yc_pos_current, yc_pos_projected , yc_neg_current, yc_neg_projected

        if yc_pos_current ==0 and yc_neg_current ==0:
            if yc_pos_projected != 0:
                vy = 0
            if yc_neg_projected != 0:
                vy = 0
        else:
            pass
            #print "Y collision error!!"


        #environmental and input controls
        on_ground = self.on_ground #environmental state
        jetpack = self.jetpack #control state

        if on_ground == 0: #agent on ground
            az += (z_gravity) if z>0 else (-z_gravity)
        #jetpack adjustment to gravity
        if jetpack != 0: az += z_jetpack
    ## calculate velocity from acceleration inputs
        vz += az

##parameters for collision
        z_margin = .01
        z_bounce = .65
        z_bounce_v_threshold = 0.35 / tr

## determine if agent is on ground and if they are colliding with anything at current position
        zc_current = 0
        zc_ground = 0
        zc_neg_projected = 0
        bz_current = float(z - b_height)
        bz_ground = floor(z - b_height - z_margin)
        bz_neg_projected = floor(z+vz-b_height)

        for bx in range(floor(x-box_r), floor(x+box_r)+1):
            for by in range(floor(y-box_r), floor(y+box_r)+1):
                if self.collisionDetection.collision(bx,by,bz_current):
                    zc_current +=1
                if self.collisionDetection.collision(bx,by,bz_ground):
                     zc_ground += 1
                if self.collisionDetection.collision(bx,by,bz_neg_projected):
                    zc_neg_projected +=1
    ##  calculate environmental state
        #agent ground state
        if zc_ground != 0:
            if self.on_ground != 1:
                self.on_ground = 1
                print "On ground!"
        else:
            if self.on_ground == 1:
                print "Off ground!"
                self.on_ground = 0

    ## apply velocity
        #Hard collision predicted and not inside of something already
        if zc_neg_projected != 0 and zc_current == 0:
            if vz < 0:
                if vz < -z_bounce_v_threshold: #vertical velocity bounce treshold
                    vz *= -1 *z_bounce
                else:
                    vz = 0

        if zc_current != 0: #if agent is inside of block, float them out the top
            z += .50 / tr

        if zc_neg_projected != 0:
            #print "Predicted neg Z-Collision!"
            pass
        if zc_current != 0:
            pass
            #print "Hard current neg Z-Collision!"

## Position Change ##
        z += vz
        x += vx
        y += vy

        self.state = [x,y,z, vx,vy,vz, ax,ay,az]
        #print 'agent state:'
        #print self.state
        NetOut.event.agent_state_change(self)


class AgentAction:

    def fire_projectile(self):
        print 'Agent.fire_projectile'
        state = self.state_vector()
        projectile = GameStateGlobal.projectileList.create(state=state, type=1, owner=self.owner)
        NetOut.event.projectile_create(projectile)


# represents an agent under control of a player
class Agent(AgentPhysics, AgentAction):

    HEALTH_MAX = 100
    _RESPAWN_TIME = 2. # seconds
    RESPAWN_TICKS = int(_RESPAWN_TIME / opts.tick)

    def __init__(self, player_id, position=None, id=None):
        if position is None:
            position = self._spawn_point()

        x,y,z = [float(i) for i in position]
        self.state = [x,y,z, 0.,0.,0., 0.,0.,0.] #position, velocity, acceleration

        self.button_state = [0 for i in range(6)]

        ### Global imports ###
        self.terrainMap = GameStateGlobal.terrainMap
        self.collisionDetection = CubeGlobal.collisionDetection
        assert self.collisionDetection != None
        ### End Global imports ###

        if id is None:
            id = GameStateGlobal.new_agent_id()
        self.id = id

        ### Agent Parameters ###
        self.b_height = 1.5
        self.t_height = .75
        self.box_r = .30
        ### End Agent Paramters ###

        ### Agent State
        self.d_x = 0 #yaw?
        self.d_y = 0 #pitch?
        self.v_x = 0
        self.v_y = 0
        self.x_angle = 0
        self.y_angle = 0
        ### End Agent State ###

        #control state variables
        self.last_control_tick = 0
        self.jump = 0 #also need to record last jump
        self.jetpack = 0
        self.brake = 0
        #environmental state variables
        self.on_ground = 0

        #misc state
        self.respawn_countdown = self.RESPAWN_TICKS
        self.health = self.HEALTH_MAX
        self.dead = False

        wl = GameStateGlobal.weaponList
        self.weapons = [    \
            wl.create('LaserGun'),
            wl.create('Pick'),
            wl.create('BlockApplier'),
            wl.create('HitscanLaserGun'),
        ]
        self._active_weapon = 0

        self.owner = player_id

    def pos(self):
        return self.state[0:3]

    def active_weapon(self):
        return self.weapons[self._active_weapon]

    def __getattr__(self, attr):
        if attr == 'x':
            return self.__dict__['state'][0]
        elif attr == 'y':
            return self.__dict__['state'][1]
        elif attr == 'z':
            return self.__dict__['state'][2]
        else:
            raise AttributeError, 'Agent instance has no property %s' % (attr,)

    def __setattr__(self, attr, val):
        if attr == 'x':
            self.__dict__['state'][0] = val
        elif attr == 'y':
            self.__dict__['state'][1] = val
        elif attr == 'z':
            self.__dict__['state'][2] = val
        else:
            self.__dict__[attr] = val

    def json(self, properties=None): # json encodable string representation
        d = {
            'id'    :   self.id,
        }
        if properties is None:
            d.update({
                'health': self.health,
                'dead'  : int(self.dead),
                'owner' : self.owner,
                'weapons': {
                    'weapons': [weapon.json() for weapon in self.weapons],
                    'active' : self._active_weapon,
                },
                'state' : self.state,
            })
        else:
            if type(properties) == str:
                properties = [properties]
            for prop in properties:
                if prop == 'weapons':
                    d[prop] = {
                        'weapons': [weapon.json() for weapon in self.weapons],
                        'active' : self._active_weapon,
                    }
                elif prop == 'active_weapon':
                    d['weapons'] = {
                        'active'    :   self._active_weapon,
                    }
                else:
                    d[prop] = getattr(self, prop)
                if prop == 'dead':
                    d[prop] = int(d[prop])
        return d

    def drop_weapon(self, weapon, by_id=False):
        old_len = len(self.weapons)
        if by_id:
            self.weapons = [w for w in self.weapons if w.id != weapon]
        else:
            self.weapons = [w for w in self.weapons if w != weapon]
        if old_len != len(self.weapons):
            NetOut.event.agent_update(self, 'weapons')

    def set_active_weapon(self, weapon_index):
        old = self._active_weapon
        self._active_weapon = weapon_index
        if old != weapon_index:
            NetOut.event.agent_update(self, 'active_weapon')

    # set agent state explicitly
    def set_control_state(self, state, angle=None, tick=None):
        old_tick = self.last_control_tick
        if tick is not None:
            self.last_control_tick = tick

        old_state = self.control_state()
        d_x, d_y, v_x, v_y, jetpack, brake = state
        self.d_x = d_x
        self.d_y = d_y
        self.v_x = v_x
        self.v_y = v_y
        self.jetpack = jetpack
        self.brake = brake

        if angle is not None:
            old_angle = self.angle()
            self.set_angle(angle)
            
        ''' Use this if/when tick is actually being used '''
        #if self.last_control_tick > old_tick and \
           #(old_state != self or old_angle != angle):
               #NetOut.event.agent_control_state(self)

        if old_state != state or old_angle != angle:
            NetOut.event.agent_control_state(self)

    def set_angle(self, angle):
        self.x_angle, self.y_angle = angle

    def angle(self):
        return [self.x_angle, self.y_angle]

    def control_state(self):
        return [\
            self.d_x,
            self.d_y,
            self.v_x,
            self.v_y,
            self.jetpack,
            self.brake
        ]

    # apply physics to agent
    def tick(self):
        if self.dead:
            self._tick_respawn()
        else:
            self._tick_physics()

    def _tick_respawn(self):
        if self.dead:
            self.respawn_countdown += -1
        if self.respawn_countdown <= 0:
            self.respawn()

    def state_vector(self):
        return [
            self.x, self.y, self.z,
            cos( self.x_angle * pi) * cos( self.y_angle * pi),
            sin( self.x_angle * pi) * cos( self.y_angle * pi),
            sin( self.y_angle),
        ]

    def take_damage(self, damage, projectile_owner=None):
        print 'agent %s taking damage %i' % (self.id, damage,)
        print self.health
        if not self.dead:
            old = self.health
            self.health -= damage
            self.health = max(self.health, 0)
            if self.health <= 0:
                self.die(projectile_owner)
            elif self.health != old:
                NetOut.event.agent_update(self, 'health')
        print damage

    def heal(self, amount):
        if not self.dead:
            old = self.health
            self.health = min(self.health + amount, self.HEALTH_MAX)
            if self.health != old:
                NetOut.event.agent_update(self, 'health')

    def die(self, projectile_owner=None):
        if not self.dead:
            suicide = False
            try:
                you_player = GameStateGlobal.playerList[self.owner]
                you = NetServer.connectionPool.by_client_id(you_player.cid)
                you_player.died()
            except e:
                print 'Error obtaining client object that owns dying agent.'
                print e
                self.dead = True
                return
            if projectile_owner is None:
                you.sendMessage.you_died('You died mysteriously.')
                pass
            else:
                try:
                    killer = GameStateGlobal.playerList[projectile_owner]
                    if killer.cid == you_player.cid:
                        msg = 'You killed yourself.'
                        suicide = True
                    else:
                        killer.killed()
                        msg = 'You were killed by %s' % (killer.name,)
                    you.sendMessage.you_died(msg)
                    if not suicide:
                        try:
                            killer_client = NetServer.connectionPool.by_client_id(killer.cid)
                            msg = 'You killed %s' % you.name
                            killer_client.sendMessage.you_killed(msg)
                        except e:
                            print 'Killer\'s client was not found'
                            print e
                except KeyError:    # race condition, where client quits before his bullet kills
                    you.sendMessage.you_died('You were killed by a ghost.')

            self.dead = True
            NetOut.event.agent_update(self, ['dead', 'health'])

    def _revive(self):
        self.health = self.HEALTH_MAX
        self.dead = False
        self.respawn_countdown = self.RESPAWN_TICKS

    def _spawn_point(self):
        # later, add spawn zones/ boundaries to spawn in
        x = randrange(0,7)
        y = randrange(0,7)
        z = 5
        return [x, y, z]

    def _set_position(self, pos=None):
        if pos is None:
            pos = self._spawn_point()
        assert len(pos) <= 3
        self.state[0:3] = pos

    def respawn(self): # or can destroy / recreate Agent
        self._revive()
        self._set_position()
        NetOut.event.agent_update(self, ['health', 'dead', 'state'])
