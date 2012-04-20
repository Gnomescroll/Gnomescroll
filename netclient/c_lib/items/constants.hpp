#pragma once

/*
    #defines, constants for items
*/

#include <c_lib/common/common.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/game/game.hpp>

// TODO - deprecate
const int COST_SPAWNER = 5;
const int COST_TURRET = 2;

unsigned int get_object_cost(ObjectType obj)
{
    unsigned int cost;
    switch (obj) {
        case OBJECT_AGENT_SPAWNER:
            cost = COST_SPAWNER;
            break;
        case OBJECT_TURRET:
            cost = COST_TURRET;
            break;
        default:
            printf("get_object_cost -- unknown obj type %d\n", obj);
            return 0;
    }
    return cost;
}

//// Spawners
//const int SPAWNER_SPAWN_RADIUS = 5;
//const int SPAWNERS_PER_TEAM = 9;
//const int SPAWNER_MAX = SPAWNERS_PER_TEAM * N_TEAMS;
//const int SPAWNER_HEALTH = 300;
//const float SPAWNER_HEIGHT = 1.9f;
//const int BASE_SPAWN_ID = 255;

//// Team indexing
//const unsigned int TEAM_INDEX_NONE = 255;
////const unsigned int TEAM_INDEX_MAX = (TEAM_INDEX_NONE+1) / N_TEAMS;

//// Turrets
//const int TURRET_MAX = 256;
//const int TURRET_HEALTH = 125;
//const float TURRET_HEIGHT = 1.9f;
//const float TURRET_CAMERA_HEIGHT = 1.6f;
//const float TURRET_SIGHT_RANGE = 128.0f - 16.0f;
//const int TURRET_FIRE_LIMIT = 45; // ticks
//const int TURRET_BLOCK_DAMAGE = 8;
//const int TURRET_AGENT_DAMAGE = 10;
//const float TURRET_TARGET_LOCK_CHANCE = 0.85f;  // likelihood of acquiring a target in view
//const float TURRET_EXPLOSION_RADIUS = 2.0f;
//const int TURRET_EXPLOSION_DAMAGE = 40;
//const int TURRET_LASER_VOXEL_DAMAGE_RADIUS = 2;
//const float TURRET_LASER_BIAS = 1.0f;

