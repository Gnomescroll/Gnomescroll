#pragma once

namespace Entities
{   // forward decl
class Entity;
}   // Entities

typedef void (*tickEntity)(class Entities::Entity*);      // for physics
typedef void (*updateEntity)(class Entities::Entity*);    // for draw prep

const size_t MOBS_MAX = 1024;

const int MAX_ENTITY_TYPES = 0xFF;

typedef enum
{
    ENTITY_DESTINATION,     // abstract entity

    ENTITY_AGENT,

    // projectiles
    ENTITY_GRENADE,

    // game items
    ENTITY_BASE,

    // fabs
    ENTITY_AGENT_SPAWNER,
    ENTITY_TURRET,
    ENTITY_ENERGY_CORE,

    // mobs
    ENTITY_MONSTER_BOMB,
    ENTITY_MONSTER_BOX,
    ENTITY_MONSTER_SPAWNER,
    ENTITY_MONSTER_SLIME,
    ENTITY_MONSTER_LIZARD_THIEF,
    ENTITY_MONSTER_BLUE_BLUB,
    ENTITY_MONSTER_RED_BLUB,

    // misc
    ENTITY_PLASMAGEN,
    ENTITY_CANNONBALL,

    NULL_ENTITY_TYPE = MAX_ENTITY_TYPES,
} EntityType;

const int MAX_COMPONENT_TYPES = 0xFF;

// the enums have weird naming for macro simplification
typedef enum
{
    COMPONENT_Position,
    COMPONENT_PositionMomentum,
    COMPONENT_Verlet,

    COMPONENT_TTL,
    COMPONENT_HitPoints,

    COMPONENT_VoxelModel,

    COMPONENT_Owner,

    COMPONENT_MonsterSpawner,

    COMPONENT_SpawnChild,

    COMPONENT_MotionTargeting,
    COMPONENT_WeaponTargeting,
    COMPONENT_DestinationTargeting,
    COMPONENT_AgentTargeting,

    COMPONENT_StateMachine,
    COMPONENT_Waiting,

    COMPONENT_Dimension,

    COMPONENT_RateLimit,

    COMPONENT_SpriteMob,

    #if DC_CLIENT
    COMPONENT_Animation,
    #endif

    #if DC_SERVER
    COMPONENT_Healer,
    COMPONENT_AgentSpawner,
    COMPONENT_Explosion,
    COMPONENT_ItemDrop,
    COMPONENT_Knockback,
    #endif

    NULL_COMPONENT = MAX_COMPONENT_TYPES,
} ComponentType;

const int MAX_COMPONENT_INTERFACE_TYPES = 0xFF;

typedef enum
{
    COMPONENT_INTERFACE_Physics,    // physics state setters/getters
    COMPONENT_INTERFACE_Stackable,  // stackable setters/getters
    COMPONENT_INTERFACE_Update,     // update()
    COMPONENT_INTERFACE_Tick,       // tick()
    COMPONENT_INTERFACE_Pickup,
    COMPONENT_INTERFACE_Health,     // is_dead()
    COMPONENT_INTERFACE_Owner,      // owner getter/setter
    COMPONENT_INTERFACE_VoxelModel,
    COMPONENT_INTERFACE_Spawner,
    COMPONENT_INTERFACE_SpawnChild,
    COMPONENT_INTERFACE_Targeting,
    COMPONENT_INTERFACE_Dimension,
    COMPONENT_INTERFACE_RateLimit,
    COMPONENT_INTERFACE_Healer,
    COMPONENT_INTERFACE_StateMachine,
    COMPONENT_INTERFACE_Waiting,

    COMPONENT_INTERFACE_SpriteMob,

    #if DC_CLIENT
    COMPONENT_INTERFACE_Draw,       // draw()
    COMPONENT_INTERFACE_Animation,
    #endif

    #if DC_SERVER
    COMPONENT_INTERFACE_Knockback,
    COMPONENT_INTERFACE_Explosion,
    COMPONENT_INTERFACE_ItemDrop,
    #endif

    NULL_COMPONENT_INTERFACE = MAX_COMPONENT_INTERFACE_TYPES,
} ComponentInterfaceType;

typedef enum
{
    NULL_ENTITY = 0xFFFF
} EntityID;

inline bool isValid(EntityType type)
{
    return (type >= 0 && type < MAX_ENTITY_TYPES);
}

inline bool isValid(ComponentType type)
{
    return (type >= 0 && type < MAX_COMPONENT_TYPES);
}

inline bool isValid(ComponentInterfaceType type)
{
    return (type >= 0 && type < MAX_COMPONENT_INTERFACE_TYPES);
}

inline bool isValid(EntityID id)
{
    return (id >= 0 && id < NULL_ENTITY);
}

typedef enum
{
    STATE_NONE = 0,
    STATE_WAITING,
    STATE_IN_TRANSIT,
    STATE_CHASE_AGENT,
} EntityState;

typedef void (*stateRouter) (class Entities::Entity*, EntityState);   // for state machine

#include <limits.h>
#include <physics/vec3.hpp>

const int MOB_BROADCAST_RATE = ONE_SECOND / 5;

const int NULL_OWNER = INT_MAX;          // owners are ids of agents. they will never be INT_MAX
const int NULL_HEALTH = INT_MAX;
const Vec3 NULL_POSITION = vec3_init(0);
const Vec3 NULL_MOMENTUM = vec3_init(0);
const Vec3 NULL_ANGLES = vec3_init(0);
const float NULL_HEIGHT = 1.0f;

const int MAX_SPAWNERS = 1024;
const EntityID BASE_SPAWN_ID = EntityID(1024);
