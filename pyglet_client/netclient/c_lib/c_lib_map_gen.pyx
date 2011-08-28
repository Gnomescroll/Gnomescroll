
cdef extern from "./map_gen/density.h":
    void init_map_gen_density()
    void update_density_map(int iterations)
    void map_density_visualize(int density, float min, float max)


def _update_density_map(int iterations):
    update_density_map(iterations)

def _map_density_visualize(int density, float min, float max):
    map_density_visualize(density, min, max)

