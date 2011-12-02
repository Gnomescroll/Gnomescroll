#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <compat.h>

#include "t_map.hpp"

// cube properties

const int max_cubes = 1024;
const int N_PIXEL_SAMPLES = 10;

struct cubeProperties {
    int active;
    int occludes;
    int solid;
    int gravity;
    int transparent;

    int max_damage;
    int neutron_tolerance;
    int nuclear;
    int infinite_texture;

};

struct cubeProperties* _get_cube_list();
struct cubeProperties* _get_cube(int id);

extern struct cubeProperties cube_list[max_cubes];

void set_infinite_texture(int id, int texture);
int get_infinite_texture(int index);

void init_t_properties();

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

static inline int isOccludes(int id) {
    return cube_list[id].occludes;
    }

//returns 0, 1, 2 or 3 based upon kind of transparency
static inline int isTransparent(int id) {
    return cube_list[id].transparent;
    }

static inline int getInfiniteTexture(int id) {
    return cube_list[id].infinite_texture;
    }

static inline int collidesBlock(int x, int y, int z) {
    return isSolid(_get(x,y,z));
}
