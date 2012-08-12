#pragma once

#include <item/common/enum.hpp>
#include <common/macros.hpp>

namespace t_map
{

struct cubeProperties 
{
    bool active;
    bool solid;
    bool occludes;
    bool transparent;
    bool item_drop; //does block drop items
    bool item_container;  //inventory and crafting bench blocks
    bool reserved7;
    bool reserved8;

    unsigned char max_damage;
    unsigned char color_type; //flat, discrete and perlin
    //bool solid;
    //bool gravity;

    //for dat debug assistance
    bool in_use;
};

const int MAX_CUBES = 256;

extern struct cubeProperties* cube_list; //[MAX_CUBES];

void init_t_properties();

void end_t_properties();

struct cubeProperties* get_cube(int id) GNOMESCROLL_API;

using namespace t_map;

inline struct cubeProperties* _get_cube(int id) __attribute((always_inline)) GNOMESCROLL_API;

void set_cube_name(int id, const  char* name, int length);
void set_cube_name(int id, const char* name);

const char* get_cube_name(int id);
int get_cube_id(const char* name);

int dat_get_cube_id(const char* name);  //use for dat files

inline bool isErrorBlock(int id) __attribute((always_inline));
inline bool isValidID(int id) __attribute((always_inline));
inline bool isInUse(int id) __attribute((always_inline));

}   // t_map


/*
    LUA interface
*/

void LUA_set_block_name(int id, const char* name, int length);

void LUA_set_block_properties(int id, int active, int solid, int occludes, int transparent);
void LUA_set_block_max_damage(int id, int max_damage);
void LUA_set_block_color_type(int id, int color_type);

/*
Cube Properties
*/


/*
    Properties by cube id
*/

inline bool isActive(int id) __attribute((always_inline));
//inline bool isSolid(int id) __attribute((always_inline));
bool isSolid(int id);
inline bool isOccludes(int id) __attribute((always_inline));
inline bool isTransparent(int id) __attribute((always_inline));

inline bool isItemContainer(int id) __attribute((always_inline));

/*
    Properties by coordinates
*/

inline bool isActive(int x, int y, int z) __attribute((always_inline));
inline bool isSolid(int x, int y, int z) __attribute((always_inline));
inline bool isOccludes(int x, int y, int z) __attribute((always_inline));

inline bool isItemContainer(int x, int y, int z) __attribute((always_inline));

/*
    Map Damage
*/

inline int maxDamage(int id) __attribute((always_inline));
