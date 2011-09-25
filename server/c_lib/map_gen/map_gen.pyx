cdef extern from "map_gen/map.hpp":
    void start_voronoi()

def start():
    start_voronoi()
