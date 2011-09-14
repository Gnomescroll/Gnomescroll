#include "./t_properties.h"

struct cubeProperties cube_list[max_cubes];

struct cubeProperties* _get_cube_list() {
    return cube_list;
}

struct cubeProperties* _get_cube(int id) {
    return &cube_list[id];
}

/*
int inline _isActive(int id) {
    return cube_list[id].active;
    }

int inline _maxDamage(int id) {
    return cube_list[id].max_damage;
    }
*/
