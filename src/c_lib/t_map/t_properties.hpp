/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <item/common/enum.hpp>
#include <common/macros.hpp>
#include <t_map/common/constants.hpp>
#include <t_map/common/types.hpp>
#include <common/dat/name_map.hpp>
#include <common/dat/properties.hpp>

// checks against value range
ALWAYS_INLINE bool isValid(CubeType cube_type);

namespace t_map
{

// checks against ERROR_CUBE/NULL_CUBE/EMPTY_CUBE and value range and if used
ALWAYS_INLINE bool isValidCube(CubeType cube_type);
ALWAYS_INLINE bool isInUse(CubeType type);


struct FastCubeProperties
{
    bool active;
    bool solid;
    bool occludes;
    bool transparent;
    bool item_drop;
    bool item_container;
    bool special; //requires action on creation/deletion
    bool radioactive;
    bool light_source; //is light source
    bool explosive;
};


struct FastCubeAttributes
{
    unsigned char light_value; //how much light cube emits
    unsigned char rad_value; //how much light cube emits
};


class CubeProperty: public Property<CubeType>
{
    public:
        CubeGroup group;

        bool solid;
        bool occludes;  // occludes on all 6 faces only
        bool active;  // Brandon's special flag
        bool transparent;     //
        bool item_drop;       // does block drop items
        bool special;

        CubeMaterial material;
        ItemContainerType container_type;  // inventory and crafting bench blocks
        bool explosive; // causes an explosion when destroyed

        bool light_source;
        int light_value;

        bool radioactive;
        int rad_value;

        unsigned char max_damage;

    CubeProperty() :
        Property<CubeType>(NULL_CUBE)
    {
        this->group = ErrorCube;

        this->solid = true;
        this->occludes = true;
        this->active = true;
        this->transparent = false;
        this->item_drop = false;

        this->special = false;

        this->material = CUBE_MATERIAL_NONE;
        this->container_type = NULL_CONTAINER_TYPE;
        this->explosive = false;

        this->light_source = false;
        this->light_value  = 0;

        this->radioactive = false;
        this->rad_value = 0;

        this->max_damage = 32;
    }
};


class CubeProperties: public Properties<CubeProperty, CubeType>
{
    public:

    CubeProperties() :
        Properties<CubeProperty, CubeType>(MAX_CUBES)
    {}
};

extern class CubeProperties* cube_properties;
extern struct FastCubeProperties* fast_cube_properties;
extern struct FastCubeAttributes* fast_cube_attributes;
extern class DatNameMap* cube_name_map;

void init_t_properties();
void end_t_properties();

class CubeProperty* get_cube_properties(CubeType type);

CubeMaterial get_cube_material(CubeType type);

const char* get_cube_name(CubeType type);
const char* get_cube_pretty_name(CubeType type);
const char* get_cube_description(CubeType type);
CubeType get_cube_type(const char* name);
CubeGroup get_cube_group(CubeType type);

const char* get_cube_name_for_container(ItemContainerType container_type);
CubeType get_cube_type_for_container(ItemContainerType container_type);

// applies forward-compatible name versioning to give an id
// use only for the serializer
const char* get_compatible_cube_name(const char* name);

ItemContainerType get_container_type_for_cube(CubeType cube_type);

// Properties by cube id

ALWAYS_INLINE bool isSolid(CubeType type);
ALWAYS_INLINE bool isOccludes(CubeType type);
ALWAYS_INLINE bool isActive(CubeType type);
ALWAYS_INLINE bool isTransparent(CubeType type);
ALWAYS_INLINE bool isItemContainer(CubeType type);
ALWAYS_INLINE bool isSpecial(CubeType type);
ALWAYS_INLINE bool isRadioactive(CubeType type);

ALWAYS_INLINE bool isExplosive(CubeType type);
ALWAYS_INLINE unsigned char maxDamage(CubeType type);
ALWAYS_INLINE bool hasItemDrop(CubeType type);

//Properties by coordinates

ALWAYS_INLINE bool isSolid(int x, int y, int z);
ALWAYS_INLINE bool isOccludes(int x, int y, int z);
ALWAYS_INLINE bool isActive(int x, int y, int z);
ALWAYS_INLINE bool isItemContainer(int x, int y, int z);
ALWAYS_INLINE bool isExplosive(int x, int y, int z);
ALWAYS_INLINE unsigned char maxDamage(int x, int y, int z);

// Properties by struct Vec3i

ALWAYS_INLINE bool isSolid(const struct Vec3i& pos);
ALWAYS_INLINE bool isOccludes(const struct Vec3i& pos);
ALWAYS_INLINE bool isActive(const struct Vec3i& pos);
ALWAYS_INLINE bool isItemContainer(const struct Vec3i& pos);
ALWAYS_INLINE bool isExplosive(const struct Vec3i& pos);
ALWAYS_INLINE unsigned char maxDamage(const struct Vec3i& pos);

}   // t_map
