

cdef extern from "ray_trace/ray_trace.h":
    int _ray_cast6(float x0,float y0,float z0, float _dfx,float _dfy,float _dfz, float max_l, float *distance, int* collision, int* pre_collision, int* tile, int* side)
    int* _nearest_block(float x, float y, float z, float vx, float vy, float vz, float max_distance, int z_low, int z_high)
    int* _farthest_empty_block(float x, float y, float z, float vx, float vy, float vz, float max_distance, int z_low, int z_high)


def ray_cast6(float x0,float y0,float z0, float dx,float dy,float dz, float max_l):
    cdef float distance
    cdef int collision[3]
    cdef int pre_collision[3]
    cdef int tile
    cdef int side[3]
    cdef int ret
    ret = _ray_cast6(x0,y0,z0, dx,dy,dz, max_l, &distance, collision, pre_collision, &tile , side)
    if(ret == 0):
        #print "No Collision with Block"
        return None
    else:
        #print "dist= %i,%i,%i, %i,%i,%i" % (collision[0],collision[1],collision[2], pre_collision[0],pre_collision[1],pre_collision[2])
        #print "distance= %f tile=%i, side=%i,%i,%i" % (distance, tile, side[0],side[1],side[2])
        return [collision[0],collision[1],collision[2],pre_collision[0],pre_collision[1],pre_collision[2], side[0],side[1],side[2]]

def nearest_block(pos, vector, float max_distance=4., int z_low=4, int z_high=3):
    cdef int* i
    cdef float x
    cdef float y
    cdef float z
    cdef float vx
    cdef float vy
    cdef float vz

    x,y,z = pos
    vx,vy,vz = vector
    
    i = _nearest_block(x,y,z, vx,vy,vz, max_distance, z_low, z_high)
    if i == NULL:
        return
    return [i[0], i[1], i[2]]

def farthest_empty_block(pos, vector, float max_distance=4., int z_low=4, int z_high=3):
    cdef int* i
    cdef float x
    cdef float y
    cdef float z
    cdef float vx
    cdef float vy
    cdef float vz

    x,y,z = pos
    vx,vy,vz = vector
    
    i = _farthest_empty_block(x,y,z, vx,vy,vz, max_distance, z_low, z_high)
    if i == NULL:
        return
    return [i[0], i[1], i[2]]

