#include "./t_properties.h"

#define max_cubes 1024
struct cubeProperties cube_list[max_cubes];

struct cubeProperties* _get_cube_list() {
    return &cube_list;
}

struct cubeProperties* _get_cube(int id) {
    return &cube_list[id];
}

//buffer
#define max_cubes 1024

Vertex quad_cache[max_cubes*6*4];

struct Vertex* _get_quad_cache() {
    return &quad_cache
}
