#define COMPONENT_MAIN_INCLUDE 1
# include "main.hpp"
#undef COMPONENT_MAIN_INCLUDE

//#include <entity/component/lists.hpp>

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

//#define COMPONENT_LIST(NAME, LNAME) \
    //NAME##List* LNAME##_list = NULL;

///* ComponentList declarations */

//PositionPhysicsComponentList* position_physics_component_list = NULL;
//PositionMomentumPhysicsComponentList* position_momentum_physics_component_list = NULL;
//VerletPhysicsComponentList* verlet_physics_component_list = NULL;

//TTLHealthComponentList* ttl_health_component_list = NULL;
//HitPointsHealthComponentList* hit_points_health_component_list = NULL;

//OwnerComponentList* owner_component_list = NULL;

//VoxelModelComponentList* voxel_model_component_list = NULL;

//MonsterSpawnerComponentList* monster_spawner_component_list = NULL;

//SpawnChildComponentList* spawn_child_component_list = NULL;

//DimensionComponentList* dimension_component_list = NULL;

//WeaponTargetingComponentList* weapon_targeting_component_list = NULL;
//MotionTargetingComponentList* motion_targeting_component_list = NULL;
//DestinationTargetingComponentList* destination_targeting_component_list = NULL;
//AgentTargetingComponentList* agent_targeting_component_list = NULL;

//StateMachineComponentList* state_machine_component_list = NULL;

//WaitingComponentList* waiting_component_list = NULL;

//#if DC_CLIENT
//AnimationComponentList* animation_component_list = NULL;
//#endif

//#if DC_SERVER
//AgentSpawnerComponentList* agent_spawner_component_list = NULL;
//ExplosionComponentList* explosion_component_list = NULL;
//ItemDropComponentList* item_drop_component_list = NULL;
//HealerComponentList* healer_component_list;
//COMPONENT_LIST(KnockbackComponent,
//#endif

//RateLimitComponentList* rate_limit_component_list = NULL;

//#undef COMPONENT_LIST

/* ComponentList handler switches */

Component* get_switch(ComponentType type)
{
    switch (type)
    {
        case COMPONENT_POSITION:
            return position_physics_component_list->subscribe();
        case COMPONENT_POSITION_MOMENTUM:
            return position_momentum_physics_component_list->subscribe();
        case COMPONENT_VERLET:
            return verlet_physics_component_list->subscribe();

        case COMPONENT_TTL:
            return ttl_health_component_list->subscribe();
        case COMPONENT_HIT_POINTS:
            return hit_points_health_component_list->subscribe();

        case COMPONENT_OWNER:
            return owner_component_list->subscribe();

        case COMPONENT_VOXEL_MODEL:
            return voxel_model_component_list->subscribe();

        case COMPONENT_MONSTER_SPAWNER:
            return monster_spawner_component_list->subscribe();

        case COMPONENT_SPAWN_CHILD:
            return spawn_child_component_list->subscribe();

        case COMPONENT_DIMENSION:
            return dimension_component_list->subscribe();

        case COMPONENT_WEAPON_TARGETING:
            return weapon_targeting_component_list->subscribe();
        case COMPONENT_MOTION_TARGETING:
            return motion_targeting_component_list->subscribe();
        case COMPONENT_DESTINATION_TARGETING:
            return destination_targeting_component_list->subscribe();
        case COMPONENT_AGENT_TARGETING:
            return agent_targeting_component_list->subscribe();

        case COMPONENT_STATE_MACHINE:
            return state_machine_component_list->subscribe();

        case COMPONENT_WAITING:
            return waiting_component_list->subscribe();

        case COMPONENT_RATE_LIMIT:
            return rate_limit_component_list->subscribe();

        #if DC_CLIENT
        case COMPONENT_VOXEL_ANIMATION:
            return animation_component_list->subscribe();
        #endif

        #if DC_SERVER
        case COMPONENT_AGENT_SPAWNER:
            return agent_spawner_component_list->subscribe();
        case COMPONENT_EXPLOSION:
            return explosion_component_list->subscribe();
        case COMPONENT_ITEM_DROP:
            return item_drop_component_list->subscribe();
        case COMPONENT_HEALER:
            return healer_component_list->subscribe();
        case COMPONENT_KNOCKBACK:
            return knockback_component_list->subscribe();
        #endif

        #if DC_CLIENT
        case COMPONENT_HEALER:
        case COMPONENT_AGENT_SPAWNER:
        #endif
        case COMPONENT_NONE:
            printf("ERROR: Component::get() -- unknown ComponentType %d\n", type);
            return NULL;
    }
    return NULL;
}

