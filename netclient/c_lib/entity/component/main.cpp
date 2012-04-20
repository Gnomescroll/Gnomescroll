#include "main.hpp"

#include <c_lib/entity/component/lists.hpp>

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

PositionPhysicsComponentList* position_physics_component_list = NULL;
PositionMomentumPhysicsComponentList* position_momentum_physics_component_list = NULL;
PositionChangedPhysicsComponentList* position_changed_physics_component_list = NULL;
PositionMomentumChangedPhysicsComponentList* position_momentum_changed_physics_component_list = NULL;
VerletPhysicsComponentList* verlet_physics_component_list = NULL;

StackableComponentList* stackable_component_list = NULL;

#if DC_CLIENT
BillboardSpriteComponentList* billboard_sprite_component_list = NULL;
ColoredVoxelComponentList* colored_voxel_component_list = NULL;
TexturedVoxelComponentList* textured_voxel_component_list = NULL;
#endif

PickupComponentList* pickup_component_list = NULL;

TTLHealthComponentList* ttl_health_component_list = NULL;
HitPointsHealthComponentList* hit_points_health_component_list = NULL;

TeamComponentList* team_component_list = NULL;
IndexedTeamComponentList* indexed_team_component_list = NULL;

OwnerComponentList* owner_component_list = NULL;

VoxelModelComponentList* voxel_model_component_list = NULL;

MonsterSpawnerComponentList* monster_spawner_component_list = NULL;
AgentSpawnerComponentList* agent_spawner_component_list = NULL;

DimensionComponentList* dimension_component_list = NULL;

TargetingComponentList* targeting_component_list = NULL;

#if DC_SERVER
ExplosionComponentList* explosion_component_list = NULL;
#endif

/* ComponentList handler switches */

Component* get_switch(ComponentType type)
{
    switch (type)
    {
        case COMPONENT_POSITION:
            return position_physics_component_list->subscribe();
        case COMPONENT_POSITION_MOMENTUM:
            return position_momentum_physics_component_list->subscribe();
        case COMPONENT_POSITION_CHANGED:
            return position_changed_physics_component_list->subscribe();
        case COMPONENT_POSITION_MOMENTUM_CHANGED:
            return position_momentum_changed_physics_component_list->subscribe();
        case COMPONENT_VERLET:
            return verlet_physics_component_list->subscribe();
            
        case COMPONENT_STACKABLE:
            return stackable_component_list->subscribe();

        #if DC_CLIENT
        case COMPONENT_BILLBOARD_SPRITE:
            return billboard_sprite_component_list->subscribe();
        case COMPONENT_COLORED_VOXEL:
            return colored_voxel_component_list->subscribe();
        case COMPONENT_TEXTURED_VOXEL:
            return textured_voxel_component_list->subscribe();
        #endif

        case COMPONENT_PICKUP:
            return pickup_component_list->subscribe();

        case COMPONENT_TTL:
            return ttl_health_component_list->subscribe();
        case COMPONENT_HIT_POINTS:
            return hit_points_health_component_list->subscribe();

        case COMPONENT_TEAM:
            return team_component_list->subscribe();
        case COMPONENT_INDEXED_TEAM:
            return indexed_team_component_list->subscribe();

        case COMPONENT_OWNER:
            return owner_component_list->subscribe();

        case COMPONENT_VOXEL_MODEL:
            return voxel_model_component_list->subscribe();

        case COMPONENT_MONSTER_SPAWNER:
            return monster_spawner_component_list->subscribe();
        case COMPONENT_AGENT_SPAWNER:
            return agent_spawner_component_list->subscribe();

        case COMPONENT_DIMENSION:
            return dimension_component_list->subscribe();

        case COMPONENT_TARGETING:
            return targeting_component_list->subscribe();

        #if DC_SERVER
        case COMPONENT_EXPLOSION:
            return explosion_component_list->subscribe();
        #endif
        
        default:
            printf("ERROR: Component::get() -- unknown ComponentType %d\n", type);
            return NULL;
    }
    return NULL;
}

