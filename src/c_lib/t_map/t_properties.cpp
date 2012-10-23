#include "t_properties.hpp"

#include "t_map.hpp"

namespace t_map
{

struct cubeProperties* cube_list = NULL;

char* cube_names = NULL;

void init_t_properties()
{
    GS_ASSERT(cube_list == NULL);
    cube_list = (cubeProperties*) calloc(MAX_CUBES, sizeof(struct cubeProperties));
    for (int i=0; i<MAX_CUBES; cube_list[i++].in_use = false);

    GS_ASSERT(cube_names == NULL);
    cube_names = (char*)calloc(MAX_CUBES*(CUBE_NAME_MAX_LENGTH+1), sizeof(char));
}

void end_t_properties()
{
    if (cube_list != NULL) free(cube_list);
    if (cube_names != NULL) free(cube_names);
}

struct cubeProperties* get_cube(int id)
{
    ASSERT_VALID_CUBE_ID(id);
    IF_INVALID_CUBE_ID(id) return NULL;
    return &cube_list[id];
}

void set_cube_name(int id, const char* name)
{
    ASSERT_VALID_CUBE_ID(id);
    IF_INVALID_CUBE_ID(id) return;

    ASSERT_VALID_CUBE_ID(id);
    IF_INVALID_CUBE_ID(id) return;

    bool valid_name = is_valid_cube_name(name);
    GS_ASSERT(valid_name);
    if (!is_valid_cube_name(name)) return;

    int index = id * (CUBE_NAME_MAX_LENGTH+1);
    strncpy(&cube_names[index], name, CUBE_NAME_MAX_LENGTH+1);
    cube_names[index + CUBE_NAME_MAX_LENGTH] = '\0';
}

const char* get_cube_name(int id)
{
    ASSERT_VALID_CUBE_ID(id);
    IF_INVALID_CUBE_ID(id) return NULL;
    if (!isInUse(id)) return NULL;
    return (cube_names + ((CUBE_NAME_MAX_LENGTH + 1) * id));
}

int get_compatible_cube_id(const char* name)
{
    return get_cube_id(name);

    // TODO -- we need a config loader thing for managing block renaming/deletions
}

int get_cube_id(const char* name)
{
    // TODO -- use hashes
    for (int i=0; i<MAX_CUBES; i++)
    {
        const char* other = get_cube_name(i);
        if (other != NULL && strcmp(name, other) == 0)
            return i;
    }
    GS_ASSERT(false);
    printf("No cube id found for name %s\n", name);
    return -1;
}

int dat_get_cube_id(const char* name)
{
    int id = get_cube_id(name);
    GS_ASSERT(id >= 0);
    if (id < 0)
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
    ASSERT_VALID_CUBE_ID(cube_id);
    IF_INVALID_CUBE_ID(cube_id) return CUBE_MATERIAL_NONE;
    return cube_list[cube_id].material;
}

bool is_valid_cube_name(const char* name)
{
    if (name == NULL) return false;
    size_t len = strlen(name);
    if (len <= 0 || len > CUBE_NAME_MAX_LENGTH) return false;
    for (size_t i=0; i<len; i++)
        if (!is_valid_name_char(name[i]))
            return false;
    return true;
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

void LUA_set_block_name(int id, const char* name)
{
    t_map::set_cube_name(id, name);
}


/*
    Properties by cube id
*/

namespace t_map
{

bool isErrorBlock(int id)
{
    return (id == ERROR_CUBE);
}

bool isInUse(int id)
{
    ASSERT_VALID_CUBE_ID(id);
    IF_INVALID_CUBE_ID(id) return false;
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