void release_switch(Component* component)
{
    IF_ASSERT(component == NULL) return;
    switch (component->type)
    {
        case COMPONENT_POSITION:
            position_physics_component_list->unsubscribe((PositionPhysicsComponent*)component);
            break;
        case COMPONENT_POSITION_MOMENTUM:
            position_momentum_physics_component_list->unsubscribe((PositionMomentumPhysicsComponent*)component);
            break;
        case COMPONENT_VERLET:
            verlet_physics_component_list->unsubscribe((VerletPhysicsComponent*)component);
            break;

        case COMPONENT_TTL:
            ttl_health_component_list->unsubscribe((TTLHealthComponent*)component);
            break;
        case COMPONENT_HIT_POINTS:
            hit_points_health_component_list->unsubscribe((HitPointsHealthComponent*)component);
            break;

        case COMPONENT_OWNER:
            owner_component_list->unsubscribe((OwnerComponent*)component);
            break;

        case COMPONENT_VOXEL_MODEL:
            voxel_model_component_list->unsubscribe((VoxelModelComponent*)component);
            break;

        case COMPONENT_MONSTER_SPAWNER:
            monster_spawner_component_list->unsubscribe((MonsterSpawnerComponent*)component);
            break;

        case COMPONENT_SPAWN_CHILD:
            spawn_child_component_list->unsubscribe((SpawnChildComponent*)component);
            break;

        case COMPONENT_DIMENSION:
            dimension_component_list->unsubscribe((DimensionComponent*)component);
            break;

        case COMPONENT_WEAPON_TARGETING:
            weapon_targeting_component_list->unsubscribe((WeaponTargetingComponent*)component);
            break;
        case COMPONENT_MOTION_TARGETING:
            motion_targeting_component_list->unsubscribe((MotionTargetingComponent*)component);
            break;
        case COMPONENT_DESTINATION_TARGETING:
            destination_targeting_component_list->unsubscribe((DestinationTargetingComponent*)component);
            break;
        case COMPONENT_AGENT_TARGETING:
            agent_targeting_component_list->unsubscribe((AgentTargetingComponent*)component);
            break;

        case COMPONENT_STATE_MACHINE:
            state_machine_component_list->unsubscribe((StateMachineComponent*)component);
            break;

        case COMPONENT_WAITING:
            waiting_component_list->unsubscribe((WaitingComponent*)component);
            break;

        case COMPONENT_RATE_LIMIT:
            rate_limit_component_list->unsubscribe((RateLimitComponent*)component);
            break;

        #if DC_CLIENT
        case COMPONENT_VOXEL_ANIMATION:
            animation_component_list->unsubscribe((AnimationComponent*)component);
            break;
        #endif

        #if DC_SERVER
        case COMPONENT_AGENT_SPAWNER:
            agent_spawner_component_list->unsubscribe((AgentSpawnerComponent*)component);
            break;
        case COMPONENT_EXPLOSION:
            explosion_component_list->unsubscribe((ExplosionComponent*)component);
            break;
        case COMPONENT_ITEM_DROP:
            item_drop_component_list->unsubscribe((ItemDropComponent*)component);
            break;
        case COMPONENT_HEALER:
            healer_component_list->unsubscribe((HealerComponent*)component);
            break;
        case COMPONENT_KNOCKBACK:
            knockback_component_list->unsubscribe((KnockbackComponent*)component);
            break;
        #endif

        #if DC_CLIENT
        case COMPONENT_HEALER:
        case COMPONENT_AGENT_SPAWNER:
        #endif
        case COMPONENT_NONE:
            printf("ERROR: Component::get() -- unknown ComponentType %d\n", component->type);
            break;
    }
}

void init_components()
{
    position_physics_component_list = new PositionPhysicsComponentList;
    position_momentum_physics_component_list = new PositionMomentumPhysicsComponentList;
    verlet_physics_component_list = new VerletPhysicsComponentList;

    ttl_health_component_list = new TTLHealthComponentList;
    hit_points_health_component_list = new HitPointsHealthComponentList;

    owner_component_list = new OwnerComponentList;

    voxel_model_component_list = new VoxelModelComponentList;

    monster_spawner_component_list = new MonsterSpawnerComponentList;

    spawn_child_component_list = new SpawnChildComponentList;

    dimension_component_list = new DimensionComponentList;

    weapon_targeting_component_list = new WeaponTargetingComponentList;
    motion_targeting_component_list = new MotionTargetingComponentList;
    destination_targeting_component_list = new DestinationTargetingComponentList;
    agent_targeting_component_list = new AgentTargetingComponentList;

    state_machine_component_list = new StateMachineComponentList;

    waiting_component_list = new WaitingComponentList;

    rate_limit_component_list = new RateLimitComponentList;

    #if DC_CLIENT
    animation_component_list = new AnimationComponentList;
    #endif

    #if DC_SERVER
    agent_spawner_component_list = new AgentSpawnerComponentList;
    explosion_component_list = new ExplosionComponentList;
    item_drop_component_list = new ItemDropComponentList;
    healer_component_list = new HealerComponentList;
    knockback_component_list = new KnockbackComponentList;
    #endif
}

void teardown_components()
{
    delete position_physics_component_list;
    delete position_momentum_physics_component_list;
    delete verlet_physics_component_list;
    delete ttl_health_component_list;
    delete hit_points_health_component_list;
    delete owner_component_list;
    delete voxel_model_component_list;
    delete monster_spawner_component_list;
    delete spawn_child_component_list;
    delete dimension_component_list;
    delete weapon_targeting_component_list;
    delete motion_targeting_component_list;
    delete destination_targeting_component_list;
    delete agent_targeting_component_list;
    delete state_machine_component_list;
    delete waiting_component_list;
    delete rate_limit_component_list;
    #if DC_CLIENT
    delete animation_component_list;
    #endif
    #if DC_SERVER
    delete agent_spawner_component_list;
    delete explosion_component_list;
    delete item_drop_component_list;
    delete healer_component_list;
    delete knockback_component_list;
    #endif
}

} // Components
