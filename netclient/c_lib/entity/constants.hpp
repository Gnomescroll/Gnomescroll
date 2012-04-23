#pragma once

int MAX_OBJECT_TYPES = 256;
typedef enum
{
    OBJECT_NONE,    // null

    OBJECT_AGENT,   // agent

    // projectiles
    OBJECT_GRENADE,

    // game items
    OBJECT_FLAG,
    OBJECT_BASE,

    // refills
    OBJECT_HEALTH_REFILL,
    OBJECT_GRENADE_REFILL,
    OBJECT_LASER_REFILL,

    // gemstones
    OBJECT_GEMSTONE_MALACHITE,
    OBJECT_GEMSTONE_RUBY,
    OBJECT_GEMSTONE_TURQUOISE,
    OBJECT_GEMSTONE_SILVER,
    OBJECT_GEMSTONE_AMETHYST,
    OBJECT_GEMSTONE_JADE,
    OBJECT_GEMSTONE_ONYX,

    // block drops
    OBJECT_DIRT_BLOCK_DROP,
    OBJECT_STONE_BLOCK_DROP,
    OBJECT_SOFT_ROCK_BLOCK_DROP,
    OBJECT_MEDIUM_ROCK_BLOCK_DROP,
    OBJECT_HARD_ROCK_BLOCK_DROP,
    OBJECT_INFECTED_ROCK_BLOCK_DROP,

    // fabs
    OBJECT_AGENT_SPAWNER,
    OBJECT_TURRET,

    // mobs
    OBJECT_MONSTER_BOMB,
    OBJECT_MONSTER_BOX,
    OBJECT_MONSTER_SPAWNER,

    // inventories
    OBJECT_AGENT_INVENTORY,
    OBJECT_AGENT_TOOLBELT,
    OBJECT_NANITE_INVENTORY,
    OBJECT_CRAFTING_BENCH,
    
} ObjectType;

int MAX_COMPONENT_TYPES = 256;
typedef enum
{
    COMPONENT_NONE,

    COMPONENT_POSITION,
    COMPONENT_POSITION_MOMENTUM,
    COMPONENT_POSITION_CHANGED,
    COMPONENT_POSITION_MOMENTUM_CHANGED,
    COMPONENT_VERLET,

    COMPONENT_STACKABLE,

    COMPONENT_PICKUP,

    COMPONENT_TTL,
    COMPONENT_HIT_POINTS,
    
    COMPONENT_VOXEL_MODEL,

    COMPONENT_TEAM,
    COMPONENT_INDEXED_TEAM,

    COMPONENT_OWNER,

    COMPONENT_AGENT_SPAWNER,
    COMPONENT_MONSTER_SPAWNER,

    COMPONENT_SPAWN_CHILD,
    
    COMPONENT_MOTION_TARGETING,
    COMPONENT_WEAPON_TARGETING,

    COMPONENT_DIMENSION,

    COMPONENT_EXPLOSION,

    COMPONENT_RATE_LIMIT,

    #if DC_CLIENT
    COMPONENT_BILLBOARD_SPRITE,
    COMPONENT_COLORED_VOXEL,
    COMPONENT_TEXTURED_VOXEL,

    COMPONENT_VOXEL_ANIMATION,
    COMPONENT_SPRITE_ANIMATION,
    #endif
} ComponentType;

int MAX_COMPONENT_INTERFACE_TYPES = 256;
typedef enum
{
    COMPONENT_INTERFACE_NONE,
    COMPONENT_INTERFACE_PHYSICS,    // physics state setters/getters
    COMPONENT_INTERFACE_STACKABLE,  // stackable setters/getters
    COMPONENT_INTERFACE_UPDATE,     // update()
    COMPONENT_INTERFACE_TICK,       // tick()
    COMPONENT_INTERFACE_PICKUP,
    COMPONENT_INTERFACE_HEALTH,     // is_dead()
    COMPONENT_INTERFACE_TEAM,       // team getter/setter
    COMPONENT_INTERFACE_OWNER,      // owner getter/setter
    COMPONENT_INTERFACE_VOXEL_MODEL,
    COMPONENT_INTERFACE_SPAWNER,
    COMPONENT_INTERFACE_SPAWN_CHILD,
    COMPONENT_INTERFACE_TARGETING,
    COMPONENT_INTERFACE_DIMENSION,
    COMPONENT_INTERFACE_EXPLOSION,
    COMPONENT_INTERFACE_RATE_LIMIT,
    #if DC_CLIENT
    COMPONENT_INTERFACE_DRAW,       // draw()
    COMPONENT_INTERFACE_ANIMATION,
    #endif
} ComponentInterfaceType;


/* NULL values
 * TODO -- move
 */

#include <limits.h>
#include <float.h>
#include <c_lib/physics/vec3.hpp>

const int NULL_OWNER = INT_MAX; // owners are ids of agents. they will never be INT_MAX
const int NULL_TEAM = SHRT_MAX;  // team ids. will never be SHRT_MAX.
const unsigned int NULL_TEAM_INDEX = UINT_MAX;
const int NULL_HEALTH = INT_MAX;
const Vec3 NULL_POSITION = vec3_init(FLT_MAX,FLT_MAX,FLT_MAX);
const Vec3 NULL_MOMENTUM = vec3_init(0,0,0);
const Vec3 NULL_ANGLES = vec3_init(0,0,0);
const float NULL_HEIGHT = 1.0f;

