'''
Agents:
    Objects through which a Player experiences the game world
'''

import math
from math import sin, cos, pi
from math import floor, ceil, fabs

from pyglet.gl import *

from game_state import GameStateGlobal

# represents an agent under control of a player
class Agent:

    HEALTH_MAX = 100
    _RESPAWN_TIME = 1. # seconds
    RESPAWN_TICKS = int(_RESPAWN_TIME / GameStateGlobal.TICK)

    def __init__(self, owner=None, id=None, state=None, weapons=None, health=None, dead=False):
        if owner is None or id is None:
            return
        if weapons is None:
            weapons = []
        if state is None:
            state = [0,0,0,0,0,0,0,0,0]
        state = map(lambda k: float(k), state)
        
        self.state = state #position, velocity, acceleration
        self.xa = state[3]
        self.ya = state[4]

        self.terrainMap = GameStateGlobal.terrainMap

        self.id = id

        self.d_x = 0
        self.d_y = 0
        self.d_xa = 0
        self.d_za = 0

        self.last_control_tick = 0
        self.jetpack = 0
        self.brake = 0

        self.x_int = int(state[0])
        self.y_int = int(state[1])
        self.z_int = int(state[2])

        if health is None:
            self.health = self.HEALTH_MAX
        else:
            self.health = health
        self.dead = bool(dead)
        self.weapons = weapons
        self.owner = owner
        self.you = False

    def update_info(self, **agent):
        args = []
        if 'id' in agent:
            args.append(agent['id'])
            self.id = agent['id']
        if 'health' in agent:
            self.health = agent['health']
        if 'dead' in agent:
            self.dead = bool(agent['dead'])
        if 'weapons' in agent:
            # update weapons accordingly
            weapons = agent['weapons']
        if 'owner' in agent:
            self.owner = agent['owner']
        if 'state' in agent:
            state = agent['state']
            if type(state) == list and len(state) == len(self.state):
                self.state = state

        GameStateGlobal.agentList.update(self, *args)

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
        assert False  #needs to do interopolation

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


