#include "main.hpp"

#include <entity/components.hpp>
#include <entity/object/helpers.hpp>
#include <entity/object/filter.hpp>
#include <entity/object/types.hpp>
#include <entity/object/config.hpp>

namespace Objects
{

using Components::Component;

ObjectList* object_list = NULL;
ObjectDataList* object_data = NULL;
ObjectListFilter* filter = NULL;

void load_object_data()
{
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
    {
        objectLoad load = get_object_load_method((ObjectType)i);
        if (load != NULL) load();
    }
}

void init()
{   // must specify maximum values for objects here

    init_config();

    GS_ASSERT(object_list == NULL);
    GS_ASSERT(object_data == NULL);
    GS_ASSERT(filter == NULL);

    filter = new ObjectListFilter;
    filter->init();
    
    object_list = new ObjectList;
    object_list->init();
    
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
    {
        int max = get_object_max((ObjectType)i);
        if (max > 0)
            object_list->set_object_max((ObjectType)i, max);
    }
    
    object_data = new ObjectDataList;
    object_data->init();
    load_object_data();
    object_data->sanity_check();

    object_list->load_object_data(object_data);
}

void teardown()
{
    if (object_list != NULL) delete object_list;
    if (object_data != NULL) delete object_data;
    if (filter != NULL) delete filter;
    teardown_config();
}

/* Iterators */

void tick()
{
    object_list->tick();
}

void update()
{
    object_list->update();
}

void harvest()
{
    object_list->harvest();
}

/* Underlying API handlers */

static Object* create_switch(ObjectType type)
{
    objectCreate create = get_object_create_method(type);
    GS_ASSERT(create != NULL);
    if (create == NULL) return NULL;
    return create();
}

Object* create(ObjectType type)
{
    Object* object = create_switch(type);
    if (object == NULL) return NULL;
    object_list->set_object_id(object);
    return object;
}

Object* create(ObjectType type, int id)
{
    if (object_list->in_use(type, id)) return NULL;
    Object* object = create_switch(type);
    if (object == NULL) return NULL;
    object_list->set_object_id(object, id);
    return object;
}

void ready_switch(Object* object)
{
    GS_ASSERT(object != NULL);
    if (object == NULL) return;

    objectReady ready = get_object_ready_method(object->type);
    GS_ASSERT(ready != NULL);
    if (ready != NULL) ready(object);
}

void destroy_switch(Object* object)
{
    GS_ASSERT(object != NULL);
    if (object == NULL) return;
    ObjectType type = object->type;
    
    objectDie die = get_object_die_method(type);
    GS_ASSERT(die != NULL);
    if (die != NULL) die(object);
    
    release_object_components(object);
    int id = object->id;
    object_list->destroy(type, id);
}

void destroy_switch(ObjectType type, int id)
{
    Object* object = get_object(type, id);
    if (object != NULL) destroy_switch(object);
}

Object* get_object(ObjectType type, int id)
{
    return object_list->get(type, id);
}

int count(ObjectType type)
{
    return object_list->count(type);
}

bool full(ObjectType type)
{
    return object_list->full(type);
}

void send_to_client(ObjectType type, int client_id)
{
    object_list->send_to_client(type, client_id);
}

bool point_occupied_by_type(ObjectType type, int x, int y, int z)
{
    if (object_list->empty(type)) return false;

    Object** objects = object_list->get_objects(type);
    GS_ASSERT(objects != NULL);
    char* used = object_list->get_used(type);
    GS_ASSERT(used != NULL);
    int max = object_list->max(type);
    GS_ASSERT(max > 0);
    
    using Components::PhysicsComponent;
    using Components::DimensionComponent;

    Object *obj;
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

void damage_objects_within_sphere(const ObjectType* types, int n_types, Vec3 position, float radius, int damage)
{
    Object* object;

    using Components::HealthComponent;
    HealthComponent* health;

    int count = filter->within_sphere(object_list, types, n_types, position, radius);
    for (int i=0; i<count; i++)
    {
        object = filter->objects[i];

        health = (HealthComponent*)object->get_component_interface(COMPONENT_INTERFACE_HEALTH);
        if (health != NULL) health->take_damage(damage);
    }
}

void spawn_mobs()
{   // fill all monster spawner capacity
    const ObjectType type = OBJECT_MONSTER_SPAWNER;
    if (object_list->empty(type)) return;
    Object** objects = object_list->get_objects(type);
    GS_ASSERT(objects != NULL);
    char* used = object_list->get_used(type);
    GS_ASSERT(used != NULL);
    int max = object_list->max(type);
    GS_ASSERT(max > 0);
    Object* obj;
    Object* child;
    using Components::MonsterSpawnerComponent;
    MonsterSpawnerComponent* spawner;
    for (int i=0; i<max; i++)
    {
        if (!used[i]) continue;
        obj = objects[i];
        spawner = (MonsterSpawnerComponent*)obj->get_component(COMPONENT_MONSTER_SPAWNER);
        GS_ASSERT(spawner != NULL);
        child = spawner->spawn_child();
        if (child != NULL) Objects::ready(child);
    }
}

#if DC_SERVER
void send_object_state_machines(const ObjectType type, const int client_id)
{
    GS_ASSERT(type == OBJECT_MONSTER_BOMB);
    if (type != OBJECT_MONSTER_BOMB) return;    // TODO
    
    if (object_list->empty(type)) return;

    Object** objects = object_list->get_objects(type);
    GS_ASSERT(objects != NULL);
    char* used = object_list->get_used(type);
    GS_ASSERT(used != NULL);
    int max = object_list->max(type);
    GS_ASSERT(max > 0);

    for (int i=0; i<max; i++)
        if (used[i])
            send_mob_bomb_state_machine_to_client(client_id, objects[i]);
}
#endif

} // Objects
