'''
Agents:
    Objects through which a Player experiences the game world
'''

import math

from math import floor, ceil
from math import pi, cos, sin
from random import randrange

from game_state import GameStateGlobal
from object_lists import GenericObjectList
from net_out import NetOut
from net_server import NetServer

import c_lib.terrain_map as terrain_map
collisionDetection = terrain_map.collisionDetection
#from cube_dat import CubeGlobal

from weapons import LaserGun, Pick, BlockApplier

from vector_lib import distance

from opts import opts

from c_lib.c_lib_agent_wrapper import AgentListWrapper, AgentWrapper

# datastore controller for agents
class AgentList(GenericObjectList):

    def __init__(self):
        GenericObjectList.__init__(self)
        self._object_type = Agent
        self._wrapper = AgentListWrapper

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

    def compute_state(self):
        # only v_x and v_y are used
        v = 1
        d_x, d_y, v_x, v_y = 0,0,0,0
        u,d,l,r, jetpack, brake = self.button_state
        
        if u:
                v_x += v*cos( self.x_angle * pi)
                v_y += v*sin( self.x_angle * pi)
        if d:
                v_x += -v*cos( self.x_angle * pi)
                v_y += -v*sin( self.x_angle * pi)
        if l:
                v_x += v*cos( self.x_angle * pi + pi/2)
                v_y += v*sin( self.x_angle * pi + pi/2)
        if r:
                v_x += -v*cos( self.x_angle * pi + pi/2)
                v_y += -v*sin( self.x_angle * pi + pi/2)
                
        return [\
            d_x,
            d_y,
            v_x,
            v_y,
            jetpack,
            brake,
        ]

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
        tr = 10. #tick rate
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

        for bz in range(int(floor(z - b_height)), int(floor((z +t_height)+1))):
            for by in range(int(floor(y-box_r)), int(floor((y+box_r)+1))):
            #x+
                if collisionDetection(bx_pos_current,by,bz):
                    xc_pos_current +=1
                if collisionDetection(bx_pos_projected,by,bz):
                    xc_pos_projected +=1
            #x-
                if collisionDetection(bx_neg_current,by,bz):
                    xc_neg_current +=1
                if collisionDetection(bx_neg_projected,by,bz):
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

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for bx in range(int(floor(x-box_r)), int(floor(x+box_r)+1)):
            #x+
                if collisionDetection(bx,by_pos_current,bz):
                    yc_pos_current +=1
                if collisionDetection(bx,by_pos_projected,bz):
                    yc_pos_projected +=1
            #x-
                if collisionDetection(bx,by_neg_current,bz):
                    yc_neg_current +=1
                if collisionDetection(bx,by_neg_projected,bz):
                    yc_neg_projected +=1

### XY Collision ###

        xyc_projected = 0

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for by in range(int(floor(y+vy-box_r)), int(floor(y+vy+box_r)+1)):
                for bx in range(int(floor(x+vx-box_r+vx)), int(floor(x+vx+box_r)+1)):
                    if collisionDetection(bx,by,bz):
                        xyc_projected += 1

        xyc_current = 0

        for bz in range(int(floor(z - b_height)), int(floor(z +t_height)+1)):
            for by in range(int(floor(y-box_r)), int(floor(y+box_r)+1)):
                for bx in range(int(floor(x-box_r+vx)), int(floor(x+box_r)+1)):
                    if collisionDetection(bx,by,bz):
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

        for bx in range(int(floor(x-box_r)), int(floor(x+box_r)+1)):
            for by in range(int(floor(y-box_r)), int(floor(y+box_r)+1)):
                if collisionDetection(bx,by,bz_current):
                    zc_current +=1
                if collisionDetection(bx,by,bz_ground):
                     zc_ground += 1
                if collisionDetection(bx,by,bz_neg_projected):
                    zc_neg_projected +=1
    ##  calculate environmental state
        #agent ground state
        if zc_ground != 0:
            if self.on_ground != 1:
                self.on_ground = 1
                #print "On ground!"
        else:
            if self.on_ground == 1:
                #print "Off ground!"
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
        #NetOut.event.agent_state_change(self)


class AgentAction:

    def fire_projectile(self, pos=None, direction=None):
        print 'Agent.fire_projectile'
        state = self.state_vector()
        if pos is not None:
            state[0:3] = pos
        if direction is not None:
            state[3:6] = direction
        projectile = GameStateGlobal.projectileList.create('Laser', state, owner=self.owner)
        NetOut.event.projectile_create(projectile)

    def throw_grenade(self, direction):
        print 'Agent.throw_grenade'
        pl = GameStateGlobal.projectileList
        pos = self.pos()
        pos[2] += 0.25 * self.b_height
        state = pos + direction
        grenade = pl.create('Grenade', state, owner=self.owner, ttl=0)
        NetOut.event.projectile_create(grenade)


