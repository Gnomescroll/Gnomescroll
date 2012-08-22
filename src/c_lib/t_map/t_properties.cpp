#include "t_properties.hpp"

#include "t_map.hpp"



namespace t_map
{

struct cubeProperties* cube_list = NULL;

char cube_names[MAX_CUBES*64];
int cube_name_index[MAX_CUBES];

void init_t_properties()
{
    //printf("init_t_properties() \n");
    
    if(cube_list != NULL) printf("ERROR: init_t_properties called twice\n");

    cube_list = (cubeProperties*) calloc(MAX_CUBES, sizeof(struct cubeProperties));
    for (int i=0; i<MAX_CUBES; cube_list[i++].in_use = false);

    memset(cube_names, 0, 64* MAX_CUBES);
    memset(cube_name_index, 0, sizeof(int) * MAX_CUBES);
    //for(int i=0; i<MAX_CUBES*64; i++) cube_names = NULL;
    //for(int i=0; i<MAX_CUBES; i++) cube_name_index = 0s;
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


void set_cube_name(int id, const char* name, int length)
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
    cube_names[index] = '\0';
    index++;
}

void set_cube_name(int id, const char* name)
{
    int length = (int)strlen(name);
    set_cube_name(id, name, length);
}

const char* get_cube_name(int id)
{
    GS_ASSERT(id >= 0 && id < MAX_CUBES);
    if (id < 0 || id >= MAX_CUBES)
    {
        printf("%s:%d, cube id %d invalid\n", __FUNCTION__, __LINE__, id);
        return NULL;
    }

    return (cube_names + cube_name_index[id]);
}

int get_cube_id(const char* name)
{
    for (int i=0; i<MAX_CUBES; i++)
    {
        if (strcmp(name, get_cube_name(i)) == 0)
            return i;
    }
    GS_ASSERT(false);
    printf("No cube id found for name %s\n", name);
    return -1;
}

int dat_get_cube_id(const char* name)
{
    int id = get_cube_id((char*) name);
    GS_ASSERT(id >= 0);
    if(id < 0)
    {
        printf("Dat Loading Failure:cube_id, dat failure, cube %s does not exist! \n", name);
        return -1;
    }
    return id; 
}

CubeMaterial get_cube_material(int cube_id)
{
    GS_ASSERT(cube_list != NULL);
    if (cube_list == NULL) return CUBE_MATERIAL_NONE;
    ASSERT_VALID_BLOCK(cube_id);
    IF_INVALID_BLOCK(cube_id) return CUBE_MATERIAL_NONE;
    return cube_list[cube_id].material;
}

}   // t_map

/*
    LUA interface
*/

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

void LUA_set_block_name(int id, const char* name, int length)
{
    t_map::set_cube_name(id, name, length);
}


/*
    Properties by cube id
*/

namespace t_map
{

bool isErrorBlock(int id)
{
    return (id == 255);
}

bool isValidID(int id)
{
    return (id >=0 && id < MAX_CUBES);
}

bool isInUse(int id)
{
    return cube_list[id].in_use;
}

}   // t_map

// TODO -- put in t_map namespace

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

bool isItemContainer(int id)
{
    return t_map::cube_list[id].item_container;
}

/*
    Properties by cordinates
*/
bool isActive(int x, int y, int z)
{
    return t_map::cube_list[t_map::get(x,y,z)].active;
    //return isActive(t_map::get(x,y,z));
}

bool isSolid(int x, int y, int z)
{
    return t_map::cube_list[t_map::get(x,y,z)].solid;
    //return isSolid(t_map::get(x,y,z));
}

bool isOccludes(int x, int y, int z)
{
    return t_map::cube_list[t_map::get(x,y,z)].occludes;
    //return isOccludes(t_map::get(x,y,z));
}

bool isItemContainer(int x, int y, int z)
{
    return t_map::cube_list[t_map::get(x,y,z)].item_container;
}

/*
    Map Damage
*/
    
int maxDamage(int id) 
{
    return t_map::cube_list[id].max_damage;
}
