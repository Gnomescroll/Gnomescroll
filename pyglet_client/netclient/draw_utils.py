'''
Drawing/rendering utilities
'''

import pyglet
from pyglet.gl import *

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

def draw_cube( x,y,z, color = None):

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

    #side_v = [
    #[4,5,6,7],   #top (z=1)
    #[0,1,2,3],   #bottom (z=0)
    #[1,5,9,10],  #north (y=1)
    #[7,3,11,8],  #south (y=0)
    #[6,2,10,11], #west (x=0)
    #[4,0,9,8],   #east (x=1)
    #]

    #(x,y,z) = (floor(self.x), floor(self.y), floor(self.z))
    v_list = []
    c_list = []
    v_num = 0
    for index,[i,j] in enumerate(vertex_index):
        v_num += 2
        v_list += [ v_set[i][0]+x, v_set[i][1]+y, v_set[i][2]+z ]
        v_list += [ v_set[j][0]+x, v_set[j][1]+y, v_set[j][2]+z ]

        if color == None:
            c_list += [155,0,0]*2
        else:
            c_list += color*2

    pyglet.graphics.draw(v_num, GL_LINES,
    ("v3f", v_list),
    ("c3B", c_list)
    )