void release_switch(Component* component)
{
    switch (component->type)
    {
        case COMPONENT_POSITION:
            position_physics_component_list->unsubscribe((PositionPhysicsComponent*)component);
            break;
        case COMPONENT_POSITION_MOMENTUM:
            position_momentum_physics_component_list->unsubscribe((PositionMomentumPhysicsComponent*)component);
            break;
        case COMPONENT_POSITION_CHANGED:
            position_changed_physics_component_list->unsubscribe((PositionChangedPhysicsComponent*)component);
            break;
        case COMPONENT_POSITION_MOMENTUM_CHANGED:
            position_momentum_changed_physics_component_list->unsubscribe((PositionMomentumChangedPhysicsComponent*)component);
            break;
        case COMPONENT_VERLET:
            verlet_physics_component_list->unsubscribe((VerletPhysicsComponent*)component);
            break;
            
        case COMPONENT_STACKABLE:
            stackable_component_list->unsubscribe((StackableComponent*)component);
            break;

        #if DC_CLIENT
        case COMPONENT_BILLBOARD_SPRITE:
            billboard_sprite_component_list->unsubscribe((BillboardSpriteComponent*)component);
            break;
        case COMPONENT_COLORED_VOXEL:
            colored_voxel_component_list->unsubscribe((ColoredVoxelComponent*)component);
            break;
        case COMPONENT_TEXTURED_VOXEL:
            textured_voxel_component_list->unsubscribe((TexturedVoxelComponent*)component);
            break;
        #endif

        case COMPONENT_PICKUP:
            pickup_component_list->unsubscribe((PickupComponent*)component);
            break;

        case COMPONENT_TTL:
            ttl_health_component_list->unsubscribe((TTLHealthComponent*)component);
            break;
        case COMPONENT_HIT_POINTS:
            hit_points_health_component_list->unsubscribe((HitPointsHealthComponent*)component);
            break;
            
        case COMPONENT_TEAM:
            team_component_list->unsubscribe((TeamComponent*)component);
            break;
        case COMPONENT_INDEXED_TEAM:
            indexed_team_component_list->unsubscribe((IndexedTeamComponent*)component);
            break;

        case COMPONENT_OWNER:
            owner_component_list->unsubscribe((OwnerComponent*)component);
            break;

        case COMPONENT_VOXEL_MODEL:
            voxel_model_component_list->unsubscribe((VoxelModelComponent*)component);
            break;

        case COMPONENT_MONSTER_SPAWNER:
            monster_spawner_component_list->unsubscribe((MonsterSpawnerComponent*)component);
        case COMPONENT_AGENT_SPAWNER:
            agent_spawner_component_list->unsubscribe((AgentSpawnerComponent*)component);
            break;

        case COMPONENT_DIMENSION:
            dimension_component_list->unsubscribe((DimensionComponent*)component);
            break;

        case COMPONENT_TARGETING:
            targeting_component_list->unsubscribe((TargetingComponent*)component);
            break;

        #if DC_SERVER
        case COMPONENT_EXPLOSION:
            explosion_component_list->unsubscribe((ExplosionComponent*)component);
            break;
        #endif

        default:
            printf("ERROR: Component::get() -- unknown ComponentType %d\n", component->type);
            break;
    }
}

void init()
{
    position_physics_component_list = new PositionPhysicsComponentList;
    position_momentum_physics_component_list = new PositionMomentumPhysicsComponentList;
    position_changed_physics_component_list = new PositionChangedPhysicsComponentList;
    position_momentum_changed_physics_component_list = new PositionMomentumChangedPhysicsComponentList;
    verlet_physics_component_list = new VerletPhysicsComponentList;

    stackable_component_list = new StackableComponentList;

    #if DC_CLIENT
    billboard_sprite_component_list = new BillboardSpriteComponentList;
    colored_voxel_component_list = new ColoredVoxelComponentList;
    textured_voxel_component_list = new TexturedVoxelComponentList;
    #endif
    
    pickup_component_list = new PickupComponentList;

    ttl_health_component_list = new TTLHealthComponentList;
    hit_points_health_component_list = new HitPointsHealthComponentList;

    team_component_list = new TeamComponentList;
    indexed_team_component_list = new IndexedTeamComponentList;

    owner_component_list = new OwnerComponentList;

    voxel_model_component_list = new VoxelModelComponentList;

    monster_spawner_component_list = new MonsterSpawnerComponentList;
    agent_spawner_component_list = new AgentSpawnerComponentList;

    dimension_component_list = new DimensionComponentList;

    targeting_component_list = new TargetingComponentList;

    #if DC_SERVER
    explosion_component_list = new ExplosionComponentList;
    #endif
}

void teardown()
{
    if (position_physics_component_list != NULL) delete position_physics_component_list;
    if (position_momentum_physics_component_list != NULL) delete position_momentum_physics_component_list;
    if (position_changed_physics_component_list != NULL) delete position_changed_physics_component_list;
    if (position_momentum_changed_physics_component_list != NULL) delete position_momentum_changed_physics_component_list;
    if (verlet_physics_component_list != NULL) delete  verlet_physics_component_list;
    
    if (stackable_component_list != NULL) delete stackable_component_list;

    #if DC_CLIENT
    if (billboard_sprite_component_list != NULL) delete billboard_sprite_component_list;
    if (colored_voxel_component_list != NULL) delete colored_voxel_component_list;
    if (textured_voxel_component_list != NULL) delete textured_voxel_component_list;
    #endif

    if (pickup_component_list != NULL) delete pickup_component_list;

    if (ttl_health_component_list != NULL) delete ttl_health_component_list;
    if (hit_points_health_component_list != NULL) delete hit_points_health_component_list;

    if (team_component_list != NULL) delete team_component_list;
    if (indexed_team_component_list != NULL) delete indexed_team_component_list;

    if (owner_component_list != NULL) delete owner_component_list;

    if (voxel_model_component_list != NULL) delete voxel_model_component_list;

    if (monster_spawner_component_list != NULL) delete monster_spawner_component_list;
    if (agent_spawner_component_list != NULL) delete agent_spawner_component_list;

    if (dimension_component_list != NULL) delete dimension_component_list;

    if (targeting_component_list != NULL) delete targeting_component_list;

    #if DC_SERVER
    if (explosion_component_list != NULL) delete explosion_component_list;
    #endif
}

} // Components
