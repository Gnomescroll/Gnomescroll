
cdef extern from "./t_map_draw.h":
    int _init_t_map_draw()

cpdef init():
    _init_t_map_draw()