# represents an agent under control of a player
class Agent(AgentPhysics, AgentAction):

    HEALTH_MAX = 100
    _RESPAWN_TIME = 2. # seconds
    RESPAWN_TICKS = int(_RESPAWN_TIME / opts.tick)

    def __init__(self, player_id, position=None, id=None, team=None):

        ### Global imports ###
        self.terrainMap = GameStateGlobal.terrainMap
        self.collisionDetection = collisionDetection
        assert self.collisionDetection != None
        ### End Global imports ###

        self.team = team

        if id is not None:
            print "SOMETHING IS GIVING SERVER AGENT AN ID"
            raise Exception

        #if id is None:
            #id = GameStateGlobal.new_agent_id()
        id = AgentListWrapper._create_agent()
        self.id = id

        ### Agent Parameters ###
        self.b_height = 1.5
        self.block_height = int(ceil(self.b_height))
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

        if position is None:
            position = self._spawn_point()

        x,y,z = [float(i) for i in position]
        self.state = [x,y,z, 0.,0.,0., 0.,0.,0.] #position, velocity, acceleration

        self.button_state = [0 for i in range(6)]

        wl = GameStateGlobal.weaponList
        self.weapons = [    \
            wl.create('LaserGun', owner=self),
            wl.create('Pick', owner=self),
            wl.create('BlockApplier', owner=self),
            wl.create('HitscanLaserGun', owner=self),
            wl.create('GrenadePouch', owner=self),
        ]
        self._active_weapon = 0
        self.inventory = []

        self.owner = player_id

    # gets or sets
    def pos(self, xyz=None):
        if xyz is None:
            return self.state[0:3]
        else:
            self.x, self.y, self.z = xyz

    def velocity(self):
        return self.state[3:6]

    def active_weapon(self):
        return self.weapons[self._active_weapon]

    def quit(self):
        self.dump_inventory()

    def dump_inventory(self):
        new_inv = []
        for item in self.inventory:
            if item.drop_on_death:
                item.drop()
            else:
                new_inv.append(inv)
        self.inventory = new_inv

    @property
    def x(self):
        return self.state[0]
    @x.setter
    def x(self, val):
        self.state[0] = val
    @property
    def y(self):
        return self.state[1]
    @y.setter
    def y(self, val):
        self.state[1] = val
    @property
    def z(self):
        return self.state[2]
    @z.setter
    def z(self, val):
        self.state[2] = val
    @property
    def vx(self):
        return self.state[3]
    @vx.setter
    def vx(self, val):
        self.state[3] = val
    @property
    def vy(self):
        return self.state[4]
    @vy.setter
    def vy(self, val):
        self.state[4] = val
    @property
    def vz(self):
        return self.state[5]
    @vz.setter
    def vz(self, val):
        self.state[5] = val
    @property
    def ax(self):
        return self.state[6]
    @ax.setter
    def ax(self, val):
        self.state[6] = val
    @property
    def ay(self):
        return self.state[7]
    @ay.setter
    def ay(self, val):
        self.state[7] = val
    @property
    def az(self):
        return self.state[8]
    @az.setter
    def az(self, val):
        self.state[8] = val

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
                'team'  : self.team.id,
            })
        else:
            if type(properties) == str:
                properties = [properties]
            for prop in properties:
                if prop == 'weapons':
                    val = {
                        'weapons': [weapon.json() for weapon in self.weapons],
                        'active' : int(self._active_weapon),
                    }
                elif prop == 'active_weapon':
                    prop = 'weapons'
                    val = {
                        'active'    :   int(self._active_weapon),
                    }
                elif prop == 'team':
                    val = self.team.id
                else:
                    val = getattr(self, prop)

                if type(val) == bool:
                    val = int(val)
                d[prop] = val
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

    def can_take_item(self, item):
        return (item not in self.inventory and item.can_take(self))

    def can_drop_item(self, item):
        return (item in self.inventory and item.can_drop(self))

    def pickup_item(self, item, index=None):
        if self.dead or not self.can_take_item(item):
            return
        if index is None:
            print 'adding %s to inv' % (item,)
            self.inventory.append(item)
        else:
            self.inventory.insert(index, item)
        item.take(self)
        
    def drop_item(self, item):
        if self.can_drop_item(item):
            print 'dropping'
            print self.inventory
            self.inventory.remove(item)
            print self.inventory
            item.drop()

    def near_item(self, item):
        if distance(self.pos(), item.pos()) < item.radius:
            return True
        return False

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

        #if old_state != state or old_angle != angle:
            #NetOut.event.agent_control_state(self)

    def set_angle(self, angle):
        old_angle = self.angle()
        self.x_angle, self.y_angle = angle
        if old_angle != self.angle():
            NetOut.event.agent_angle(self)

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

    def has_flags(self):
        flags = [team.flag for team in GameStateGlobal.teamList.values() if team.flag is not None]
        flags = [flag for flag in flags if flag != self.team.flag]
        held_flags = [flag for flag in self.inventory if flag in flags]
        return held_flags

    def take_damage(self, damage, projectile_owner=None, suicidal=False):
        print 'agent %s taking damage %i' % (self.id, damage,)
        print self.health
        if self.dead:
            return
        # check team kills
        if projectile_owner is not None:
            if not hasattr(projectile_owner, 'id'):
                projectile_owner = GameStateGlobal.playerList[projectile_owner]

            if suicidal and projectile_owner.agent == self:
                pass
            elif projectile_owner.team == self.team and not opts.team_kills:
                print 'team_kill'
                return
            
        old = self.health
        self.health -= damage
        self.health = max(self.health, 0)
        if self.health <= 0:
            self.die(projectile_owner)
        elif self.health != old:
            NetOut.event.agent_update(self, 'health')
        print damage

    def heal_all(self):
        self.heal(self.HEALTH_MAX)

    def restore_ammo(self):
        for weapon in self.weapons:
            weapon.refill()

    def restore_health_and_ammo(self):
        self.heal_all()
        self.restore_ammo()

    def heal(self, amount):
        if not self.dead:
            old = self.health
            self.health = min(self.health + amount, self.HEALTH_MAX)
            if self.health != old:
                NetOut.event.agent_update(self, 'health')

    def die(self, projectile_owner=None, no_score=False, custom_msg=None, respawn_ticks=None):
        if self.dead:
            return
            
        suicide = False
        try:
            you_player = GameStateGlobal.playerList[self.owner]
            you = NetServer.connectionPool.by_client_id(you_player.cid)
            you_player.died(no_score)
        except e:
            print 'Error obtaining client object that owns dying agent.'
            print e
            self.dead = True
            return
        if projectile_owner is None:
            msg = 'You died mysteriously.'
        else:
            try:
                killer = projectile_owner
                if not hasattr(killer, 'id'):
                    killer = GameStateGlobal.playerList[projectile_owner]
                if killer.cid == you_player.cid:
                    msg = 'You killed yourself.'
                    suicide = True
                else:
                    killer.killed()
                    msg = 'You were killed by %s' % (killer.name,)
                if not suicide:
                    try:
                        killer_client = NetServer.connectionPool.by_client_id(killer.cid)
                        kmsg = 'You killed %s' % you.name
                        killer_client.sendMessage.you_killed(kmsg)
                    except e:
                        print 'Killer\'s client was not found'
                        print e
            except KeyError:    # race condition, where client quits before his bullet kills
                msg = 'You were killed by a ghost.'

        if custom_msg is not None:
            msg = custom_msg
        you.sendMessage.you_died(msg)

        self.dead = True
        if respawn_ticks is not None:
            self.respawn_countdown = respawn_ticks
        self.dump_inventory()
        NetOut.event.agent_update(self, ['dead', 'health'])

    def _revive(self):
        self.health = self.HEALTH_MAX
        self.dead = False
        self.respawn_countdown = self.RESPAWN_TICKS

    def _spawn_point(self):
        # later, add spawn zones/ boundaries to spawn in
        x,y,z = terrain_map.map_x, terrain_map.map_y, terrain_map.map_z
        x = randrange(3*(x/8), 5*(x/8))
        y = randrange(3*(x/8), 5*(y/8))
        z = terrain_map.get_highest_open_block(x,y, self.block_height)
        if z < 0:
            z = terrain_map.zmax
        return [x+0.5, y+0.5, z+self.b_height]

    def _set_position(self, pos=None):
        if pos is None:
            pos = self._spawn_point()
        assert len(pos) <= 3
        self.state[0:3] = pos

    def respawn(self): # or can destroy / recreate Agent
        self._revive()
        self._set_position()
        NetOut.event.agent_update(self, ['health', 'dead', 'state'])
