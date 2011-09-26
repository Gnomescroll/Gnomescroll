'''
cdef extern from "map_gen/map2.hpp":
    void start_voronoi()

def start():
    start_voronoi()

'''
