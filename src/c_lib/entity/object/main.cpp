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
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
    {
        entityLoad load = get_object_load_method((EntityType)i);
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

    for (int i=0; i<MAX_OBJECT_TYPES; i++)
    {
        int max = get_object_max((EntityType)i);
        if (max > 0)
            entity_list->set_object_max((EntityType)i, max);
    }

    entity_data = new EntityDataList;
    entity_data->init();
    load_object_data();
    entity_data->sanity_check();

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

static Entity* create_switch (EntityType type)
{
    entityCreate create = get_object_create_method(type);
    GS_ASSERT(create != NULL);
    if (create == NULL) return NULL;
    return create();
}

Entity* create(EntityType type)
{
    Entity* object = create_switch (type);
    if (object == NULL) return NULL;
    entity_list->set_object_id(object);
    return object;
}

Entity* create(EntityType type, int id)
{
    if (entity_list->in_use(type, id)) return NULL;
    Entity* object = create_switch (type);
    if (object == NULL) return NULL;
    entity_list->set_object_id(object, id);
    return object;
}

void ready_switch (Entity* object)
{
    GS_ASSERT(object != NULL);
    if (object == NULL) return;

    entityReady ready = get_object_ready_method(object->type);
    GS_ASSERT(ready != NULL);
    if (ready != NULL) ready(object);
}

void destroy_switch (Entity* object)
{
    GS_ASSERT(object != NULL);
    if (object == NULL) return;
    EntityType type = object->type;

    entityDie die = get_object_die_method(type);
    GS_ASSERT(die != NULL);
    if (die != NULL) die(object);

    release_object_components(object);
    int id = object->id;
    entity_list->destroy(type, id);
}

void destroy_switch (EntityType type, int id)
{
    Entity* object = get_object(type, id);
    if (object != NULL) destroy_switch (object);
}

Entity* get_object(EntityType type, int id)
{
    return entity_list->get(type, id);
}

int get_all(const EntityType type, class Entity**& entities, char*& used)
{
    used = entity_list->get_used(type);
    entities = entity_list->get_objects(type);
    return entity_list->max(type);
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

bool point_occupied_by_type(EntityType type, int x, int y, int z)
{
    if (entity_list->empty(type)) return false;

    Entity** objects = entity_list->get_objects(type);
    GS_ASSERT(objects != NULL);
    char* used = entity_list->get_used(type);
    GS_ASSERT(used != NULL);
    int max = entity_list->max(type);
    GS_ASSERT(max > 0);

    using Components::PhysicsComponent;
    using Components::DimensionComponent;

    Entity *obj;
    int px,py,pz,height;
    PhysicsComponent* physics;
    DimensionComponent* dims;

    for (int i=0; i<max; i++)
    {
        if (!used[i]) continue;
        obj = objects[i];

        physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        GS_ASSERT(physics != NULL);
        Vec3 position = physics->get_position();
        px = (int)position.x;
        py = (int)position.y;
        if (px != x || py != y) continue;
        pz = (int)position.z;

        dims = (DimensionComponent*)obj->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
        if (dims != NULL) height = dims->get_integer_height();
        else height = 1;

        for (int j=0; j<height; j++)
            if (pz + j == z) return true;
    }
    return false;
}

void damage_objects_within_sphere(const EntityType* types, int n_types, Vec3 position, float radius, int damage)
{
    int count = filter->within_sphere(entity_list, types, n_types, position, radius);
    for (int i=0; i<count; i++)
    {
        Entity* object = filter->objects[i];
        using Components::HealthComponent;
        HealthComponent* health = (HealthComponent*)object->get_component_interface(COMPONENT_INTERFACE_HEALTH);
        if (health != NULL) health->take_damage(damage);
    }
}

void spawn_mobs()
{   // fill all monster spawner capacity
    const EntityType type = OBJECT_MONSTER_SPAWNER;
    if (entity_list->empty(type)) return;
    Entity** objects = entity_list->get_objects(type);
    GS_ASSERT(objects != NULL);
    char* used = entity_list->get_used(type);
    GS_ASSERT(used != NULL);
    int max = entity_list->max(type);
    GS_ASSERT(max > 0);
    Entity* obj;
    Entity* child;
    using Components::MonsterSpawnerComponent;
    MonsterSpawnerComponent* spawner;
    for (int i=0; i<max; i++)
    {
        if (!used[i]) continue;
        obj = objects[i];
        spawner = (MonsterSpawnerComponent*)obj->get_component(COMPONENT_MONSTER_SPAWNER);
        GS_ASSERT(spawner != NULL);
        child = spawner->spawn_child();
        if (child != NULL) ready(child);
    }
}

#if DC_SERVER
void send_object_state_machines(const EntityType type, const ClientID client_id)
{
    GS_ASSERT(type == OBJECT_MONSTER_BOMB);
    if (type != OBJECT_MONSTER_BOMB) return;    // TODO

    if (entity_list->empty(type)) return;

    Entity** objects = entity_list->get_objects(type);
    GS_ASSERT(objects != NULL);
    char* used = entity_list->get_used(type);
    GS_ASSERT(used != NULL);
    int max = entity_list->max(type);
    GS_ASSERT(max > 0);

    for (int i=0; i<max; i++)
        if (used[i])
            send_mob_bomb_state_machine_to_client(client_id, objects[i]);
}

void send_to_client(ClientID client_id)
{
    // TODO -- make these one function call
    send_to_client(OBJECT_BASE, client_id);
    send_to_client(OBJECT_TURRET, client_id);
    send_to_client(OBJECT_AGENT_SPAWNER, client_id);
    send_to_client(OBJECT_ENERGY_CORE, client_id);
    send_to_client(OBJECT_MONSTER_BOMB, client_id);

    // DOESNT WORK RIGHT:
    send_object_state_machines(OBJECT_MONSTER_BOMB, client_id);
    send_to_client(OBJECT_MONSTER_BOX, client_id);
    send_to_client(OBJECT_MONSTER_SPAWNER, client_id);
}
#endif

} // Entities
