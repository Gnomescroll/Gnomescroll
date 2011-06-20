from libc.stdlib cimport malloc, free

#core functionality

from cube_lib.types cimport Quad_VBO, Vertex, Quad

cimport cube_lib.cube_dat as cube_dat
cimport cube_lib.terrain_map as terrain_map

from terrain_map cimport MapChunk

#cimport cube_lib.types
from cube_lib.types cimport Vertex, Quad

#constants
cdef enum:
    max_cubes = 1024

cdef enum:
    x_chunk_size = 8
    y_chunk_size = 8
    z_chunk_size = 8
    chunk_size = 512

cdef struct Chunk_scratch:
    Quad quad[chunk_size*6] #6 quad per cube
    int v_num

#cdef struct Quad_cache:
#   Quad quad[max_cubes*6]  #6 quad per cube

#globals


cdef Chunk_scratch* chunk_scratch
chunk_scratch = <Chunk_scratch *>malloc(sizeof(Chunk_scratch))

cdef Quad* quad_cache
quad_cache = <Quad *>malloc(max_cubes*6 * sizeof(Quad))
#quad_cache = <Quad *>malloc(1024*6 * sizeof(Quad))

#cdef Quad_cache quad_cache2

#print "size = " + str(sizeof(max_cubes*6))
#quad_cache = <Quad *>malloc(max_cubes*6*sizeof(Quad))

cdef float * v_index
v_index = <float*> malloc(72*sizeof(float))

l = [
         0,1,1 , 0,0,1 , 1,0,1 , 1,1,1 , #top
         1,0,0 , 0,0,0 , 0,1,0 , 1,1,0 , #bottom
         0,1,1 , 1,1,1 , 1,1,0 , 0,1,0 , #north
         0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , #south
         0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , #west
         1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , #east
    ]

for i in range(0, 72):
    v_index[i] = float(l[i])

cdef inline set_tex(int vert_num, Vertex* vertex, float x, float y):
    vertex.tx = x * (float(1)/16)
    vertex.ty = x * (float(1)/16)
    if vert_num == 0:
        vertex.tx += 0
        vertex.ty += 0
    elif vert_num == 1:
        vertex.tx += float(1)/16
        vertex.ty += 0
    elif vert_num == 2:
        vertex.tx += float(1)/16
        vertex.ty += float(1)/16
    elif vert_num == 3:
        vertex.tx += 0
        vertex.ty += float(1)/16
    else:
        print "Error!!!! set_tex invalid input"
    #print "!!! (tx= %f,ty= %f)" %(vertex.tx, vertex.ty)

def convert_index(index, height, width):
    index = int(index)
    height = int(height)
    width = int(width)
    x_ = index % width
    y_ = int((index - x_) / width)
    y = height - y_ -1
    rvalue =  x_ + y*width
#init

cpdef test():
    global v_index, quad_cache
    cdef int i
    print str(max_cubes)
    for i in range(0, max_cubes*6):
        print str(i)
        #print str(quad_cache[i].vertex.r)
        quad_cache[i].vertex[0].y = 5

def init_quad_cache():
    global v_index, quad_cache
    #quad_cache = <Quad *>malloc(max_cubes*6 * sizeof(Quad))
    #print "max cubes= " + str(max_cubes)
    #print "Size of Quad=" + str(sizeof(Quad))
    cdef Quad* quad
    cdef Vertex* vertex
    cdef int i,j,k,index
    for k in range(0, max_cubes):
        for i in range(0,6):
            quad = &quad_cache[6*k+i]
            for j in range(0,4):
                #print "(%i,%i,%i)" % (k,i,j)
                index = 12*i + 3*j
                vi = 6*k+i
                vertex = &quad.vertex[j]
                #vertices
                quad_cache[6*k+i].vertex[j].x = v_index[index + 0]
                quad_cache[6*k+i].vertex[j].y = v_index[index + 1]
                quad_cache[6*k+i].vertex[j].z = v_index[index + 2]
                ##vertex.x = v_index[index + 0]
                ##vertex.y = v_index[index + 1]
                ##vertex.z = v_index[index + 2]
                #colors
                vertex.r = 255
                vertex.g = 255
                vertex.b = 255
                vertex.a = 255
                #tex
                set_tex(j, vertex, 0, 0)
                #print "(%i,%i,%i)" % (k,i,j)
                #print "(tx= %f,ty= %f)" %(quad_cache[6*k+i].vertex[j].tx,quad_cache[6*k+i].vertex[j].ty)
    print "done"

#from libc.stdlib cimport memcpy

cdef extern from "stdlib.h":
    void* memcpy(void* source, void* dest, int size)

import time

cdef inline set_side(float x, float y, float z, int tile_id, int side_num, Quad* quad):
    global quad_cache
    cdef int i
    #cdef Vertex* vertex
    #cdef Quad* quad2 = &quad_cache[6*tile_id + side_num]
    memcpy(quad, &quad_cache[6*tile_id + side_num], sizeof(Quad))
    for i in range(0,4):
        #print "(tx= %f,ty= %f)" %(quad_cache[6*tile_id + side_num].vertex[i].tx,quad_cache[6*tile_id + side_num].vertex[i].ty) + " tx,ty= %f, %f" %(quad.vertex[i].tx, quad.vertex[i].ty)
        quad.vertex[i].x += x
        quad.vertex[i].y += y
        quad.vertex[i].z += z
        #time.sleep(1.)

