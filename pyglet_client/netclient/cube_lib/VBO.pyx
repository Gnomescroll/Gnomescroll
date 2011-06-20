from libc.stdlib cimport malloc, free

#core functionality

from cube_lib.types cimport *

#cimport cube_lib.cube_dat
cimport cube_lib.terrain_map as terrain_map

#import cube_lib.terrain_map

from cube_lib.terrain_map cimport MapChunk

from cube_dat import cube_list

#from cube_dat cimport cubePhysicalProperties
#from cube_lib.cube_dat cimport cubePhysicalProperties

#import cube_lib.cube_dat.cubePhysicalProperties as cubePhysicalProperties

#constants
#cdef enum:
#    max_cubes = 1024

#cdef enum:
#    x_chunk_size = 8
#    y_chunk_size = 8
#    z_chunk_size = 8
#    chunk_size = 512

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
    init_cubes()
    init_quad_cache()
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

        _init_draw_terrain()


## Update VBO ##
def update_chunks():
    cdef MapChunk mc
    ll = terrain_map.get_chunk_list()
    for l in ll:
        mc = <MapChunk>l
        if mc.update_VBO != 0:
            if mc.VBO.v_num != 0:
                delete_VBO(mc)
            update_VBO(mc)
            print "updating VBO"
            #load VBO

def draw_chunks():
    cdef MapChunk mc
    ll = terrain_map.get_chunk_list()
    for l in ll:
        mc = <MapChunk>l
        if mc.VBO.VBO_id != 0:
            pass
            print "draw!"
## Draw VBO ##

cdef update_VBO(MapChunk mc):
    global chunk_scratch
    cdef int tile_id, x, y, z, side_num
    cdef float x_off, y_off, z_off

    x_off = mc.index[0]
    y_off = mc.index[1]
    z_off = mc.index[2]

    clear_chunk_scratch()
    mc.update_VBO = 0
    #if mc.VBO.v_num != 0:
    #    delete_VBO(mc)

    for x in range(0, x_chunk_size):
        for y in range(0, y_chunk_size):
            for z in range(0, z_chunk_size):
                tile_id = terrain_map.get(x,y,z)
                ###
                if isActive(tile_id) != 0: #non-active tiles are not draw
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
        global s_array
        cdef int _x, _y, _z, tile_id,i

        i = s_array[3*side_num]
        _x = s_array[i+0] + x
        _y = s_array[i+1] + y
        _z = s_array[i+2] + z

        tile_id = terrain_map.get(_x,_y,_z)
        return isOcclude(tile_id)

## cube physical

cdef struct CubePhysical:
    int id
    int active
    int occludes
    int solid
    int gravity
    int transparent

#used for initing the struct
cdef void init_CubePhysical(CubePhysical*x, int id, int active, int occludes, int solid, int gravity, int transparent):
    x.id = id
    x.active = active
    x.occludes = occludes
    x.solid = solid
    x.gravity = gravity
    x.transparent = transparent

#cdef enum:
#    max_cubes = 1024

def init_cubes():
    global cube_list
    for cube in cube_list.values():
        add_cube(cube)

cdef CubePhysical cube_array[max_cubes] #cube state

def add_cube(d):
    id = int(d['id'])
    if id >= max_cubes: #max number of cubes
        print "Error: cube id is too high"
        return
    active = int(d.get('active',1))
    occludes = int(d.get('occludes', 0))
    solid = int(d.get('solid', 1))
    gravity = int(d.get('gravity', 0))
    transparent = int(d.get('transparent', 0))
    init_CubePhysical(&cube_array[id], id, active, occludes, solid, gravity, transparent)

#!!!should not need to be cp
cpdef inline int isActive(unsigned int id):
    if id >= max_cubes: #max number of cubes
        return 0
    return cube_array[id].active
#!!!should not need to be cp
cpdef inline int isOcclude(unsigned int id):
    if id >= max_cubes: #max number of cubes
        return 0
    return cube_array[id].occludes

cpdef inline int isTransparent(unsigned int id):
    if id >= max_cubes: #max number of cubes
        return 0
    return cube_array[id].transparent

cpdef inline int isSolid(unsigned int id):
    if id >= max_cubes: #max number of cubes
        return 0
    return cube_array[id].solid


#cdef CubePhysicalProperties* cubePhysicalProperties
#cdef CollisionDetection collisionDetection

### Cube Utilities ###

cpdef inline int collisionDetection(int x, int y, int z):
    cdef int tile
    tile = terrain_map.get(x,y,z)
    return isSolid(tile)
