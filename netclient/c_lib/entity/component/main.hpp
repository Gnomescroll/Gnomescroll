#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/component/component.hpp>
#include <c_lib/entity/component/component_list.hpp>
#include <c_lib/entity/components/include.hpp>

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

/* ComponentList typedefs */

// TODO - move
 
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

/* Owner */
const int MAX_OWNER_COMPONENTS = 4096;
typedef ComponentList<OwnerComponent, COMPONENT_OWNER, MAX_OWNER_COMPONENTS>
    OwnerComponentList;

/* Voxel Model */
const int MAX_VOXEL_MODEL_COMPONENTS = 1024;
typedef ComponentList<VoxelModelComponent, COMPONENT_VOXEL_MODEL, MAX_VOXEL_MODEL_COMPONENTS>
    VoxelModelComponentList;

/* Targeting */
const int MAX_TARGETING_COMPONENTS = 2048;
typedef ComponentList<TargetingComponent, COMPONENT_TARGETING, MAX_TARGETING_COMPONENTS>
    TargetingComponentList;

/* Spawner */

// TODO - adapt correctly, then move
const int MAX_SPAWNER_COMPONENTS = 512;
typedef ComponentList<MonsterSpawnerComponent, OBJECT_MONSTER_SPAWNER, MAX_SPAWNER_COMPONENTS>
    MonsterSpawnerComponentList;
class AgentSpawnerComponentList: public ComponentList<AgentSpawnerComponent, COMPONENT_AGENT_SPAWNER, MAX_SPAWNER_COMPONENTS>
{
    bool SpawnerList::team_spawner_available(int team)
    {
        Object *s;
        int n = 0;
        for (int i=0; i<this->max; i++)
        {
            if (this->components[i] == NULL) continue;
            s = this->components[i]->obj;
            if (s == NULL) continue;
            if (s->get_team() == team) n++;
        }
        return (n < this->max_per_team);
    }

    int SpawnerList::get_random_spawner(int team)
    {
        Object *s;
        int objects[this->max_per_team+1];
        int j=0;
        for (int i=0; i<this->max; i++)
        {   // filter down to team's objects
            if (this->components[i] == NULL) continue;
            s = this->components[i]->obj;
            if (s == NULL) continue;
            if (s->get_team() == team)
                objects[j++] = s->get_team_index();
        }
        objects[j++] = BASE_SPAWN_ID;
        return objects[randrange(0,j-1)];
    }

    // when objects player says "spawner 8" he may be on the other team
    // we need to find the 8th spawner for his team
    int SpawnerList::get_numbered_team_spawner(int team, int id)
    {
        Object *s;
        for (int i=0; i<this->max; i++)
        {
            if (this->components[i] == NULL) continue;
            s = this->components[i]->obj;
            if (s == NULL) continue;
            if (s->get_team() != team) continue;
            if ((int)s->get_team_index() == id)
                return s->state()->id;
        }
        return BASE_SPAWN_ID;
    }

    Object* SpawnerList::get_by_team_index(int team, int team_index)
    {
        Object *s;
        for (int i=0; i<this->max; i++)
        {
            if (this->components[i] == NULL) continue;
            s = this->components[i]->obj;
            if (s == NULL) continue;
            if (s->get_team() != team) continue;
            if ((int)s->get_team_index() == team_index)
                return s;
        }
        return NULL;
    }

    bool SpawnerList::spawner_exists(int team, int team_index)
    {
        if (this->get_by_team_index(team, team_index) != NULL)
            return true;
        return false;
    }

    void SpawnerList::assign_team_index(Object* spawner)
    {   // pick an index for the spawner that is available, these are separate from
        // id because each team's set of objects has its own indexing
        // and objects may be destroyed; we dont want to renumber every time
        // get smallest available team index
        if (!this->max) return;
        int* taken = (int*)calloc(this->max, sizeof(int));
        int team_index = TEAM_INDEX_NONE;
        Object* s;
        for (int i=0; i<this->max; i++)
        {
            if (this->components[i] == NULL) continue;
            s = this->components[i]->obj;
            if (s == NULL) continue;
            if (s->get_team() != spawner->get_team()) continue;
            if (spawner->get_team_index() != TEAM_INDEX_NONE
              && spawner->get_team_index() != 0)  // should never be 0, team_indexing starts at 1
                taken[spawner->get_team_index() - 1] = 1;
        }
        for (int i=0; i<this->max; i++)
            if (!taken[i])
            {
                team_index = i+1;
                break;
            }
        spawner->set_team_index(team_index);
        free(taken);
    }
}

const int DIMENSION_COMPONENT_MAX = 4096;
typedef ComponentList<DimensionComponent, COMPONENT_DIMENSION, DIMENSION_COMPONENT_MAX>
    DimensionComponentList;

/* ComponentList declarations */

extern PositionPhysicsComponentList* position_physics_component_list;
extern PositionMomentumPhysicsComponentList* position_momentum_physics_component_list;
extern PositionChangedPhysicsComponentList* position_changed_physics_component_list;
extern PositionMomentumChangedPhysicsComponentList* position_momentum_changed_physics_component_list;
extern VerletPhysicsComponentList* verlet_physics_component_list;

extern StackableComponentList* stackable_component_list;

extern BillboardSpriteComponentList* billboard_sprite_component_list;
extern ColoredVoxelComponentList* colored_voxel_component_list;
extern TexturedVoxelComponentList* textured_voxel_component_list;

extern PickupComponentList* pickup_component_list;

extern TTLHealthComponentList* ttl_health_component_list;
extern HitPointsHealthComponentList* hit_points_health_component_list;

extern TeamComponentList* team_component_list;
extern IndexedTeamComponentList* indexed_team_component_list;

extern OwnerComponentList* owner_component_list;

extern VoxelModelComponentList* voxel_model_component_list;

extern MonsterSpawnerComponentList monster_spawner_component_list;
extern AgentSpawnerComponentList* agent_spawner_component_list;

extern DimensionComponentList* dimension_component_list;

extern TargetingComponentList* targeting_component_list;

/* ComponentList handler switches */

Component* get_switch(ComponentType type);
void release_switch(Component* component);

} // Components
