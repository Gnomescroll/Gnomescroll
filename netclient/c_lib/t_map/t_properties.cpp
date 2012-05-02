#include "t_properties.hpp"

#include "t_map.hpp"



namespace t_map
{
    struct cubeProperties* cube_list = NULL;
}


namespace t_map
{

char cube_names[MAX_CUBES*64];
int cube_name_index[MAX_CUBES];

void init_t_properties()
{
    //printf("init_t_properties() \n");
    
    if(cube_list != NULL) printf("ERROR: init_t_properties called twice\n");

    cube_list = (cubeProperties*) malloc(sizeof(struct cubeProperties) * MAX_CUBES);
    memset(cube_list, 0, sizeof(struct cubeProperties) * MAX_CUBES);


    memset(cube_names, 0, 64* MAX_CUBES);
    memset(cube_name_index, 0, sizeof(int) * MAX_CUBES);
    //for(int i=0; i<MAX_CUBES*64; i++) cube_names = NULL;
    //for(int i=0; i<MAX_CUBES; i++) cube_name_index = 0s;

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


void set_cube_name(int id, char* name, int length)
{
    static int index = 0;

    if(length >= 64)
    {
        printf("Error: set_cube_name(), name length greater than 63 characters\n");
        return;
    }

    if(index +length + 1 > MAX_CUBES*64)
    {
        printf("Error: set_cube_name(), WTF potential overflow\n");
        return;
    }

    if(id < 0 || id >= MAX_CUBES)
    {
        printf("Error: set_cube_name(), cube id error\n");
        return;
    }

    cube_name_index[id] = index;

    memcpy(cube_names+index, name, length);
    index += length;
    cube_names[index] = 0x00;
    index++;
}

void set_cube_name(int id, char* name)
{
    int length = strlen(name);
    set_cube_name(id, name, length);
}

char* get_cube_name(int id)
{
    if(id < 0 || id >= MAX_CUBES)
    {
        printf("Error: cube_name_index(), cube id error\n");
        return NULL;
    }

    return (cube_names + cube_name_index[id]);
}

int get_cube_id(char* name)
{
    for (int i=0; i<MAX_CUBES; i++)
    {
        if (strcmp(name, get_cube_name(i)) == 0)
            return i;
    }
    return -1;
}

int dat_get_cube_id(const char* name)
{
    int id = get_cube_id((char*) name);
    if(id == -1)
    {
        printf("Dat Loading Failure:cube_id, dat failure, cube %s does not exist! \n", name);
        abort();
    }   
    return id; 
}

}

/*
    LUA interface
*/

extern "C"
{
    void LUA_set_block_properties(int id, int active, int solid, int occludes, int transparent)
    {
        t_map::cube_list[id].active = active;
        t_map::cube_list[id].solid = solid;
        t_map::cube_list[id].occludes = occludes;
        t_map::cube_list[id].transparent = transparent;        
    }

    void LUA_set_block_max_damage(int id, int max_damage)
    {
        t_map::cube_list[id].max_damage = max_damage;
    }

    void LUA_set_block_color_type(int id, int color_type)
    {
        t_map::cube_list[id].color_type = color_type;
    }

    void LUA_set_block_name(int id, char* name, int length)
    {
        t_map::set_cube_name(id, name, length);
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
    
int maxDamage(int id) 
{
    return t_map::cube_list[id].max_damage;
}