class PlayerAgent(Agent):

    def __init__(self, owner=None, id=None, state=None, weapons=None, health=None, dead=False):
        Agent.__init__(self, owner, id, state, weapons, health, dead)

        self.you = True
        self.control_state = [0,0,0,0,0,0,0]
        self.x = -.5
        self.y = -.5
        self.z = -.5
        self.x_angle = 0
        self.y_angle = 0

        self.vx = 0
        self.vy = 0
        self.vz = 0
        self.ax = 0
        self.ay = 0
        self.az = 0

        #settings
        self.b_height = 1.5
        self.t_height = .75
        self.box_r = .30

    def draw(self):
        self.draw_aiming_direction()
        self.draw_bounding_box()
        #self.draw_selected_cube()
        #self.draw_selected_cube2()
        self.draw_position(points=10, seperation = 0.10)
        self.draw_velocity(point_density=15, units=200)
        self.draw_acceleration(point_density=15, units=100000)

    def draw_position(self, points, seperation):
        v_num = 0
        v_list = []
        c_list = []
        for n in range(-points, points):
            temp = float(n)*float(seperation)
            v_list += [self.x+temp, self.y, self.z]
            v_list += [self.x,self.y+temp, self.z]
            v_list += [self.x,self.y, self.z+temp]
            c_list += [255,255,255] + [255,255,255] + [255,255,255]
            v_num +=3
        pyglet.graphics.draw(v_num, GL_POINTS,
        ("v3f", v_list),
        ("c3B", c_list)
        )

    def draw_velocity(self, point_density, units):
        v_num = 0
        v_list = []
        c_list = []
        vlen = units*math.sqrt(self.vx*self.vx + self.vy*self.vy + self.vz*self.vz)
        #print "vlen= " + str(vlen)
        num_points = int(math.floor(point_density*vlen))
        c = 1.0 / float(point_density)
        for n in range(0, num_points):
            v_list += [self.x+units*n*c*self.vx, self.y+units*n*c*self.vy, self.z+units*n*c*self.vz]
            c_list += [0,0,255]
            v_num +=1
        pyglet.graphics.draw(v_num, GL_POINTS,
        ("v3f", v_list),
        ("c3B", c_list)
        )

    def draw_acceleration(self, point_density, units):
        v_num = 0
        v_list = []
        c_list = []
        vlen = units*math.sqrt(self.ax*self.ax + self.ay*self.ay + self.az*self.az)
        #print "alen= " + str(vlen)
        num_points = int(math.floor(point_density*vlen))
        c = 1.0 / float(point_density)
        for n in range(0, num_points):
            v_list += [self.x+units*n*c*self.ax, self.y+units*n*c*self.ay, self.z+units*n*c*self.az]
            c_list += [0,255,0]
            v_num +=1
        pyglet.graphics.draw(v_num, GL_POINTS,
        ("v3f", v_list),
        ("c3B", c_list)
        )

    def draw_aiming_direction(self, distance=50):
        dx = cos( self.x_angle * pi) * cos( self.y_angle * pi)
        dy = sin( self.x_angle * pi) * cos( self.y_angle * pi)
        dz = sin( self.y_angle)

        ep = 0.33
        v_list = []
        v_num = 0
        cf = 0.
        while cf < distance:
            v_num += 1
            x= self.x + cf*dx
            y= self.y + cf*dy
            z= self.z + cf*dz
            cf += ep
            v_list += [x,y,z]
        #print str(v_list)
        #print str(v_num)
        pyglet.graphics.draw(v_num, GL_POINTS,
        ("v3f", v_list),
        ("c3B", [255, 0, 0]*v_num)
        )

    def pan(self, dx_angle, dy_angle):
        self.x_angle += dx_angle
        self.y_angle += dy_angle
        if self.y_angle < -0.499:
            self.y_angle = -0.499
        if self.y_angle > 0.499:
            self.y_angle = 0.499



    def draw_bounding_box(self):
        #agent parameters
        b_height = self.b_height
        t_height = self.t_height
        box_r = self.box_r

        x = self.x
        y = self.y
        z = self.z
        #cordinates for corners
        x_neg = x-box_r
        x_pos = x+box_r
        y_neg = y-box_r
        y_pos = y+box_r

        z0 = z-b_height
        z1 = z
        z2 = z+t_height

        draw_box(x_neg, x_pos, y_neg, y_pos, z0, z1, [255,0,0])
        draw_box(x_neg, x_pos, y_neg, y_pos, z1, z2, [180,0,0])

    def draw_selected_cube(self):
        dx = cos( self.x_angle * pi) * cos( self.y_angle * pi)
        dy = sin( self.x_angle * pi) * cos( self.y_angle * pi)
        dz = sin( self.y_angle)

        l_list = []
        for ix in range (0, 5):
            if dx*dx < 0.00000001: #prevent division by zero errors
                continue
            l = (ix-self.x)/fabs(dx)
            (x_,y_,z_) = (dx*l+self.x, dy*l+self.y, dz*l+self.z)
            if dx < 0:
                draw_cube(x_-1, floor(y_), floor(z_), 2) #north
            elif dx >= 0:
                draw_cube(x_, floor(y_), floor(z_), 3) #south

        for iy in range(0,5):
            if dy*dy < 0.00000001: #prevent division by zero errors
                continue
            l = (iy-self.x)/fabs(dy)
            (x_,y_,z_) = (dx*l+self.x, dy*l+self.y, dz*l+self.z)
            if dy < 0:
                draw_cube(floor(x_), y_-1, floor(z_), 4) #west
            elif dy >= 0:
                draw_cube(floor(x_), y_, floor(z_), 5) #east

        if dx < 0:
            pass #north side

    def draw_selected_cube2(self, distance = 20):
        dx = cos( self.x_angle * pi) * cos( self.y_angle * pi)
        dy = sin( self.x_angle * pi) * cos( self.y_angle * pi)
        dz = sin( self.y_angle)

        cube_sides = []

        l= 0.
        ix = 0.
        while l < distance:
            if dx*dx < 0.00000001: #prevent division by zero errors
                break
            l = (ix-self.x)/fabs(dx)
            (x_,y_,z_) = (dx*l+self.x, dy*l+self.y, dz*l+self.z)
            if dx < 0:
                cube_sides.append((x_-1, floor(y_), floor(z_), 2)) #north
            elif dx >= 0:
                cube_sides.append((x_, floor(y_), floor(z_), 3)) #south
            ix += 1

        l=0.
        iy = 0.
        while l < distance:
            if dy*dy < 0.00000001: #prevent division by zero errors
                break
            l = (iy-self.x)/fabs(dy)
            (x_,y_,z_) = (dx*l+self.x, dy*l+self.y, dz*l+self.z)
            if dy < 0:
                cube_sides.append((floor(x_), y_-1, floor(z_), 4)) #west
            elif dy >= 0:
                cube_sides.append((floor(x_), y_, floor(z_), 5)) #east
            iy += 1

        l=0.
        iz = 0.
        while l < distance:
            if dz*dz < 0.00000001: #prevent division by zero errors
                break
            l = (iz-self.x)/fabs(dz)
            (x_,y_,z_) = (dx*l+self.x, dy*l+self.y, dz*l+self.z)
            if dz < 0:
                cube_sides.append((floor(x_), floor(y_), z_-1, 0)) #top
            elif dz >= 0:
                cube_sides.append((floor(x_), floor(y_), z_, 1)) #bottom
            iz += 1

        draw_sides(cube_sides)

    #Deprecate?
