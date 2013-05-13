#pragma once

#include <entity/constants.hpp>
#include <entity/component/component.hpp>
#include <entity/component/component_list.hpp>
#include <entity/components/include.hpp>

namespace Components
{

/*
 * Adding a component:
 *
 * Add type. Create component, either inheriting exisiting interface or creating one
 * If creating new interface, add interface type
 * typedef list from template, declare extern, intern and alloc/free
 * Add subscribe/unsubscribe to switches
 *
 */

/* ComponentList declarations */

#ifdef COMPONENT_MAIN_INCLUDE
# define COMPONENT_LIST(NAME, LNAME, TYPE, MAX) \
    typedef ComponentList<NAME, TYPE, MAX> NAME##List; \
    extern NAME##List* LNAME##_list;
# define CALLABLE_COMPONENT_LIST(NAME, LNAME, TYPE, MAX) \
    typedef CallableComponentList<NAME, TYPE, MAX> NAME##List; \
    extern NAME##List* LNAME##_list;
# define CUSTOM_COMPONENT_LIST(NAME, LNAME) \
    extern NAME##List* LNAME##_list;
#else
# define COMPONENT_LIST(NAME, LNAME, TYPE, MAX) \
    typedef ComponentList<NAME, TYPE, MAX> NAME##List; \
    extern NAME##List* LNAME##_list; \
    NAME##List* LNAME##_list = NULL;
# define CALLABLE_COMPONENT_LIST(NAME, LNAME, TYPE, MAX) \
    typedef CallableComponentList<NAME, TYPE, MAX> NAME##List; \
    extern NAME##List* LNAME##_list; \
    NAME##List* LNAME##_list = NULL;
# define CUSTOM_COMPONENT_LIST(NAME, LNAME) \
    extern NAME##List* LNAME##_list; \
    NAME##List* LNAME##_list = NULL;
#endif

// TODO -- compute component list sizes from needs of mob list sizes
#define MOBS_MAX 1024

CALLABLE_COMPONENT_LIST(PositionPhysicsComponent, position_physics_component,
                        COMPONENT_POSITION, MOBS_MAX);
CALLABLE_COMPONENT_LIST(PositionMomentumPhysicsComponent, position_momentum_physics_component,
                        COMPONENT_POSITION_MOMENTUM, MOBS_MAX);
CALLABLE_COMPONENT_LIST(VerletPhysicsComponent, verlet_physics_component,
                        COMPONENT_VERLET, MOBS_MAX);

COMPONENT_LIST(TTLHealthComponent, ttl_health_component, COMPONENT_TTL, MOBS_MAX);
COMPONENT_LIST(HitPointsHealthComponent, hit_points_health_component,
               COMPONENT_HIT_POINTS, MOBS_MAX);

CUSTOM_COMPONENT_LIST(OwnerComponent, owner_component);

COMPONENT_LIST(VoxelModelComponent, voxel_model_component,
               COMPONENT_VOXEL_MODEL, MOBS_MAX);

COMPONENT_LIST(MonsterSpawnerComponent, monster_spawner_component,
               COMPONENT_MONSTER_SPAWNER, MOBS_MAX / 8);

COMPONENT_LIST(SpawnChildComponent, spawn_child_component,
               COMPONENT_SPAWN_CHILD, MOBS_MAX);

COMPONENT_LIST(DimensionComponent, dimension_component, COMPONENT_DIMENSION, MOBS_MAX);

COMPONENT_LIST(WeaponTargetingComponent, weapon_targeting_component,
               COMPONENT_WEAPON_TARGETING, MOBS_MAX / 2);
CALLABLE_COMPONENT_LIST(MotionTargetingComponent, motion_targeting_component,
               COMPONENT_MOTION_TARGETING, MOBS_MAX / 2);
COMPONENT_LIST(DestinationTargetingComponent, destination_targeting_component,
               COMPONENT_DESTINATION_TARGETING, MOBS_MAX / 2);
CALLABLE_COMPONENT_LIST(AgentTargetingComponent, agent_targeting_component,
               COMPONENT_AGENT_TARGETING, MOBS_MAX / 2);

COMPONENT_LIST(StateMachineComponent, state_machine_component,
               COMPONENT_STATE_MACHINE, MOBS_MAX / 2);

COMPONENT_LIST(WaitingComponent, waiting_component, COMPONENT_WAITING, MOBS_MAX / 2);

CALLABLE_COMPONENT_LIST(RateLimitComponent, rate_limit_component, COMPONENT_RATE_LIMIT, MOBS_MAX);

#if DC_CLIENT
COMPONENT_LIST(AnimationComponent, animation_component, COMPONENT_VOXEL_ANIMATION, MOBS_MAX / 4);
#endif

#if DC_SERVER
CUSTOM_COMPONENT_LIST(AgentSpawnerComponent, agent_spawner_component);
COMPONENT_LIST(ExplosionComponent, explosion_component, COMPONENT_EXPLOSION, MOBS_MAX / 2);
COMPONENT_LIST(ItemDropComponent, item_drop_component, COMPONENT_ITEM_DROP, MOBS_MAX);
CALLABLE_COMPONENT_LIST(HealerComponent, healer_component, COMPONENT_HEALER, MOBS_MAX / 4);
COMPONENT_LIST(KnockbackComponent, knockback_component, COMPONENT_KNOCKBACK, MOBS_MAX);
#endif

#undef COMPONENT_LIST

/* ComponentList handler switches */

Component* get_switch(ComponentType type);
void release_switch(Component* component);

void init_components();
void teardown_components();

} // Components
