#pragma once


// cube properties

#define max_cubes 1024

struct cubeProperties {
    int active;
    int occludes;
    int solid;
    int gravity;
    int transparent;

    int max_damage;
    int neutron_tolerance;
    int nuclear;
};

struct cubeProperties* _get_cube_list();
struct cubeProperties* _get_cube(int id);

extern struct cubeProperties cube_list[max_cubes];

int _isActive(int id);

static inline int isActive(int id) {
    return cube_list[id].active;
    }

static inline int isNuclear(int id) {
    return cube_list[id].nuclear;
    }

static inline int isSolid(int id) {
    return cube_list[id].solid;
}

int inline _maxDamage(int id) {
    return cube_list[id].max_damage;
    }