#    def _append_side(self, cube_dict, x,y,z,side):
#        (x,y,z) = (int(round(x)),int(round(y)), int(round(z)))
#        if not cube_dict.has_key((x,y,z)):
#            cube_dict[(x,y,z)] = []
#        cube_dict[(x,y,z)].append(side)



### DRAWING STUFF ####

    #axis aligned
def draw_box(x_neg, x_pos, y_neg, y_pos, z_neg, z_pos, color = [255,0,0]):

    v_list = []
    c_list = []
    v_num = 0

    vi_list = [
    [0, 0, 0], [1, 0, 0], [1, 0, 0], [1, 1, 0],
    [1, 1, 0], [0, 1, 0], [0, 1, 0], [0, 0, 0],
    [0, 0, 1], [1, 0, 1], [1, 0, 1], [1, 1, 1],
    [1, 1, 1], [0, 1, 1], [0, 1, 1], [0, 0, 1],
    [0, 0, 0], [0, 0, 1], [1, 0, 0], [1, 0, 1],
    [1, 1, 0], [1, 1, 1], [0, 1, 0], [0, 1, 1]
    ]

    v_t0 = [None, None, None]
    for v_t1 in vi_list:

        v_t0[0] = x_neg if v_t1[0]==0 else x_pos
        v_t0[1] = y_neg if v_t1[1]==0 else y_pos
        v_t0[2] = z_neg if v_t1[2]==0 else z_pos

        v_list += v_t0
        c_list += color
        v_num += 1

    pyglet.graphics.draw(v_num, GL_LINES,
    ("v3f", v_list),
    ("c3B", c_list)
    )

def draw_sides( p_list):

    v_set = [
        [0,0,0],
        [1,0,0],
        [1,1,0],
        [0,1,0],
        [0,0,1],
        [1,0,1],
        [1,1,1],
        [0,1,1]
    ]

    vertex_index = [
        [0,1],
        [1,2],
        [2,3],
        [3,0],

        [4,5],
        [5,6],
        [6,7],
        [7,4],

        [0,4],
        [1,5],
        [2,6],
        [3,7],
    ]

    side_v = [
    [4,5,6,7],   #top (z=1)
    [0,1,2,3],   #bottom (z=0)
    [1,5,9,10],  #north (y=1)
    [7,3,11,8],  #south (y=0)
    [6,2,10,11], #west (x=0)
    [4,0,9,8],   #east (x=1)
    ]


    v_list = []
    c_list = []
    v_num = 0

    for (x,y,z,side) in p_list:
        for k in side_v[side]:
            [i,j] = vertex_index[k]
            v_num += 2
            v_list += [ v_set[i][0]+x, v_set[i][1]+y, v_set[i][2]+z ]
            v_list += [ v_set[j][0]+x, v_set[j][1]+y, v_set[j][2]+z ]
            c_list += [150,0,0]*2

    pyglet.graphics.draw(v_num, GL_LINES,
    ("v3f", v_list),
    ("c3B", c_list)
    )

def draw_cube( x,y,z, side = None):

    v_set = [
        [0,0,0],
        [1,0,0],
        [1,1,0],
        [0,1,0],
        [0,0,1],
        [1,0,1],
        [1,1,1],
        [0,1,1]
    ]

    vertex_index = [
        [0,1],
        [1,2],
        [2,3],
        [3,0],

        [4,5],
        [5,6],
        [6,7],
        [7,4],

        [0,4],
        [1,5],
        [2,6],
        [3,7],
    ]

    side_v = [
    [4,5,6,7],   #top (z=1)
    [0,1,2,3],   #bottom (z=0)
    [1,5,9,10],  #north (y=1)
    [7,3,11,8],  #south (y=0)
    [6,2,10,11], #west (x=0)
    [4,0,9,8],   #east (x=1)
    ]

    #(x,y,z) = (floor(self.x), floor(self.y), floor(self.z))
    v_list = []
    c_list = []
    v_num = 0
    for index,[i,j] in enumerate(vertex_index):
        v_num += 2
        v_list += [ v_set[i][0]+x, v_set[i][1]+y, v_set[i][2]+z ]
        v_list += [ v_set[j][0]+x, v_set[j][1]+y, v_set[j][2]+z ]

        if side != None and index in side_v[side]:
            c_list += [155,0,0]*2
            #print str(j)
        else:
            c_list += [0,155,155]*2

    pyglet.graphics.draw(v_num, GL_LINES,
    ("v3f", v_list),
    ("c3B", c_list)
    )

from net_out import NetOut
