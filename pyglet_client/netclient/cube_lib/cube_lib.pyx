
#core functionality


#constants
cdef enum:
    max_cubes = 4096

 cdef enum:
    x_chunk_size = 8
    y_chunk_size = 8
    z_chunk_size = 8

#structs
cdef struct Vertex:
    float x,y,z
    float tx,ty
    unsigned char r,g,b,a #for packing

cdef struct Quad:
    Vertex vertex[4]

cdef struct Cube:
    Quad quad[6]

cdef struct Chunk_scratch:
    Quad quad[x_chunk_size*y_chunk_size*z_chunk_size]
    int v_num

#globals

cdef quad_cache quad[4096]

cdef float v_index[72] = [
         0,1,1 , 0,0,1 , 1,0,1 , 1,1,1 , #top
         1,0,0 , 0,0,0 , 0,1,0 , 1,1,0 , #bottom
         0,1,1 , 1,1,1 , 1,1,0 , 0,1,0 , #north
         0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , #south
         0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , #west
         1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , #east
    ]

cdef inline set_side(float x, float y, float z, int tile_id, int side_num, Quad* quad):
    (*quad) = quad_cache[6*tile_id + side_num]
    quad->x +=x
    quad->y +=y
    quad->z +=z

#init
def init_quad_cache():
    cdef Quad* quad
    cdef int i,j,index
    for i in range(0,5):
        for j in range(0,4)
            index = 12*i + 4*j
            quad->x = v_index[index + 0]
            quad->y = v_index[index + 1]
            quad->z = v_index[index + 2]
(tv_list, tc_list, ttex_list) = self.cubeRenderCache.get_side(rx, ry, rz, tile_id, side_num)

#    int x_off,y_off,z_off

from libc.stdlib cimport malloc, free

cdef Chunk_scratch * chunk_scratch = <Chunk_scratch *>malloc(sizeof(Chunk_scratch))

