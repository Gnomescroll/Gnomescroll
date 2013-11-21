/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "_interface.hpp"

#include <entity/components.hpp>
#include <entity/entity/filter.hpp>
#include <entity/entity/config.hpp>

namespace Entities
{

using Components::Component;

EntityList* entity_list = NULL;
EntityDataList* entity_data = NULL;
EntityListFilter* filter = NULL;

void load_entity_data()
{
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {
        entityLoad load = get_entity_load_method((EntityType)i);
        if (load != NULL) load();
    }
}

void init()
{   // must specify maximum values for entities here
    init_config();

    GS_ASSERT(entity_list == NULL);
    GS_ASSERT(entity_data == NULL);
    GS_ASSERT(filter == NULL);

    filter = new EntityListFilter;
    filter->init();

    entity_list = new EntityList;
    entity_list->init();

    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {
        int max = get_entity_max((EntityType)i);
        if (max > 0)
            entity_list->set_entity_max((EntityType)i, max);
    }

    entity_data = new EntityDataList;
    entity_data->init();
    load_entity_data();
    entity_list->init_entities(entity_data);
}

void teardown()
{
    delete filter;
    delete entity_list;
    delete entity_data;
    teardown_config();
}

/* Iterators */

void tick()
{
    entity_list->tick();
}

void update()
{
    entity_list->update();
}

void harvest()
{
    entity_list->harvest();
}

/* Underlying API handlers */

Entity* create(EntityType type)
{
    return entity_list->create(type);
}

Entity* create(EntityType type, EntityID id)
{
    return entity_list->create(type, id);
}

void ready(Entity* entity)
{
    IF_ASSERT(entity == NULL) return;
    entity->ready();
    #if DC_SERVER
    if (entity_is_networked(entity->type))
        entity->broadcastCreate();
    #endif
}

void destroy(Entity* entity)
{
    IF_ASSERT(entity == NULL) return;
    entity->destroy();
    #if DC_SERVER
    if (entity_is_networked(entity->type))
        entity->broadcastDeath();
    #endif
    entity_list->destroy(entity->type, entity->id);
}

void destroy(EntityType type, EntityID id)
{
    Entity* entity = get(type, id);
    if (entity != NULL)
        destroy(entity);
}

Entity* get(EntityType type, EntityID id)
{
    return entity_list->get(type, id);
}

class Entity* get_all(EntityType type, int& max, int& count)
{
    max = entity_list->max(type);
    count = entity_list->count(type);
    return entity_list->get_entities(type);
}

void destroy_all()
{
    entity_list->destroy_all();
}

int count(EntityType type)
{
    return entity_list->count(type);
}

bool full(EntityType type)
{
    return entity_list->full(type);
}

size_t get_components_needed(ComponentType type)
{
    size_t ct = 0;
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {
        size_t uses = entity_data->get_components_needed(EntityType(i), type);
        ct += get_entity_max(EntityType(i)) * uses;
    }
    return ct;
}

Components::Component* get_component_interface_reference(
    EntityType type, ComponentInterfaceType interface)
{
    return entity_data->get_component_interface_reference(type, interface);
}

bool point_occupied_by_type(EntityType type, const Vec3i& position)
{
    if (entity_list->empty(type)) return false;

    Entity* entities = entity_list->get_entities(type);
    IF_ASSERT(entities == NULL) return false;
    int max = entity_list->max(type);
    int count = entity_list->count(type);
    GS_ASSERT(max > 0);
    for (int i=0, j=0; i<max && j<count; i++)
    {
        if (entities[i].id == entity_list->null_id)
            continue;
        j++;
        Entity* obj = &entities[i];

        auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
        IF_ASSERT(physics == NULL) continue;
        Vec3i p = vec3i_init(physics->get_position());
        if (p.x != position.x || p.y != position.y) continue;
        auto dims = GET_COMPONENT_INTERFACE(Dimension, obj);
        int height = 1;
        if (dims != NULL)
            height = dims->get_integer_height();

        for (int k=0; k<height; k++)
            if (p.z + k == position.z)
                return true;
    }
    return false;
}

void damage_entities_within_sphere(const EntityType* types, int n_types,
                                   const Vec3& position, float radius,
                                   int damage, AgentID attacker_id)
{
    IF_ASSERT(damage < 0) return;
    if (!damage) return;
    int count = filter->within_sphere(entity_list, types, n_types, position, radius);
    for (int i=0; i<count; i++)
    {
        Entity* entity = filter->entities[i];
        auto health = GET_COMPONENT_INTERFACE(Health, entity);
        if (health != NULL)
            health->take_damage(damage);

        auto state_machine = GET_COMPONENT_INTERFACE(StateMachine, entity);
        if (state_machine != NULL)
        {
            AgentID* attacker = (AgentID*)malloc(sizeof(attacker_id));
            *attacker = attacker_id;
            state_machine->receive_event("agent_attacked", (void*)attacker);
        }
    }
}

void spawn_mobs()
{   // fill all monster spawner capacity
    const EntityType type = ENTITY_MONSTER_SPAWNER;
    if (entity_list->empty(type)) return;
    Entity* entities = entity_list->get_entities(type);
    IF_ASSERT(entities == NULL) return;
    int max = entity_list->max(type);
    GS_ASSERT(max > 0);
    int count = entity_list->count(type);
    for (int i=0, j=0; i<max && j<count; i++)
    {
        if (entities[i].id == entity_list->null_id)
            continue;
        j++;
        Entity* obj = &entities[i];
        auto spawner = GET_COMPONENT(MonsterSpawner, obj);
        IF_ASSERT(spawner == NULL) continue;
        Entity* child = spawner->spawn_child();
        if (child != NULL) ready(child);
    }
}

#if DC_SERVER
void send_to_client(ClientID client_id)
{
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
        if (entity_is_networked(EntityType(i)))
            entity_list->send_to_client(EntityType(i), client_id);

}
#endif

} // Entities
