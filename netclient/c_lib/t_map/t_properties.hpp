#pragma once

namespace t_map
{

struct cubeProperties 
{
    bool active;
    bool solid;
    bool occludes;
    bool transparent;
    bool reserved5;
    bool reserved6;
    bool reserved7;
    bool reserved8;

    unsigned char max_damage;

    //bool solid;
    //bool gravity;
};

const int max_cubes = 256;
extern struct cubeProperties cube_list[max_cubes];


void init_t_properties();

struct cubeProperties* get_cube(int id)
{
    if(id < 0) printf("get_cube: error id less than zero \n");
    if(id >= max_cubes ) printf("get_cube: error id exceeds max_cubes \n");
    return &cube_list[id];
}

using namespace t_map;

inline struct cubeProperties* _get_cube(int id) __attribute((always_inline));

struct cubeProperties* _get_cube(int id)
{
    return &cube_list[id];
}

}

/*
Cube Properties
*/

inline bool isActive(int id) __attribute((always_inline));
inline bool isSolid(int id) __attribute((always_inline));
inline bool isOccludes(int id) __attribute((always_inline));
inline bool isTransparent(int id) __attribute((always_inline));

bool isActive(int id) 
{
    return t_map::cube_list[id].active;
}

bool isSolid(int id) 
{
    return t_map::cube_list[id].solid;
}

bool isOccludes(int id) 
{
    return t_map::cube_list[id].occludes;
}

bool isTransparent(int id)
{
    return t_map::cube_list[id].transparent;
}

inline bool isActive(int x, int y, int z) __attribute((always_inline));
inline bool isSolid(int x, int y, int z) __attribute((always_inline));
inline bool isOccludes(int x, int y, int z) __attribute((always_inline));

bool isActive(int x, int y, int z)
{
    return isActive(t_map::get(x,y,z));
}

bool isSolid(int x, int y, int z)
{
    return isSolid(t_map::get(x,y,z));
}

bool isOccludes(int x, int y, int z)
{
    return isOccludes(t_map::get(x,y,z));
}