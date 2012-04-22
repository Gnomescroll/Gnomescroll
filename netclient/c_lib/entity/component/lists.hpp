#pragma once

#include <c_lib/entity/component/lists/agent_spawner_list.hpp>
#include <c_lib/entity/component/component_list.hpp>

namespace Components
{

/* ComponentList typedefs */
 
/* Physics */
const int MAX_PHYSICS_COMPONENTS = 4096;
typedef ComponentList<PositionPhysicsComponent, COMPONENT_POSITION, MAX_PHYSICS_COMPONENTS>
    PositionPhysicsComponentList;
typedef ComponentList<PositionMomentumPhysicsComponent, COMPONENT_POSITION_MOMENTUM, MAX_PHYSICS_COMPONENTS>
    PositionMomentumPhysicsComponentList;
typedef ComponentList<PositionChangedPhysicsComponent, COMPONENT_POSITION_CHANGED, MAX_PHYSICS_COMPONENTS>
    PositionChangedPhysicsComponentList;
typedef ComponentList<PositionMomentumChangedPhysicsComponent, COMPONENT_POSITION_MOMENTUM_CHANGED, MAX_PHYSICS_COMPONENTS>
    PositionMomentumChangedPhysicsComponentList;
typedef ComponentList<VerletPhysicsComponent, COMPONENT_VERLET, MAX_PHYSICS_COMPONENTS>
    VerletPhysicsComponentList;

/* Stackables */
const int MAX_STACKABLE_COMPONENTS = 4096;
typedef ComponentList<StackableComponent, COMPONENT_STACKABLE, MAX_STACKABLE_COMPONENTS>
    StackableComponentList;

#if DC_CLIENT
/* Drawing */
const int MAX_RENDERABLE_COMPONENTS = 4096;
//typedef ComponentList<BillboardSpriteComponent, COMPONENT_BILLBOARD_SPRITE, MAX_RENDERABLE_COMPONENTS>
    //BillboardSpriteComponentList;
class BillboardSpriteComponentList:
    public CallableComponentList<BillboardSpriteComponent, COMPONENT_BILLBOARD_SPRITE, MAX_RENDERABLE_COMPONENTS>
{};
class ColoredVoxelComponentList:
    public CallableComponentList<ColoredVoxelComponent, COMPONENT_COLORED_VOXEL, MAX_RENDERABLE_COMPONENTS>
{};
class TexturedVoxelComponentList:
    public CallableComponentList<TexturedVoxelComponent, COMPONENT_TEXTURED_VOXEL, MAX_RENDERABLE_COMPONENTS>
{};

/* Animations */
const int MAX_ANIMATION_COMPONENTS = 1024;
typedef ComponentList<AnimationComponent, COMPONENT_VOXEL_ANIMATION, MAX_ANIMATION_COMPONENTS>
    AnimationComponentList;
#endif

/* Pickup */
const int MAX_PICKUP_COMPONENTS = 2048;
typedef ComponentList<PickupComponent, COMPONENT_PICKUP, MAX_PICKUP_COMPONENTS>
    PickupComponentList;

/* Health */
const int MAX_HEALTH_COMPONENTS = 4096;
typedef ComponentList<TTLHealthComponent, COMPONENT_TTL, MAX_HEALTH_COMPONENTS>
    TTLHealthComponentList;
typedef ComponentList<HitPointsHealthComponent, COMPONENT_HIT_POINTS, MAX_HEALTH_COMPONENTS>
    HitPointsHealthComponentList;

/* Team */
const int MAX_TEAM_COMPONENTS = 1024;
typedef ComponentList<TeamComponent, COMPONENT_TEAM, MAX_TEAM_COMPONENTS>
    TeamComponentList;
typedef ComponentList<IndexedTeamComponent, COMPONENT_INDEXED_TEAM, MAX_TEAM_COMPONENTS>
    IndexedTeamComponentList;

/* Voxel Model */
const int MAX_VOXEL_MODEL_COMPONENTS = 1024;
typedef ComponentList<VoxelModelComponent, COMPONENT_VOXEL_MODEL, MAX_VOXEL_MODEL_COMPONENTS>
    VoxelModelComponentList;

/* Targeting */
const int MAX_TARGETING_COMPONENTS = 2048;
typedef ComponentList<WeaponTargetingComponent, COMPONENT_WEAPON_TARGETING, MAX_TARGETING_COMPONENTS>
    WeaponTargetingComponentList;
typedef ComponentList<MotionTargetingComponent, COMPONENT_MOTION_TARGETING, MAX_TARGETING_COMPONENTS>
    MotionTargetingComponentList;

/* Spawner */

const int MAX_MONSTER_SPAWNER_COMPONENTS = 512;
typedef ComponentList<MonsterSpawnerComponent, COMPONENT_MONSTER_SPAWNER, MAX_MONSTER_SPAWNER_COMPONENTS>
    MonsterSpawnerComponentList;

/* Spawn Child */
const int MAX_SPAWN_CHILD_COMPONENTS = 4096;
typedef ComponentList<SpawnChildComponent, COMPONENT_SPAWN_CHILD, MAX_SPAWN_CHILD_COMPONENTS>
    SpawnChildComponentList;

/* Dimensions */

const int DIMENSION_COMPONENT_MAX = 4096;
typedef ComponentList<DimensionComponent, COMPONENT_DIMENSION, DIMENSION_COMPONENT_MAX>
    DimensionComponentList;

#if DC_SERVER
/* Damaging Explosion */
const int EXPLOSION_COMPONENT_MAX = 2048;
typedef ComponentList<ExplosionComponent, COMPONENT_EXPLOSION, EXPLOSION_COMPONENT_MAX>
    ExplosionComponentList;
#endif

/* Rate Limiting */
const int RATE_LIMIT_COMPONENT_MAX = 4096;
typedef ComponentList<RateLimitComponent, COMPONENT_RATE_LIMIT, RATE_LIMIT_COMPONENT_MAX>
    RateLimitComponentList;

} // Components
