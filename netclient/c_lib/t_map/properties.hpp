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
extern short cube_side_texture_array[max_cubes*6];




void init_t_properties();

/*
    cube side texture array
*/
void set_cube_side_texture(int id, int side, int tex_id);
int get_cube_side_texture(int id, int side);

inline struct cubeProperties* _get_cube(int id) __attribute((always_inline);

inline bool isActive(int id) __attribute((always_inline);
inline bool isSolid(int id) __attribute((always_inline);
inline bool isOccludes(int id) __attribute((always_inline);

/*
Cube Properties
*/

bool isActive(int id) 
{
    return cube_list[id].active;
}

bool isSolid(int id) 
{
    return cube_list[id].solid;
}

bool isOccludes(int id) 
{
    return cube_list[id].occludes;
}

}