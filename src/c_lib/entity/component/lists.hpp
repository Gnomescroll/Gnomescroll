#pragma once

#if DC_SERVER
#include <entity/component/lists/agent_spawner_list.hpp>
#endif
#include <entity/component/component_list.hpp>

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

/* Health */
const int MAX_HEALTH_COMPONENTS = 4096;
typedef ComponentList<TTLHealthComponent, COMPONENT_TTL, MAX_HEALTH_COMPONENTS>
    TTLHealthComponentList;
typedef ComponentList<HitPointsHealthComponent, COMPONENT_HIT_POINTS, MAX_HEALTH_COMPONENTS>
    HitPointsHealthComponentList;

/* Voxel Model */
const int MAX_VOXEL_MODEL_COMPONENTS = 1024;
typedef ComponentList<VoxelModelComponent, COMPONENT_VOXEL_MODEL, MAX_VOXEL_MODEL_COMPONENTS>
    VoxelModelComponentList;

/* Targeting */
const int MAX_TARGETING_COMPONENTS = 2048;
typedef ComponentList<WeaponTargetingComponent, COMPONENT_WEAPON_TARGETING, MAX_TARGETING_COMPONENTS>
    WeaponTargetingComponentList;
typedef CallableComponentList<MotionTargetingComponent, COMPONENT_MOTION_TARGETING, MAX_TARGETING_COMPONENTS>
    MotionTargetingComponentList;
typedef ComponentList<DestinationTargetingComponent, COMPONENT_DESTINATION_TARGETING, MAX_TARGETING_COMPONENTS>
    DestinationTargetingComponentList;
typedef CallableComponentList<AgentTargetingComponent, COMPONENT_AGENT_TARGETING, MAX_TARGETING_COMPONENTS>
    AgentTargetingComponentList;

/* state Machines */
const int MAX_STATE_MACHINE_COMPONENTS = 2048;
typedef ComponentList<StateMachineComponent, COMPONENT_STATE_MACHINE, MAX_STATE_MACHINE_COMPONENTS>
    StateMachineComponentList;

/* Waiting State */
const int MAX_WAITING_COMPONENTS = 2048;
typedef ComponentList<WaitingComponent, COMPONENT_WAITING, MAX_WAITING_COMPONENTS>
    WaitingComponentList;

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

/* Rate Limiting */
const int RATE_LIMIT_COMPONENT_MAX = 4096;
class RateLimitComponentList:
    public CallableComponentList<RateLimitComponent, COMPONENT_RATE_LIMIT, RATE_LIMIT_COMPONENT_MAX>
{};

#if DC_CLIENT
/* Animations */
const int MAX_ANIMATION_COMPONENTS = 1024;
typedef ComponentList<AnimationComponent, COMPONENT_VOXEL_ANIMATION, MAX_ANIMATION_COMPONENTS>
    AnimationComponentList;
#endif

#if DC_SERVER
/* Damaging Explosion */
const int EXPLOSION_COMPONENT_MAX = 2048;
typedef ComponentList<ExplosionComponent, COMPONENT_EXPLOSION, EXPLOSION_COMPONENT_MAX>
    ExplosionComponentList;

/* Item Drop */
const int ITEM_DROP_COMPONENT_MAX = 4096;
typedef ComponentList<ItemDropComponent, COMPONENT_ITEM_DROP, ITEM_DROP_COMPONENT_MAX>
    ItemDropComponentList;

/* Healer */
const int HEALER_COMPONENT_MAX = 1024;
class HealerComponentList:
    public CallableComponentList<HealerComponent, COMPONENT_HEALER, HEALER_COMPONENT_MAX>
{};
#endif

} // Components
