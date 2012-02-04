#pragma once

const int max_cubes = 256;

struct cubeProperties 
{
    bool active;
    bool occludes;
    bool transparent;
    bool reserved4;
    bool reserved5;
    bool reserved6;
    bool reserved7;
    bool reserved8;

    unsigned char max_damage;

    //bool solid;
    //bool gravity;
};

struct cubeProperties* _get_cube_list();
struct cubeProperties* _get_cube(int id);

extern struct cubeProperties cube_list[max_cubes];

void init_t_properties();

/*
    cube side texture array
*/
void _set_cube_side_texture(int id, int side, int tex_id);
int _get_cube_side_texture(int id, int side);



inline bool isActive(int id) __attribute((always_inline);
inline bool isSolid(int id) __attribute((always_inline);
inline bool isOccludes(int id) __attribute((always_inline);

/*
Cube Properties
*/

inline int isActive(int id) 
{
    return cube_list[id].active;
}

inline int isSolid(int id) 
{
    return cube_list[id].solid;
}

inline int isOccludes(int id) 
{
    return cube_list[id].occludes;
}