//refills
namespace ItemDrops
{

typedef enum
{
    GRENADE_REFILL,
    LASER_REFILL,
    HEALTH_REFILL,
    MEAT,
    MALACHITE,
    RUBY,
    TURQUOISE,
    SILVER,
    AMETHYST,
    JADE,
    ONYX,

} PickupSpriteTypes;

/* Refills */

const int REFILL_MAX = 512;

const int DEFAULT_PICKUP_ITEM_TTL = 30 * 12;    // 12 seconds
const float DEFAULT_PICKUP_ITEM_RADIUS = 2.0f;
const float DEFAULT_PICKUP_ITEM_SCALE = 0.5f;
const float DEFAULT_PICKUP_ITEM_DAMP = 0.4f;
const float DEFAULT_PICKUP_ITEM_MASS = 1.0f;

/* Grenade refill */

const int GRENADE_REFILL_SPRITE_INDEX = 6;
const float GRENADE_REFILL_TEXTURE_SCALE = 0.5f;
const float GRENADE_REFILL_MASS = 1.0f;
const int GRENADE_REFILL_TTL = 30 * 12;   // 12 seconds
const float GRENADE_REFILL_DAMP = 0.4f;

/* Laser refill */

const int LASER_REFILL_SPRITE_INDEX = 7;
const float LASER_REFILL_TEXTURE_SCALE = 0.5f;
const float LASER_REFILL_MASS = 1.0f;
const int LASER_REFILL_TTL = 30 * 12;   // 12 seconds
const float LASER_REFILL_DAMP = 0.4f;

/* Health refill */

const int HEALTH_REFILL_SPRITE_INDEX = 23;
const float HEALTH_REFILL_TEXTURE_SCALE = 0.5f;
const float HEALTH_REFILL_MASS = 1.0f;
const int HEALTH_REFILL_TTL = 30 * 12;   // 12 seconds
const float HEALTH_REFILL_DAMP = 0.4f;

/* Minivox drops */
const float MINIVOX_ITEM_ROTATION_THETA = 0.02f;
const float MINIVOX_ITEM_ROTATION_PHI = 0.0f;

/* Block drops */

// defaults
const int BLOCK_DROP_MAX = 1024;
const int BLOCK_DROP_TTL = 30 * 12;
const float BLOCK_DROP_SIZE = 0.5f;
const float BLOCK_DROP_DAMP = 0.4f;
const float BLOCK_DROP_MASS = 1.0f;
const int BLOCK_DROP_SPRITE_INDEX = 0;
const struct Color BLOCK_DROP_COLOR = { 255, 255, 255 };
const float BLOCK_DROP_TEXTURED_PIXEL_WIDTH = 4;

//typedef enum
//{
    //DIRT,
    //STONE,
    //SOFT_ROCK,
    //MEDIUM_ROCK,
    //HARD_ROCK,
    //INFECTED_ROCK,
//} BlockDropSubtypes;

const struct Color DIRT_COLOR = { 150, 98, 0 };
const struct Color STONE_COLOR = { 140, 140, 140 };

//const int DIRT_SPRITE_INDEX = 0;
//const int STONE_SPRITE_INDEX = 1;
//const int SOFT_ROCK_SPRITE_INDEX = 2;
//const int MEDIUM_ROCK_SPRITE_INDEX = 3;
//const int HARD_ROCK_SPRITE_INDEX = 4;
//const int INFECTED_ROCK_SPRITE_INDEX = 5;

/* Gems */
const int GEMSTONE_PICKUP_RADIUS = 1.0f;
const int GEMSTONE_MAX = 256 * 16;
const float GEMSTONE_SCALE = 0.4f;
const int MALACHITE_SPRITE_INDEX = 0;
const int RUBY_SPRITE_INDEX = 2;
const int TURQUOISE_SPRITE_INDEX = 3;
const int SILVER_SPRITE_INDEX = 4;
const int AMETHYST_SPRITE_INDEX = 18;
const int JADE_SPRITE_INDEX = 19;
const int ONYX_SPRITE_INDEX = 20;

/* Misc */
const int MEAT_MAX = 256;
const float MEAT_SCALE = 0.5f;
const int MEAT_SPRITE_INDEX = 1;

const int SPAWNER_SPRITE_INDEX = 6;
const int TURRET_SPRITE_INDEX = 7;

int get_object_type_sprite_index(ObjectType type)
{
    const int ERROR_SPRITE = 48;    // BUG -- CANNOT SPECIFY 3rd row or higher textures
    //switch (type)
    //{

        //case OBJECT_BLOCK_DROP:
            //switch (subtype)
            //{
                //default: return ERROR_SPRITE;
            //}
            //break;
            
        //case OBJECT_GEMSTONE:
            //switch (subtype)
            //{
                //case MALACHITE:
                    //return MALACHITE_SPRITE_INDEX;
                //case RUBY:
                    //return RUBY_SPRITE_INDEX;
                //case TURQUOISE:
                    //return TURQUOISE_SPRITE_INDEX;
                //case SILVER:
                    //return SILVER_SPRITE_INDEX;
                //case AMETHYST:
                    //return AMETHYST_SPRITE_INDEX;
                //case JADE:
                    //return JADE_SPRITE_INDEX;
                //case ONYX:
                    //return ONYX_SPRITE_INDEX;
                //default: return ERROR_SPRITE;
            //}
            //break;

        //case OBJECT_REFILL:
            //switch (subtype)
            //{
                //case GRENADE_REFILL:
                    //return GRENADE_REFILL_SPRITE_INDEX;
                //case LASER_REFILL:
                    //return LASER_REFILL_SPRITE_INDEX;
                //case HEALTH_REFILL:
                    //return HEALTH_REFILL_SPRITE_INDEX;
                //default: return ERROR_SPRITE;
            //}
            //break;

        //case OBJECT_MEAT:
            //return MEAT_SPRITE_INDEX;

         ////Functional items
        //case OBJECT_AGENT_SPAWNER:
            //return SPAWNER_SPRITE_INDEX;
        //case OBJECT_TURRET:
            //return TURRET_SPRITE_INDEX;

        //default: return ERROR_SPRITE;
    //}
    return ERROR_SPRITE;
}

} // ItemDrops


// Inventory
const int INVENTORY_MAX = 1024;

typedef enum
{
    INVENTORY_TYPE_AGENT,
    INVENTORY_TYPE_TMAP,
    INVENTORY_TYPE_TMECH,
} InventoryTypes;

int get_max_stack_size(ObjectType type)
{
    return 1;
}
