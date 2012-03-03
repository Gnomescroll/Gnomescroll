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

const int MAX_CUBES = 256;

extern struct cubeProperties* cube_list; //[MAX_CUBES];


void init_t_properties();

void end_t_properties();

struct cubeProperties* get_cube(int id) GNOMESCROLL_API;

using namespace t_map;

inline struct cubeProperties* _get_cube(int id) __attribute((always_inline)) GNOMESCROLL_API;

//struct cubeProperties* _get_cube(int id)
//{
    //return &cube_list[id];
//}

}

/*
Cube Properties
*/


/*
    Properties by cube id
*/

inline bool isActive(int id) __attribute((always_inline));
//inline bool isSolid(int id) __attribute((always_inline));
bool isSolid(int id) GNOMESCROLL_API;
inline bool isOccludes(int id) __attribute((always_inline));
inline bool isTransparent(int id) __attribute((always_inline));

/*
    Properties by coordinates
*/

inline bool isActive(int x, int y, int z) __attribute((always_inline));
inline bool isSolid(int x, int y, int z) __attribute((always_inline));
inline bool isOccludes(int x, int y, int z) __attribute((always_inline));


/*
    Map Damage
*/

inline int maxDamage(int id) __attribute((always_inline));
