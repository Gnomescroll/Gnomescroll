#pragma once

#include <item/common/enum.hpp>
#include <common/macros.hpp>
#include <t_map/common/constants.hpp>
#include <t_map/common/types.hpp>

namespace t_map
{

class CubeProperties 
{
    public:
        CubeID id;
        CubeType type;
        
        bool solid;
        bool occludes;  // occludes on all 6 faces only
        bool transparent;     //
        bool item_drop;       // does block drop items
        bool item_container;  // inventory and crafting bench blocks

        bool magic;  // Brandon's special flag

        CubeMaterial material;

        unsigned char max_damage;

        char name[CUBE_NAME_MAX_LENGTH+1];

        //for dat debug assistance
        bool in_use;

    CubeProperties()
    {
        this->id = NULL_CUBE;
        this->type = ErrorCube;
        
        this->solid = true;
        this->occludes = true;
        this->transparent = false;
        this->item_drop = false;
        this->item_container = false;
        this->material = material;

        this->magic = true;

        this->material = CUBE_MATERIAL_NONE;

        this->max_damage = 32;
        
        memset(this->name, 0, sizeof(this->name));

        this->in_use = false;
    }
};

extern class CubeProperties* cube_properties;

void init_t_properties();
void end_t_properties();

class CubeProperties* get_cube_properties(CubeID id);

CubeMaterial get_cube_material(CubeID cube_id);

const char* get_cube_name(CubeID id);
CubeID get_cube_id(const char* name);

bool is_valid_cube_name(const char* name);

// applies forward-compatible name versioning to give an id
// use only for the serializer
CubeID get_compatible_cube_id(const char* name);

// checks against ERROR_CUBE/NULL_CUBE/EMPTY_CUBE and value range and used
inline bool isValidCube(CubeID cube_id) __attribute((always_inline));
inline bool isInUse(CubeID id) __attribute((always_inline));

// Properties by cube id

inline bool isSolid(CubeID id) __attribute((always_inline));
inline bool isOccludes(CubeID id) __attribute((always_inline));
inline bool isMagic(CubeID id) __attribute((always_inline));
inline bool isTransparent(CubeID id) __attribute((always_inline));
inline bool isItemContainer(CubeID id) __attribute((always_inline));
inline unsigned char maxDamage(CubeID id) __attribute((always_inline));

//Properties by coordinates

inline bool isSolid(int x, int y, int z) __attribute((always_inline));
inline bool isOccludes(int x, int y, int z) __attribute((always_inline));
inline bool isMagic(int x, int y, int z) __attribute((always_inline));
inline bool isItemContainer(int x, int y, int z) __attribute((always_inline));
inline unsigned char maxDamage(int x, int y, int z) __attribute((always_inline));

}   // t_map
