#pragma once

#include <item/common/enum.hpp>
#include <common/macros.hpp>
#include <t_map/common/constants.hpp>
#include <t_map/common/types.hpp>
#include <common/dat/name_map.hpp>

namespace t_map
{

struct FastCubeProperties
{
    bool active;
    bool solid;
    bool occludes;
    bool transparent;
    bool item_drop;
    bool item_container;
    bool explosive;
    bool light_source; //is light source
};

struct FastCubeAttributes
{
    unsigned char light_value; //how much light cube emits
};

class CubeProperties
{
    public:
        CubeType type;
        CubeGroup group;

        bool solid;
        bool occludes;  // occludes on all 6 faces only
        bool active;  // Brandon's special flag
        bool transparent;     //
        bool item_drop;       // does block drop items

        CubeMaterial material;
        ItemContainerType container_type;  // inventory and crafting bench blocks
        bool explosive; // causes an explosion when destroyed

        bool light_source;
        int light_value;

        unsigned char max_damage;

        char name[DAT_NAME_MAX_LENGTH+1];

        //for dat debug assistance
        bool loaded;

    CubeProperties()
    {
        this->type = NULL_CUBE;
        this->group = ErrorCube;

        this->solid = true;
        this->occludes = true;
        this->active = true;
        this->transparent = false;
        this->item_drop = false;

        this->material = CUBE_MATERIAL_NONE;
        this->container_type = NULL_CONTAINER_TYPE;
        this->explosive = false;

        this->light_source = false;
        this->light_value  = 0;

        this->max_damage = 32;

        memset(this->name, 0, sizeof(this->name));

        this->loaded = false;
    }
};

extern struct FastCubeProperties* fast_cube_properties;
extern struct FastCubeAttributes* fast_cube_attributes;

extern class CubeProperties* cube_properties;
extern class DatNameMap* cube_name_map;

void init_t_properties();
void end_t_properties();

class CubeProperties* get_cube_properties(CubeType id);

CubeMaterial get_cube_material(CubeType cube_type);

const char* get_cube_name(CubeType id);
CubeType get_cube_type(const char* name);

CubeType get_cube_type(CubeType id);

const char* get_cube_name_for_container(ItemContainerType container_type);
CubeType get_cube_type_for_container(ItemContainerType container_type);

bool is_valid_cube_name(const char* name);

// applies forward-compatible name versioning to give an id
// use only for the serializer
const char* get_compatible_cube_name(const char* name);

ItemContainerType get_container_type_for_cube(CubeType cube_type);

// checks against ERROR_CUBE/NULL_CUBE/EMPTY_CUBE and value range and used
inline bool isValidCube(CubeType cube_type) __attribute((always_inline));
inline bool isInUse(CubeType id) __attribute((always_inline));

// Properties by cube id

inline bool isSolid(CubeType id) __attribute((always_inline));
inline bool isOccludes(CubeType id) __attribute((always_inline));
inline bool isActive(CubeType id) __attribute((always_inline));
inline bool isTransparent(CubeType id) __attribute((always_inline));
inline bool isItemContainer(CubeType id) __attribute((always_inline));
inline bool isExplosive(CubeType id) __attribute((always_inline));
inline unsigned char maxDamage(CubeType id) __attribute((always_inline));

//Properties by coordinates

inline bool isSolid(int x, int y, int z) __attribute((always_inline));
inline bool isOccludes(int x, int y, int z) __attribute((always_inline));
inline bool isActive(int x, int y, int z) __attribute((always_inline));
inline bool isItemContainer(int x, int y, int z) __attribute((always_inline));
inline bool isExplosive(int x, int y, int z) __attribute((always_inline));
inline unsigned char maxDamage(int x, int y, int z) __attribute((always_inline));

}   // t_map
