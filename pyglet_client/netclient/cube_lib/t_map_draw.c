#include "t_map_draw.h"
#include "t_map.h"

struct vm_map* map;

int _init_t_map_draw() {
    map = _get_map();
    }

#define max_cubes 1024
struct cubeProperties cube_list[max_cubes];

struct cubeProperties* _get_cube(int id) {
    return &cube_list[id];
}
