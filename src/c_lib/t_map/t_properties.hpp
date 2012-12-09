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
    bool reserved8;
};

class CubeProperties 
{
    public:
        CubeID id;
        CubeType type;
        
        bool solid;
        bool occludes;  // occludes on all 6 faces only
        bool active;  // Brandon's special flag
        bool transparent;     //
        bool item_drop;       // does block drop items

        CubeMaterial material;
        ItemContainerType container_type;  // inventory and crafting bench blocks
        bool explosive; // causes an explosion when destroyed

        unsigned char max_damage;

        char name[DAT_NAME_MAX_LENGTH+1];

        //for dat debug assistance
        bool loaded;

    CubeProperties()
    {
        this->id = NULL_CUBE;
        this->type = ErrorCube;
        
        this->solid = true;
        this->occludes = true;
        this->active = true;
        this->transparent = false;
        this->item_drop = false;

        this->material = CUBE_MATERIAL_NONE;
        this->container_type = CONTAINER_TYPE_NONE;
        this->explosive = false;

        this->max_damage = 32;
        
        memset(this->name, 0, sizeof(this->name));

        this->loaded = false;
    }
};

extern struct FastCubeProperties* fast_cube_properties;
extern class CubeProperties* cube_properties;
extern class DatNameMap* cube_name_map;

void init_t_properties();
void end_t_properties();

class CubeProperties* get_cube_properties(CubeID id);

CubeMaterial get_cube_material(CubeID cube_id);

const char* get_cube_name(CubeID id);
CubeID get_cube_id(const char* name);

CubeType get_cube_type(CubeID id);

const char* get_cube_name_for_container(ItemContainerType container_type);
CubeID get_cube_id_for_container(ItemContainerType container_type);

bool is_valid_cube_name(const char* name);

// applies forward-compatible name versioning to give an id
// use only for the serializer
const char* get_compatible_cube_name(const char* name);

ItemContainerType get_container_type_for_cube(CubeID cube_id);

// checks against ERROR_CUBE/NULL_CUBE/EMPTY_CUBE and value range and used
inline bool isValidCube(CubeID cube_id) __attribute((always_inline));
inline bool isInUse(CubeID id) __attribute((always_inline));

// Properties by cube id

inline bool isSolid(CubeID id) __attribute((always_inline));
inline bool isOccludes(CubeID id) __attribute((always_inline));
inline bool isActive(CubeID id) __attribute((always_inline));
inline bool isTransparent(CubeID id) __attribute((always_inline));
inline bool isItemContainer(CubeID id) __attribute((always_inline));
inline bool isExplosive(CubeID id) __attribute((always_inline));
inline unsigned char maxDamage(CubeID id) __attribute((always_inline));

//Properties by coordinates

inline bool isSolid(int x, int y, int z) __attribute((always_inline));
inline bool isOccludes(int x, int y, int z) __attribute((always_inline));
inline bool isActive(int x, int y, int z) __attribute((always_inline));
inline bool isItemContainer(int x, int y, int z) __attribute((always_inline));
inline bool isExplosive(int x, int y, int z) __attribute((always_inline));
inline unsigned char maxDamage(int x, int y, int z) __attribute((always_inline));

}   // t_map
