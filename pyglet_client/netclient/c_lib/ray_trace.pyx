
cdef extern from "ray_trace/ray_trace.h":
    int ray_cast(float x0,float y0,float z0, float x1,float y1,float z1)
    int ray_cast_lax(float x0,float y0,float z0, float x1,float y1,float z1)
    int* ray_cast3(float x0,float y0,float z0, float x1,float y1,float z1, float* distance)
