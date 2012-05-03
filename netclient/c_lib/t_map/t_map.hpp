#pragma once

#include <common/defines.h>
#include <t_map/constants.hpp>

#include <t_map/t_map_class.hpp>

struct MapDimension
{
    int x,y,z;
};

extern struct MapDimension map_dim;

#include <t_map/common/map_element.hpp>

namespace t_map
{

extern Terrain_map* main_map;

typedef enum TerrainModificationAction
{
    TMA_PICK=0,
    TMA_APPLIER,
    TMA_GRENADE,
    TMA_LASER,
    TMA_TURRET,
    TMA_MONSTER_BOX
} TerrainModificationAction;
 
void init_t_map() GNOMESCROLL_API;

#ifdef DC_CLIENT
void init_for_draw() GNOMESCROLL_API;
#endif

class Terrain_map* get_map();

inline int get(int x, int y, int z)
{
    return main_map->get_block(x,y,z);
}

inline void set(int x, int y, int z, int value)
{
    main_map->set_block(x,y,z,value);
}

int apply_damage(int x, int y, int z, int dmg);
#ifdef DC_SERVER
void apply_damage_broadcast(int x, int y, int z, int dmg, TerrainModificationAction action);
#endif

int get_highest_open_block(int x, int y, int agent_height);
int get_highest_open_block(int x, int y);
int get_lowest_open_block(int x, int y, int n);
int get_highest_solid_block(int x, int y, int z=MAP_HEIGHT);
int get_lowest_solid_block(int x, int y);

}

int _get(int x, int y, int z) GNOMESCROLL_API;
void _set(int x, int y, int z, int value) GNOMESCROLL_API;

/*
    Deprecate eventually
*/
#ifdef DC_SERVER
void _set_broadcast(int x, int y, int z, int value);
/*
    Deprecated
*/

void send_map_metadata(int client_id);  //Deprecate
#endif
//void send_map_metadata();   //Deprecate
void set_map_size(int x, int y, int z); //Deprecate

#include "t_properties.hpp"


/*
    Move this somewhere
*/
int _get_highest_open_block(int x, int y, int agent_height);
int _get_highest_open_block(int x, int y);
int _get_lowest_open_block(int x, int y, int n);
int _get_highest_solid_block(int x, int y, int z=t_map::MAP_HEIGHT);
int _get_lowest_solid_block(int x, int y);
bool point_in_map(int x, int y, int z);


int get_height_at(int x, int y);
#if DC_CLIENT
unsigned char get_cached_height(int x, int y);
#endif

