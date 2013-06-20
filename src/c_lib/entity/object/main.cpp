#include "main.hpp"

#include <entity/components.hpp>
#include <entity/object/helpers.hpp>
#include <entity/object/filter.hpp>
#include <entity/object/types.hpp>
#include <entity/object/config.hpp>

namespace Entities
{

using Components::Component;

EntityList* entity_list = NULL;
EntityDataList* entity_data = NULL;
EntityListFilter* filter = NULL;

void load_object_data()
{
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {
        entityLoad load = get_entity_load_method((EntityType)i);
        if (load != NULL) load();
    }
}

void init()
{   // must specify maximum values for objects here

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
            entity_list->set_object_max((EntityType)i, max);
    }

    entity_data = new EntityDataList;
    entity_data->init();
    load_object_data();

    entity_list->load_object_data(entity_data);
}

void teardown()
{
    delete entity_list;
    delete entity_data;
    delete filter;
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

static void load_entity_properties(Entity* entity)
{
    if (entity == NULL) return;
    size_t n_components = entity_data->get_component_count(entity->type);
    Components::Component const* const* components = entity_data->get_components(entity->type);
    IF_ASSERT(components == NULL) return;
    for (size_t i=0; i<n_components; i++)
    {
        const Components::Component* c = components[i];
        IF_ASSERT(c == NULL) continue;
        Components::Component* d = add_component_to_object(entity, c->type);
        d->load_settings_from(c);
    }
}

static Entity* _create(EntityType type)
{
    Entity* obj = entity_list->create(type);
    load_entity_properties(obj);
    return obj;
}

Entity* create(EntityType type)
{
    Entity* object = _create(type);
    if (object == NULL) return NULL;
    entity_list->set_object_id(object);
    return object;
}

Entity* create(EntityType type, EntityID id)
{
    if (entity_list->in_use(type, id)) return NULL;
    Entity* object = _create(type);
    if (object == NULL) return NULL;
    entity_list->set_object_id(object, id);
    return object;
}

void ready_switch(Entity* object)
{
    IF_ASSERT(object == NULL) return;
    entityReady ready = get_entity_ready_method(object->type);
    IF_ASSERT(ready == NULL) return;
    ready(object);
}

void destroy_switch(Entity* object)
{
    IF_ASSERT(object == NULL) return;
    EntityType type = object->type;

    entityDie die = get_entity_die_method(type);
    GS_ASSERT(die != NULL);
    if (die != NULL) die(object);

    release_object_components(object);
    EntityID id = object->id;
    entity_list->destroy(type, id);
}

void destroy_switch(EntityType type, EntityID id)
{
    Entity* object = get_object(type, id);
    if (object != NULL) destroy_switch(object);
}

Entity* get_object(EntityType type, EntityID id)
{
    return entity_list->get(type, id);
}

class Entity** get_all(EntityType type, char*& used, int& count)
{
    count = entity_list->max(type);
    used = entity_list->get_used(type);
    return entity_list->get_objects(type);
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

void send_to_client(EntityType type, ClientID client_id)
{
    entity_list->send_to_client(type, client_id);
}

bool point_occupied_by_type(EntityType type, const Vec3i& position)
{
    if (entity_list->empty(type)) return false;

    Entity** objects = entity_list->get_objects(type);
    GS_ASSERT(objects != NULL);
    char* used = entity_list->get_used(type);
    GS_ASSERT(used != NULL);
    int max = entity_list->max(type);
    GS_ASSERT(max > 0);

    Entity* obj;
    Vec3i p = vec3i_init(0);
    int height = 1;

    for (int i=0; i<max; i++)
    {
        if (!used[i]) continue;
        obj = objects[i];

        auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
        IF_ASSERT(physics == NULL) continue;
        p = vec3i_init(physics->get_position());
        if (p.x != position.x || p.y != position.y) continue;
        auto dims = GET_COMPONENT_INTERFACE(Dimension, obj);
        if (dims != NULL) height = dims->get_integer_height();
        else height = 1;

        for (int j=0; j<height; j++)
            if (p.z + j == position.z)
                return true;
    }
    return false;
}

void damage_objects_within_sphere(const EntityType* types, int n_types, Vec3 position, float radius, int damage)
{
    int count = filter->within_sphere(entity_list, types, n_types, position, radius);
    for (int i=0; i<count; i++)
    {
        Entity* object = filter->objects[i];
        auto health = GET_COMPONENT_INTERFACE(Health, object);
        if (health != NULL) health->take_damage(damage);
    }
}

void spawn_mobs()
{   // fill all monster spawner capacity
    const EntityType type = ENTITY_MONSTER_SPAWNER;
    if (entity_list->empty(type)) return;
    Entity** objects = entity_list->get_objects(type);
    GS_ASSERT(objects != NULL);
    char* used = entity_list->get_used(type);
    GS_ASSERT(used != NULL);
    int max = entity_list->max(type);
    GS_ASSERT(max > 0);
    Entity* obj;
    Entity* child;
    for (int i=0; i<max; i++)
    {
        if (!used[i]) continue;
        obj = objects[i];
        auto spawner = GET_COMPONENT(MonsterSpawner, obj);
        IF_ASSERT(spawner == NULL) continue;
        child = spawner->spawn_child();
        if (child != NULL) ready(child);
    }
}

#if DC_SERVER
void send_to_client(ClientID client_id)
{
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
        if (object_is_networked(EntityType(i)))
            send_to_client(EntityType(i), client_id);
}
#endif

} // Entities
