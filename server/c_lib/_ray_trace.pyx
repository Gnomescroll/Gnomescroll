
#cdef extern from "ray_trace/ray_trace.h":
#    int ray_cast(float x0,float y0,float z0, float x1,float y1,float z1)
#    int ray_cast_lax(float x0,float y0,float z0, float x1,float y1,float z1)
#    int* _ray_cast3(float x0,float y0,float z0, float x1,float y1,float z1, float* distance)

def ray_cast3(float x0,float y0,float z0, float x1,float y1,float z1):
    print "WARNING: _ray_trace.pyx  :: ray_cast3 -- function deprecated, does nothing"
#    cdef float distance
#    cdef int* s
#    distance = 0.
#    s = _ray_cast3(x0,y0,z0,x1,y1,z1, &distance)
    x = [None, None, None, None]
#    x[0] = distance
#    x[1] = s[0]
#    x[2] = s[1]
#    x[3] = s[2]
#    print "distance= %f" % (distance)
    return x
