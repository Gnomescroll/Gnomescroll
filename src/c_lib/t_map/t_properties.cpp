#include "t_properties.hpp"

#include <t_map/t_map.hpp>

namespace t_map
{

class CubeProperties* cube_properties = NULL;

void init_t_properties()
{
    GS_ASSERT(cube_properties == NULL);
    cube_properties = new class CubeProperties[MAX_CUBES];
}

void end_t_properties()
{
    if (cube_properties != NULL) delete[] cube_properties;
}

class CubeProperties* get_cube_properties(CubeID id)
{
    ASSERT_VALID_CUBE_ID(id);
    IF_INVALID_CUBE_ID(id) return NULL;
    if (!cube_properties[id].in_use) return NULL;
    return &cube_properties[id];
}

const char* get_cube_name(CubeID id)
{
    class CubeProperties* p = get_cube_properties(id);
    GS_ASSERT(p != NULL);
    if (p == NULL) return NULL;
    return p->name;
}

CubeID get_compatible_cube_id(const char* name)
{
    return get_cube_id(name);

    // TODO -- we need a config loader thing for managing block renaming/deletions
}

CubeID get_cube_id(const char* name)
{
    // TODO -- use hashes
    for (int i=0; i<MAX_CUBES; i++)
    {
        class CubeProperties* p = get_cube_properties((CubeID)i);
        if (p == NULL) continue;
        if (p->name != NULL && strcmp(name, p->name) == 0)
            return (CubeID)i;
    }
    GS_ASSERT(false);
    printf("No cube id found for name %s\n", name);
    return NULL_CUBE;
}

CubeMaterial get_cube_material(CubeID cube_id)
{
    class CubeProperties* p = get_cube_properties(cube_id);
    GS_ASSERT(p != NULL);
    if (p == NULL) return CUBE_MATERIAL_NONE;
    return p->material;
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

inline bool isValidCube(CubeID cube_id)
{
    IF_INVALID_CUBE_ID(cube_id) return false;   // range check
    if (!isInUse(cube_id)) return false;
    return (cube_id != ERROR_CUBE && cube_id != EMPTY_CUBE && cube_id != NULL_CUBE);
}

bool isInUse(CubeID id)
{
    ASSERT_VALID_CUBE_ID(id);
    IF_INVALID_CUBE_ID(id) return false;
    return cube_properties[id].in_use;
}

bool isSolid(CubeID id) 
{
    ASSERT_VALID_CUBE_ID(id);
    IF_INVALID_CUBE_ID(id) return false;
    return t_map::cube_properties[id].solid;
}

bool isOccludes(CubeID id) 
{
    // don't check id because this shouldnt be used publicly
    return t_map::cube_properties[id].occludes;
}

bool isMagic(CubeID id)
{
    // don't check id because this shouldnt be used publicly
    return t_map::cube_properties[id].magic;
}

bool isTransparent(CubeID id)
{
    // don't check id because this shouldnt be used publicly
    return t_map::cube_properties[id].transparent;
}

bool isItemContainer(CubeID id)
{
    ASSERT_VALID_CUBE_ID(id);
    IF_INVALID_CUBE_ID(id) return false;
    return t_map::cube_properties[id].item_container;
}

unsigned char maxDamage(CubeID id) 
{
    ASSERT_VALID_CUBE_ID(id);
    IF_INVALID_CUBE_ID(id) return 32;
    return t_map::cube_properties[id].max_damage;
}

bool isSolid(int x, int y, int z)
{
    return t_map::cube_properties[t_map::get(x,y,z)].solid;
}

bool isOccludes(int x, int y, int z)
{
    return t_map::cube_properties[t_map::get(x,y,z)].occludes;
}

bool isMagic(int x, int y, int z)
{
    return t_map::cube_properties[t_map::get(x,y,z)].magic;
}

bool isItemContainer(int x, int y, int z)
{
    return t_map::cube_properties[t_map::get(x,y,z)].item_container;
}

unsigned char maxDamage(int x, int y, int z)
{
    return t_map::cube_properties[t_map::get(x,y,z)].max_damage;
}

}   // t_map
