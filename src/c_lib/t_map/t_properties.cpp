#include "t_properties.hpp"

#include <t_map/t_map.hpp>

ALWAYS_INLINE bool isValid(CubeType cube_type)
{
    return (cube_type >= 0 && cube_type < MAX_CUBES && cube_type != NULL_CUBE);
}

namespace t_map
{

class CubeProperties* cube_properties;
struct FastCubeProperties* fast_cube_properties = NULL;
struct FastCubeAttributes* fast_cube_attributes = NULL;
class DatNameMap* cube_name_map = NULL;

void init_t_properties()
{
    GS_ASSERT(cube_properties == NULL);
    cube_properties = new class CubeProperties;

    GS_ASSERT(fast_cube_properties == NULL);
    fast_cube_properties = (struct FastCubeProperties*)calloc(MAX_CUBES, sizeof(struct FastCubeProperties));

    GS_ASSERT(fast_cube_attributes == NULL);
    fast_cube_attributes = (struct FastCubeAttributes*)calloc(MAX_CUBES, sizeof(struct FastCubeAttributes));

    GS_ASSERT(cube_name_map == NULL);
    cube_name_map = new class DatNameMap(256, DAT_NAME_MAX_LENGTH);
}

void end_t_properties()
{
    delete cube_properties;
    free(fast_cube_properties);
    free(fast_cube_attributes);
    delete cube_name_map;
}

class CubeProperty* get_cube_properties(CubeType type)
{
    return cube_properties->get(type);
}

class CubeProperty* get_cube_properties(const char* name)
{
    return cube_properties->get(name);
}

const char* get_cube_name(CubeType type)
{
    class CubeProperty* p = get_cube_properties(type);
    IF_ASSERT(p == NULL) return NULL;
    return p->name;
}

const char* get_cube_pretty_name(CubeType type)
{
    class CubeProperty* p = get_cube_properties(type);
    IF_ASSERT(p == NULL) return NULL;
    return p->pretty_name;
}

const char* get_cube_description(CubeType type)
{
    class CubeProperty* p = get_cube_properties(type);
    IF_ASSERT(p == NULL) return NULL;
    return p->description;
}

const char* get_compatible_cube_name(const char* name)
{
    const char* mapname = cube_name_map->get_mapped_name(name);
    if (mapname != NULL) return mapname;

    // look for the name defined in the cube list
    // (we don't reuse get_cube_type() becuase that returns ERROR_CUBE when not found,
    // which is technically a valid cube for other purposes
    for (int i=0; i<MAX_CUBES; i++)
    {
        class CubeProperty* p = get_cube_properties((CubeType)i);
        if (p != NULL && strcmp(name, p->name) == 0)
            return name;
    }
    return NULL;
}

CubeType get_cube_type(const char* name)
{
    CubeProperty* p = get_cube_properties(name);
    IF_ASSERT(p == NULL) return NULL_CUBE;
    return p->type;
}

CubeGroup get_cube_group(CubeType type)
{
    class CubeProperty* p = get_cube_properties(type);
    IF_ASSERT(p == NULL) return ErrorCube;
    return p->group;
}

const char* get_cube_name_for_container(ItemContainerType container_type)
{   // not indexed/fast, use only for init/cached values
    CubeType cube_type = get_cube_type_for_container(container_type);
    if (cube_type == NULL_CUBE) return NULL;
    return get_cube_properties(cube_type)->name;
}

CubeType get_cube_type_for_container(ItemContainerType container_type)
{
    for (size_t i=0; i<cube_properties->max; i++)
    {
        class CubeProperty* p = get_cube_properties((CubeType)i);
        if (p != NULL && p->container_type == container_type)
            return (CubeType)i;
    }
    return NULL_CUBE;
}

CubeMaterial get_cube_material(CubeType cube_type)
{
    class CubeProperty* p = get_cube_properties(cube_type);
    IF_ASSERT(p == NULL) return CUBE_MATERIAL_NONE;
    return p->material;
}

ItemContainerType get_container_type_for_cube(CubeType cube_type)
{
    class CubeProperty* p = get_cube_properties(cube_type);
    IF_ASSERT(p == NULL) return NULL_CONTAINER_TYPE;
    return p->container_type;
}

ALWAYS_INLINE bool isValidCube(CubeType cube_type)
{
    if (!isValid(cube_type)) return false;
    if (!isInUse(cube_type)) return false;
    return (cube_type != ERROR_CUBE &&
            cube_type != EMPTY_CUBE &&
            cube_type != NULL_CUBE);
}

ALWAYS_INLINE bool isInUse(CubeType type)
{
    return (get_cube_properties(type) != NULL);
}

ALWAYS_INLINE bool isSolid(CubeType type)
{   // make sure you don't call this with a cube out of range
    return fast_cube_properties[type].solid;
}

ALWAYS_INLINE bool isOccludes(CubeType type)
{
    // don't check type because this shouldnt be used publicly
    return fast_cube_properties[type].occludes;
}

ALWAYS_INLINE bool isActive(CubeType type)
{
    // don't check type because this shouldnt be used publicly
    return fast_cube_properties[type].active;
}

ALWAYS_INLINE bool isTransparent(CubeType type)
{
    // don't check type because this shouldnt be used publicly
    return fast_cube_properties[type].transparent;
}

ALWAYS_INLINE bool isItemContainer(CubeType type)
{
    return fast_cube_properties[type].item_container;
}

ALWAYS_INLINE bool isSpecial(CubeType id)
{
    return fast_cube_properties[id].special;
}

ALWAYS_INLINE bool isRadioactive(CubeType id)
{
    return fast_cube_properties[id].radioactive;
}

ALWAYS_INLINE bool isExplosive(CubeType type)
{
    return fast_cube_properties[type].explosive;
}

ALWAYS_INLINE unsigned char maxDamage(CubeType type)
{
    CubeProperty* p = get_cube_properties(type);
    IF_ASSERT(p == NULL) return 32;
    return p->max_damage;
}

ALWAYS_INLINE bool hasItemDrop(CubeType type)
{
    CubeProperty* p = get_cube_properties(type);
    IF_ASSERT(p == NULL) return false;
    return p->item_drop;
}

// Properties by coordinate

ALWAYS_INLINE bool isSolid(int x, int y, int z)
{
    return fast_cube_properties[get(x,y,z)].solid;
}

ALWAYS_INLINE bool isOccludes(int x, int y, int z)
{
    return fast_cube_properties[get(x,y,z)].occludes;
}

ALWAYS_INLINE bool isActive(int x, int y, int z)
{
    return fast_cube_properties[get(x,y,z)].active;
}

ALWAYS_INLINE bool isItemContainer(int x, int y, int z)
{
    return fast_cube_properties[get(x,y,z)].item_container;
}

ALWAYS_INLINE bool isSpecial(int x, int y, int z)
{
    return fast_cube_properties[get(x,y,z)].special;
}

ALWAYS_INLINE bool isRadioactive(int x, int y, int z)
{
    return fast_cube_properties[get(x,y,z)].radioactive;
}


ALWAYS_INLINE bool isExplosive(int x, int y, int z)
{
    return fast_cube_properties[get(x,y,z)].explosive;
}

ALWAYS_INLINE unsigned char maxDamage(int x, int y, int z)
{
    return maxDamage(get(x,y,z));
}

// Properties my struct Vec3i

ALWAYS_INLINE bool isSolid(const struct Vec3i& pos)
{
    return fast_cube_properties[get(pos)].solid;
}

ALWAYS_INLINE bool isOccludes(const struct Vec3i& pos)
{
    return fast_cube_properties[get(pos)].occludes;
}

ALWAYS_INLINE bool isActive(const struct Vec3i& pos)
{
    return fast_cube_properties[get(pos)].active;
}

ALWAYS_INLINE bool isItemContainer(const struct Vec3i& pos)
{
    return fast_cube_properties[get(pos)].item_container;
}


ALWAYS_INLINE bool isExplosive(const struct Vec3i& pos)
{
    return fast_cube_properties[get(pos)].explosive;
}

ALWAYS_INLINE unsigned char maxDamage(const struct Vec3i& pos)
{
    return maxDamage(get(pos));
}

}   // t_map
