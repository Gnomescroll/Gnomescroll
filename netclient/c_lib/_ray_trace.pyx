
cdef extern from "ray_trace/ray_trace.h":
    int ray_cast(float x0,float y0,float z0, float x1,float y1,float z1)
    int ray_cast_lax(float x0,float y0,float z0, float x1,float y1,float z1)
    int* _ray_cast3(float x0,float y0,float z0, float x1,float y1,float z1, float* distance)
    int _ray_cast6(float x0,float y0,float z0, float _dfx,float _dfy,float _dfz, float max_l, float *distance, int* collision, int* pre_collision, int* tile, int* side)

def ray_cast3(float x0,float y0,float z0, float x1,float y1,float z1):
    cdef float distance
    cdef int* s
    s = _ray_cast3(x0,y0,z0,x1,y1,z1, &distance)
    x = [None, None, None, None]
    x[0] = distance
    x[1] = s[0]
    x[2] = s[1]
    x[3] = s[2]
    print "distance= %f" % (distance)
    return x

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
        print "dist= %i,%i,%i, %i,%i,%i" % (collision[0],collision[1],collision[2], pre_collision[0],pre_collision[1],pre_collision[2])
        print "distance= %f tile=%i, side=%i,%i,%i" % (distance, tile, side[0],side[1],side[2])
        return [collision[0],collision[1],collision[2],pre_collision[0],pre_collision[1],pre_collision[2], side[0],side[1],side[2]]
