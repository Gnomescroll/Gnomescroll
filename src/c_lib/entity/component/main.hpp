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

extern PositionPhysicsComponentList* position_physics_component_list;
extern PositionMomentumPhysicsComponentList* position_momentum_physics_component_list;
extern PositionChangedPhysicsComponentList* position_changed_physics_component_list;
extern PositionMomentumChangedPhysicsComponentList* position_momentum_changed_physics_component_list;
extern VerletPhysicsComponentList* verlet_physics_component_list;

extern StackableComponentList* stackable_component_list;

extern PickupComponentList* pickup_component_list;

extern TTLHealthComponentList* ttl_health_component_list;
extern HitPointsHealthComponentList* hit_points_health_component_list;

extern OwnerComponentList* owner_component_list;

extern VoxelModelComponentList* voxel_model_component_list;

extern MonsterSpawnerComponentList* monster_spawner_component_list;

extern SpawnChildComponentList* spawn_child_component_list;

extern DimensionComponentList* dimension_component_list;

extern WeaponTargetingComponentList* weapon_targeting_component_list;
extern MotionTargetingComponentList* motion_targeting_component_list;

extern RateLimitComponentList* rate_limit_component_list;

#if DC_CLIENT
extern BillboardSpriteComponentList* billboard_sprite_component_list;
extern ColoredVoxelComponentList* colored_voxel_component_list;
extern TexturedVoxelComponentList* textured_voxel_component_list;

extern AnimationComponentList* animation_component_list;
#endif

#if DC_SERVER
extern AgentSpawnerComponentList* agent_spawner_component_list;
extern ExplosionComponentList* explosion_component_list;
extern ItemDropComponentList* item_drop_component_list;
#endif

/* ComponentList handler switches */

Component* get_switch(ComponentType type);
void release_switch(Component* component);

void init_components();
void teardown_components();

} // Components
