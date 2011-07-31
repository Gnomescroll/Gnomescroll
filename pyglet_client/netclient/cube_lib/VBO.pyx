

#core functionality

from cube_lib.types cimport *

#cimport cube_lib.cube_dat
cimport cube_lib.terrain_map
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
                #set_tex(j, vertex, 0, 0)

                tx,ty = get_cube_texture(k, i, j) #tile_id, side, vert_num

                vertex.tx = tx
                vertex.ty = ty
                #print "(%i,%i,%i)" % (k,i,j)
                #print "(tx= %f,ty= %f)" %(quad_cache[6*k+i].vertex[j].tx,quad_cache[6*k+i].vertex[j].ty)
    print "done"

#from libc.stdlib cimport memcpy

cdef extern from "stdlib.h":
    void* memcpy(void* source, void* dest, int size)

import time

cdef inline set_side(float x, float y, float z, int side_num, int tile_id, Quad* quad):
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


cdef inline add_quad(float x,float y,float z,int side,int tile):
    global chunk_scratch
    cdef Quad* quad = &chunk_scratch.quad[chunk_scratch.v_num]
    chunk_scratch.v_num += 1 #quads have 4 vertex
    set_side(x,y,z, side, tile, quad)

def init():
    _init_draw_terrain()
    init_cubes()
    init_quad_cache()
    clear_chunk_scratch()

cdef inline clear_chunk_scratch():
    global chunk_scratch
    chunk_scratch.v_num = 0

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
    int _start_vbo_draw()
    int _draw_vbo(Quad_VBO* q_VBO)
    int _end_vbo_draw()



test_var = 0
cdef int v_num = 0
cdef int vbo_id = 0

#deprecate
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
    ll = terrain_map.get_raw_chunk_list()
    for l in ll:
        mc = <MapChunk>l
        if mc.update_VBO != 0:
            #if mc.VBO.v_num != 0:
            #    delete_VBO(mc)
            print "updating VBO"
            update_VBO(mc)
            #print "updating VBO"
            break
            #load VBO

def draw_chunks():
    cdef MapChunk mc
    ll = terrain_map.get_raw_chunk_list()
    #print "Draw"
    #cube_lib.terrain_map.set(5,5,0,1)
    #for z in range (-16, 16):
    #    if cube_lib.terrain_map.get(5,5,z) != 0:
    #        print "non zero!"

    for l in ll:
        mc = <MapChunk>l
        if mc.VBO.VBO_id != 0:
            _draw_vbo(&mc.VBO)
            #print "VBO_id, v_num= %i, %i" % (mc.VBO.VBO_id, mc.VBO.v_num)
    _end_vbo_draw()
## Draw VBO ##

cdef update_VBO(MapChunk mc):
    global chunk_scratch
    cdef int tile_id, x, y, z, side_num
    cdef float x_off, y_off, z_off
    cdef int x_, y_, z_
    cdef int active_cube_num

    x_off = mc.index[0]
    y_off = mc.index[1]
    z_off = mc.index[2]

    print "1,2,3 = %f, %f, %f" % (x_off, y_off, z_off)
    clear_chunk_scratch()
    mc.update_VBO = 0
    if mc.VBO.v_num != 0:
        delete_VBO(mc)
    active_cube_num = 0
    #print "ysa= %i" % (x_chunk_size)
    for x_ in range(0, x_chunk_size):
        for y_ in range(0, y_chunk_size):
            for z_ in range(0, z_chunk_size):
                tile_id = cube_lib.terrain_map.get(x_+mc.index[0],y_+mc.index[1],z_+mc.index[2])
                #if tile_id != 0:
                #    print "tile, active= %i, %i" %(tile_id, isActive(tile_id))
                if isActive(tile_id) != 0: #non-active tiles are not draw
                    active_cube_num += 1 #comment out
                    for side_num in [0,1,2,3,4,5]: #[1]:#
                        #if not _is_occluded(x_+mc.index[0],y_+mc.index[1],z_+mc.index[2],side_num): #ints
                        if True:
                            add_quad(x_+x_off,y_+y_off,z_+z_off,side_num,tile_id) #floats

    print "v_num for chunk scratch = %i" % (chunk_scratch.v_num)
    print "active cubes= %i" % (active_cube_num)
    #print "3"
    #mc.VBO.v_num = chunk_scratch.v_num
    #print "4"
    _create_vbo(&mc.VBO, chunk_scratch.quad, chunk_scratch.v_num)
    print "VBO_id= %i" % (mc.VBO.VBO_id)
    #print "5"

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
    #properties
    active = int(d.get('active',1))
    occludes = int(d.get('occludes', 0))
    solid = int(d.get('solid', 1))
    gravity = int(d.get('gravity', 0))
    transparent = int(d.get('transparent', 0))
    init_CubePhysical(&cube_array[id], id, active, occludes, solid, gravity, transparent)

def get_cube_texture(tile_id, side, vert_num):
    global cube_list
    d = cube_list.get(tile_id, {})
    assert vert_num < 4 and side < 6
    #print "side, vert_num = %i, %i" % (side, vert_num)
    texture_id = d.get('texture_id', [0,1,2,3,4,5])[side]
    texture_order = d.get('texture_order', [[0,1,2,3],
                            [0,1,2,3],
                            [0,1,2,3],
                            [0,1,2,3],
                            [0,1,2,3],
                            [0,1,2,3],])[side][vert_num]

    x = texture_id % 16
    y = (texture_id - (texture_id % 16)) / 16

    tx = float(x) * 1./16.
    ty = float(y) * 1./16.

    if vert_num == 0:
        tx += 0
        ty += 0
    elif vert_num == 1:
        tx += 1./16.
        ty += 0
    elif vert_num == 2:
        tx += 1./16.
        ty += 1./16.
    elif vert_num == 3:
        tx += 0
        ty += 1./16.
    else:
        print "Error!!!! set_tex invalid input"
        assert False

    return (tx,ty)

#!!!should not need to be cp
cpdef inline int isActive(unsigned int id):
    if id >= max_cubes: #max number of cubes
        return 0
        print "max cubes exceeded!"
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