#(tv_list, tc_list, ttex_list) = self.cubeRenderCache.get_side(rx, ry, rz, tile_id, side_num)

#    int x_off,y_off,z_off

#from libc.stdlib cimport malloc, free

#cdef Chunk_scratch * chunk_scratch = <Chunk_scratch *>malloc(sizeof(Chunk_scratch))

## control state

def init():
    print "1"
    init_quad_cache()
    print "2"
    clear_chunk_scratch()

cdef inline clear_chunk_scratch():
    global chunk_scratch
    chunk_scratch.v_num = 0

cdef add_quad(float x,float y,float z,int side,int tile):
    global chunk_scratch
    cdef Quad* quad = &chunk_scratch.quad[chunk_scratch.v_num]
    chunk_scratch.v_num += 1 #quads have 4 vertex
    set_side(x,y,z, tile, side, quad)

#testing
def test_chunk():
    global quad_cache,chunk_scratch
    chunk_scratch.v_num =0
    cdef int i
    for i in range(0,15):
        for j in range(0,6):
            add_quad(1,1,i-4,j,3)

#cimport SDL.gl
#import SDL.gl
#from SDL.gl cimport bind_VBO

#cdef exern _bind_VBO(Quad* quad_list, int v_num)

#extern from 'draw_functions.h':
#    cdef exern _bind_VBO(Quad* quad_list, int v_num)


cdef extern from 'draw_terrain.h':
    int _init_draw_terrain()

    int _create_vbo(Quad_VBO* q_VBO, Quad* quad_list, int v_num)
    int _delete_vbo(Quad_VBO* q_VBO)
    int _draw_vbo(Quad_VBO* q_VBO)


test_var = 0
cdef int v_num = 0
cdef int vbo_id = 0

def draw_test_chunk():
    return
    global chunk_scratch
    global test_var,v_num,vbo_id
    cdef Quad* quad_list
    if test_var == 0:
        #init
        _init_draw_terrain()
        #create VBO
       # quad_list = chunk_scratch.quad
        #v_num = chunk_scratch.v_num
        #vbo_id = _create_vbo(quad_list, v_num)

    #_draw_vbo(vbo_id, v_num)

    #cdef Quad* quad_list = chunk_scratch.quad
    #cdef int v_num = chunk_scratch.v_num
    #SGL.gl._bind_VBO(quad_list, v_num)
    #bind_VBO(quad_list, v_num)


### TEST ###

#cdef int update_VBO
#cdef Quad_VBO VBO

#    int v_num
#    Quad* quad_array
#    int VBO_id

from terrain_map cimport MapChunk
from cube_lib.types cimport Quad_VBO

#from cube_dat cimport cubePhysicalProperties

cdef update_VBO(MapChunk mc):
    global chunk_scratch
    cdef int tile_id, x, y, z, side_num
    cdef float x_off, y_off, z_off

    x_off = mc.index[0]
    y_off = mc.index[1]
    z_off = mc.index[2]

    clear_chunk_scratch()
    mc.update_VBO = 0
    if mc.VBO.v_num != 0:
        delete_VBO(mc)

    for x in range(0, x_chunk_size):
        for y in range(0, y_chunk_size):
            for z in range(0, z_chunk_size):
                tile_id = terrain_map.get(x,y,z)
                ###
                if cube_dat.cubePhysicalProperties.isActive(tile_id) != 0: #non-active tiles are not draw
                    active_cube_number += 1
                    for side_num in [0,1,2,3,4,5]:
                        if not _is_occluded(x,y,z,side_num):
                            add_quad(x+x_off,y_off,z_off,side_num,tile_id)

    mc.VBO.v_num = chunk_scratch.v_num
    mc.VBO.VBO_id = _create_vbo(&mc.VBO, chunk_scratch.quad, chunk_scratch.v_num)

cdef delete_VBO(MapChunk mc):
    #free(mc.VBO.quad_array)
    mc.VBO.VBO_id = 0
    mc.VBO.v_num = 0
    _delete_vbo(&mc.VBO)

l = [0,0,1, 0,0,-1, 0,1,0, 0,-1,0, -1,0,0, 1,0,0]
cdef int s_array[3*6]
for i in range(0, 3*6):
    s_array[i] = l[i]

cdef inline _is_occluded(int x,int y,int z, int side_num):
        global s_array, cubePhysicalProperties
        cdef int _x, _y, _z, tile_id,i

        i = s_array[3*side_num]
        _x = s_array[i+0] + x
        _y = s_array[i+1] + y
        _z = s_array[i+2] + z

        tile_id = terrain_map.get(_x,_y,_z)
        cube_dat.cubePhysicalProperties.isOcclude(tile_id)

