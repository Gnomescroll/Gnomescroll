#pragma once

#include <physics/vec3.hpp>

#include <common/defines.h>
#include <t_map/common/constants.hpp>
#include <t_map/common/map_element.hpp>

#include "t_properties.hpp"

//#include <t_map/t_map_class.hpp>

struct MapDimension
{
    int x,y,z;
};

extern struct MapDimension map_dim;



namespace t_map
{

extern class Terrain_map* main_map;

typedef enum TerrainModificationAction
{
    TMA_NONE=0,
    TMA_PICK,
    TMA_APPLIER,
    TMA_GRENADE,
    TMA_LASER,
    TMA_TURRET,
    TMA_MONSTER_BOX,
    TMA_MONSTER_BOMB,
    TMA_PLACE_BLOCK,
} TerrainModificationAction;
 
void init_t_map();

#if DC_CLIENT
void init_for_draw();
#endif

class Terrain_map* get_map();

int get(int x, int y, int z);
void set(int x, int y, int z, int value);
inline void set_fast(int x, int y, int z, int value) __attribute__((always_inline));
void set_palette(int x, int y, int z, int value);

int apply_damage(int x, int y, int z, int dmg);
#if DC_SERVER
void apply_damage_broadcast(int x, int y, int z, int dmg, TerrainModificationAction action);

void broadcast_set_block_action(int x, int y, int z, int block, int action);
void broadcast_set_block(int x, int y, int z, int block);
void broadcast_set_block_palette(int x, int y, int z, int block, int palette);

#endif

inline int get_highest_open_block(int x, int y, int vertical_gap);
inline int get_highest_open_block(int x, int y);
inline int get_lowest_open_block(int x, int y, int n);
inline int get_highest_solid_block(int x, int y);
inline int get_highest_solid_block(int x, int y, int z);
inline int get_lowest_solid_block(int x, int y);

}   // t_map
