from math import sin, cos, pi
from math import floor, ceil, fabs
from pyglet.gl import *

class Player:

    def __init__(self, main=None):
        self.main = main

        self.height = 1.7
        self.radius = 0.4
        self.speed = .8 #meters per second
        self.onGround = True

        self.jump_power = 0.5
        self.vx = 0.
        self.vy = 0.
        self.vz = 0.

        self.x = -.5
        self.y = -.5
        self.z = -.5
        self.x_angle = 0
        self.y_angle = 0

    def tick(self, gravity, v_drag, jump):
        drag_v = v_drag * -fabs(self.vz)
        if jump == True:
            jump_v = self.jump_power
        else:
            jump_v = 0
        self.vz += gravity + jump_v + drag_v


    def draw(self):
        self.draw_aiming_direction()
        #self.draw_selected_cube()
        self.draw_player_bounding_box()
        self.draw_selected_cube2()

    def draw_player_bounding_box(self):
        v_sets = [
        [ [0,1,1] , [0,0,1] , [1,0,1] , [1,1,1] ], #top
        [ [1,0,0] , [0,0,0] , [0,1,0] , [1,1,0] ], #bottom
        [ [0,1,1] , [1,1,1] , [1,1,0] , [0,1,0] ], #north
        [ [0,0,1] , [0,0,0] , [1,0,0] , [1,0,1] ], #south
        [ [0,1,1] , [0,1,0] , [0,0,0] , [0,0,1] ], #west
        [ [1,0,1] , [1,0,0] , [1,1,0] , [1,1,1] ], #east
    ]

        (x,y,z) = (floor(self.x), floor(self.y), floor(self.z))
        v_list = []
        v_num = 0
        for v_set in v_sets:
            for i in [0,1,2,3]:
                v_num += 2
                v_list += [v_set[i][0]+x, v_set[i][1]+y, v_set[i][2]+z]
                v_list += [v_set[(i+1)%4][0]+x, v_set[(i+1)%4][1]+y, v_set[(i+1)%4][2]+z]

        pyglet.graphics.draw(v_num, GL_LINES,
        ("v3f", v_list),
        ("c3B", [255, 255, 0] *v_num)
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

    def draw_sides(self, p_list):

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

    def draw_cube(self, x,y,z, side = None):

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
                self.draw_cube(x_-1, floor(y_), floor(z_), 2) #north
            elif dx >= 0:
                self.draw_cube(x_, floor(y_), floor(z_), 3) #south

        for iy in range(0,5):
            if dy*dy < 0.00000001: #prevent division by zero errors
                continue
            l = (iy-self.x)/fabs(dy)
            (x_,y_,z_) = (dx*l+self.x, dy*l+self.y, dz*l+self.z)
            if dy < 0:
                self.draw_cube(floor(x_), y_-1, floor(z_), 4) #west
            elif dy >= 0:
                self.draw_cube(floor(x_), y_, floor(z_), 5) #east

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

        self.draw_sides(cube_sides)

    def _append_side(self, cube_dict, x,y,z,side):
        (x,y,z) = (int(round(x)),int(round(y)), int(round(z)))
        if not cube_dict.has_key((x,y,z)):
            cube_dict[(x,y,z)] = []
        cube_dict[(x,y,z)].append(side)
