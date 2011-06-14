
#core functionality


#constants
cdef enum:
    max_cubes = 4096

cdef enum:
    x_chunk_size = 8
    y_chunk_size = 8
    z_chunk_size = 8
    chunk_size = 512

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
    Quad quad[chunk_size*6]
    int v_num

#globals

cdef quad_cache quad[max_cubes]

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

cdef inline set_tex(int vert_num, Quad* quad, float x, float y):
    quad.tx = x * (1/8)
    quad.ty = x * (1/8)
    if vert_num == 0:
        quad.tx += 0
        quad.ty += 0
    if vert_num == 1:
        quad.tx += 1/8
        quad.ty += 0
    if vert_num == 2:
        quad.tx += 1/8
        quad.ty += 1/8
    if vert_num == 3:
        quad.tx += 0
        quad.ty += 1/8

def convert_index(index, height, width):
    index = int(index)
    height = int(height)
    width = int(width)
    x_ = index % width
    y_ = int((index - x_) / width)
    y = height - y_ -1
    rvalue =  x_ + y*width
#init
def init_quad_cache():
    cdef Quad* quad
    cdef int i,j,k,index
    for k in range(0, chunk_size):
        for i in range(0,5):
            quad = &quad_cache[6*k+i]
            for j in range(0,4)
                index = 12*i + 4*j
                #vertices
                quad.x = v_index[index + 0]
                quad.y = v_index[index + 1]
                quad.z = v_index[index + 2]
                #colors
                quad.r = 255
                quad.g = 255
                quad.b = 255
                quad.a = 255
                set_tex(j, quad, 0, 3)

#(tv_list, tc_list, ttex_list) = self.cubeRenderCache.get_side(rx, ry, rz, tile_id, side_num)

#    int x_off,y_off,z_off

#from libc.stdlib cimport malloc, free

#cdef Chunk_scratch * chunk_scratch = <Chunk_scratch *>malloc(sizeof(Chunk_scratch))

## control state

def clear_buffer():
    quad_cache.v_num = 0

def add_quad(x,y,z,side,tile):
    quad_cache.v_num += 1
