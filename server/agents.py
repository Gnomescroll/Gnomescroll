'''
Agents:
    Objects through which a Player experiences the game world
'''
import opts
opts = opts.opts

from math import floor, ceil, pow
from math import pi, cos, sin
from random import randrange

from vector_lib import distance
from weapons import LaserGun, Pick, BlockApplier
from game_state import GameStateGlobal
from object_lists import GenericObjectList
from net_out import NetOut
from net_server import NetServer
from c_lib.c_lib_agents import AgentListWrapper, AgentWrapper, teleport_Agent
import c_lib.terrain_map as terrain_map

collisionDetection = terrain_map.collisionDetection

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

        z_min = z
        z_max = z + self.b_height
        x_max = x + self.box_r
        x_min = x - self.box_r
        y_max = y + self.box_r
        y_min = y - self.box_r

        if x_min < x_ and x_ < x_max and y_min < y_ and y_ < y_max and z_min < z_ and z_ < z_max:
            return True
        else:
            return False

    def sphere_collision_test(self, x,y,z,r):
        pass

    def _tick_physics(self):
        return


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
        pl = GameStateGlobal.projectileList
        pos = self.pos()
        pos[2] += self.b_height * 0.75
        state = pos + direction
        grenade = pl.create('Grenade', state, owner=self.owner, ttl=0)
        #NetOut.event.projectile_create(grenade)


# represents an agent under control of a player
class Agent(AgentWrapper, AgentPhysics, AgentAction):

    HEALTH_MAX = 100
    _RESPAWN_TIME = 2000  # milliseconds
    _TICK_RATE = 30       # milliseconds
    RESPAWN_TICKS = int(float(_RESPAWN_TIME) / float(_TICK_RATE))

    def __init__(self, player_id, position=None, id=None, team=None):

        ### Global imports ###
        self.terrainMap = GameStateGlobal.terrainMap
        ### End Global imports ###

        if id is not None:
            print "SOMETHING IS GIVING SERVER AGENT AN ID"
            raise Exception

        AgentWrapper.__init__(self)
        
        self.team = team

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

    def __getattribute__(self, name):
        try:
            val = AgentWrapper.__getattribute__(self, name)
        except AttributeError:
            val = object.__getattribute__(self, name)
        return val

    def block_height(self):
        return int(ceil(self.b_height))

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
    def state(self):
        return [self.x, self.y, self.z, self.vx, self.vy, self.vz, self.ax, self.ay, self.az]
    @state.setter
    def state(self, val):
        self.x, self.y, self.z, self.vx, self.vy, self.vz, self.ax, self.ay, self.az = val

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
        b_height = int(self.block_height())
        b_height = min(1, b_height)
        z = terrain_map.get_highest_open_block(x,y, b_height)
        if z < 0:
            z = terrain_map.zmax
        return [x+0.5, y+0.5, z+self.b_height]

    def _set_position(self, pos=None):
        if pos is None:
            pos = self._spawn_point()
        x,y,z = map(float, pos)
        teleport_Agent(self.id, x,y,z)
        #self.pos(pos)

    def respawn(self): # or can destroy / recreate Agent
        self._revive()
        self._set_position()
        NetOut.event.agent_update(self, ['health', 'dead', 'state'])
