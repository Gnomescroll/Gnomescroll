#include "t_properties.hpp"

#include "t_map.hpp"

namespace t_map
{

const int MAX_TEXTURES = 256;

struct cubeProperties* cube_list;

//struct cubeProperties* _get_cube(int id)  { return &cube_list[id]; }

void init_t_properties()
{
    printf("init cube property list \n");
    
    cube_list = new struct cubeProperties[MAX_TEXTURES];
    memset(cube_list, 0, MAX_TEXTURES* sizeof(struct cubeProperties) );
}

}

/*
    Properties by cube id
*/

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

/*
    Properties by cordinates
*/
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

/*
    Map Damage
*/

int maxDamage(int id) __attribute((always_inline));

int maxDamage(int id) 
{
    return t_map::cube_list[id].max_damage;
}
