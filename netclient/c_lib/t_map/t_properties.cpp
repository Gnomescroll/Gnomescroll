#include "t_properties.hpp"

#include "t_map.hpp"

//include <stdio.h>

namespace t_map
{

struct cubeProperties* cube_list = NULL;

//struct cubeProperties* _get_cube(int id)  { return &cube_list[id]; }

void init_t_properties()
{
    printf("init_t_properties() \n");
    
    if(cube_list != NULL) printf("ERROR: init_t_properties called twice\n");

    cube_list = (cubeProperties*) malloc(sizeof(struct cubeProperties) * MAX_CUBES);
    return;
}

void end_t_properties()
{
    free(cube_list);
    return;
}

struct cubeProperties* get_cube(int id)
{
    if(id < 0) printf("get_cube: error id less than zero \n");
    if(id >= MAX_CUBES ) printf("get_cube: error id exceeds MAX_CUBES \n");
    return &cube_list[id];
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
