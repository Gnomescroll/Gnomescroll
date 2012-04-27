#include "main.hpp"

#include <entity/components.hpp>
#include <entity/object/helpers.hpp>
#include <entity/object/filter.hpp>

namespace Objects
{

using Components::Component;

ObjectList* object_list = NULL;
ObjectDataList* object_data = NULL;
ObjectListFilter* filter = NULL;

void load_object_data()
{
    // refills
    load_pickup_sprite_data(OBJECT_HEALTH_REFILL);
    load_pickup_sprite_data(OBJECT_LASER_REFILL);
    load_pickup_sprite_data(OBJECT_GRENADE_REFILL);
    // gemstones

/*
    load_pickup_sprite_data(OBJECT_GEMSTONE_MALACHITE);
    load_pickup_sprite_data(OBJECT_GEMSTONE_RUBY);
    load_pickup_sprite_data(OBJECT_GEMSTONE_TURQUOISE);
    load_pickup_sprite_data(OBJECT_GEMSTONE_SILVER);
    load_pickup_sprite_data(OBJECT_GEMSTONE_AMETHYST);
    load_pickup_sprite_data(OBJECT_GEMSTONE_JADE);
    load_pickup_sprite_data(OBJECT_GEMSTONE_ONYX);

    // block drops
    load_dirt_block_drop_data();
    load_stone_block_drop_data();
    load_soft_rock_block_drop_data();
    load_medium_rock_block_drop_data();
    load_hard_rock_block_drop_data();
    load_infected_rock_block_drop_data();
*/
    // fabs
    load_agent_spawner_data();
    load_turret_data();

    // mobs
    load_mob_spawner_data();
    load_mob_robot_box_data();
    load_mob_bomb_data();
}

void init()
{   // must specify maximum values for objects here

    filter = new ObjectListFilter;
    filter->init();
    
    object_list = new ObjectList;
    object_list->init();
    
    // refills
    object_list->set_object_max(OBJECT_HEALTH_REFILL, 256);
    object_list->set_object_max(OBJECT_LASER_REFILL, 256);
    object_list->set_object_max(OBJECT_GRENADE_REFILL, 256);

/*
    // block drops
    object_list->set_object_max(OBJECT_DIRT_BLOCK_DROP, 256);
    object_list->set_object_max(OBJECT_STONE_BLOCK_DROP, 256);
    object_list->set_object_max(OBJECT_SOFT_ROCK_BLOCK_DROP, 256);
    object_list->set_object_max(OBJECT_MEDIUM_ROCK_BLOCK_DROP, 256);
    object_list->set_object_max(OBJECT_HARD_ROCK_BLOCK_DROP, 256);
    object_list->set_object_max(OBJECT_INFECTED_ROCK_BLOCK_DROP, 256);

    // gemstones
    object_list->set_object_max(OBJECT_GEMSTONE_MALACHITE, 256);
    object_list->set_object_max(OBJECT_GEMSTONE_RUBY, 256);
    object_list->set_object_max(OBJECT_GEMSTONE_TURQUOISE, 256);
    object_list->set_object_max(OBJECT_GEMSTONE_SILVER, 256);
    object_list->set_object_max(OBJECT_GEMSTONE_AMETHYST, 256);
    object_list->set_object_max(OBJECT_GEMSTONE_JADE, 256);
    object_list->set_object_max(OBJECT_GEMSTONE_ONYX, 256);
*/
    // fabs
    object_list->set_object_max(OBJECT_AGENT_SPAWNER, 256);
    object_list->set_object_max(OBJECT_TURRET, 512);

    // mobs
    object_list->set_object_max(OBJECT_MONSTER_SPAWNER, 64);
    object_list->set_object_max(OBJECT_MONSTER_BOX, 1024);
    object_list->set_object_max(OBJECT_MONSTER_BOMB, 512);

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
    switch (type)
    {
        // refills
        case OBJECT_HEALTH_REFILL:
        case OBJECT_LASER_REFILL:
        case OBJECT_GRENADE_REFILL:
        // gemstones
        case OBJECT_GEMSTONE_MALACHITE:
        case OBJECT_GEMSTONE_RUBY:
        case OBJECT_GEMSTONE_TURQUOISE:
        case OBJECT_GEMSTONE_SILVER:
        case OBJECT_GEMSTONE_AMETHYST:
        case OBJECT_GEMSTONE_JADE:
        case OBJECT_GEMSTONE_ONYX:
            return create_pickup_sprite(type);
/*
        // block drops
        case OBJECT_DIRT_BLOCK_DROP:
            return create_dirt_block_drop();
        case OBJECT_STONE_BLOCK_DROP:
            return create_stone_block_drop();
        case OBJECT_SOFT_ROCK_BLOCK_DROP:
            return create_soft_rock_block_drop();
        case OBJECT_MEDIUM_ROCK_BLOCK_DROP:
            return create_medium_rock_block_drop();
        case OBJECT_HARD_ROCK_BLOCK_DROP:
            return create_hard_rock_block_drop();
        case OBJECT_INFECTED_ROCK_BLOCK_DROP:
            return create_infected_rock_block_drop();
*/
        // fabs
        case OBJECT_AGENT_SPAWNER:
            return create_agent_spawner();
        case OBJECT_TURRET:
            return create_turret();

        // mobs
        case OBJECT_MONSTER_SPAWNER:
            return create_mob_spawner();
        case OBJECT_MONSTER_BOX:
            return create_mob_robot_box();
        case OBJECT_MONSTER_BOMB:
            return create_mob_bomb();
        
        default:
            printf("WARNING: creating unknown object type %d\n", type);
            return NULL;
    }
    return NULL;
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
    Object* object = create_switch(type);
    if (object == NULL) return NULL;
    object_list->set_object_id(object, id);
    return object;
}

void ready_switch(Object* object)
{
    switch (object->type)
    {
        // refills
        case OBJECT_HEALTH_REFILL:
        case OBJECT_LASER_REFILL:
        case OBJECT_GRENADE_REFILL:
        // gemstones
        case OBJECT_GEMSTONE_MALACHITE:
        case OBJECT_GEMSTONE_RUBY:
        case OBJECT_GEMSTONE_TURQUOISE:
        case OBJECT_GEMSTONE_SILVER:
        case OBJECT_GEMSTONE_AMETHYST:
        case OBJECT_GEMSTONE_JADE:
        case OBJECT_GEMSTONE_ONYX:
            ready_pickup_sprite(object);
            break;
/*
        // block drops
        case OBJECT_DIRT_BLOCK_DROP:
            ready_dirt_block_drop(object);
            break;
        case OBJECT_STONE_BLOCK_DROP:
            ready_stone_block_drop(object);
            break;
        case OBJECT_SOFT_ROCK_BLOCK_DROP:
            ready_soft_rock_block_drop(object);
            break;
        case OBJECT_MEDIUM_ROCK_BLOCK_DROP:
            ready_medium_rock_block_drop(object);
            break;
        case OBJECT_HARD_ROCK_BLOCK_DROP:
            ready_hard_rock_block_drop(object);
            break;
        case OBJECT_INFECTED_ROCK_BLOCK_DROP:
            ready_infected_rock_block_drop(object);
            break;
*/
        // fabs
        case OBJECT_AGENT_SPAWNER:
            ready_agent_spawner(object);
            break;
        case OBJECT_TURRET:
            ready_turret(object);
            break;

        // mobs
        case OBJECT_MONSTER_SPAWNER:
            ready_mob_spawner(object);
            break;
        case OBJECT_MONSTER_BOX:
            ready_mob_robot_box(object);
            break;
        case OBJECT_MONSTER_BOMB:
            ready_mob_bomb(object);
            break;

        default:
            printf("WARNING: birthing unknown object type %d\n", object->type);
            break;
    }
}

void destroy_switch(Object* object)
{
    ObjectType type = object->type;
    switch (type)
    {
        // refills
        case OBJECT_HEALTH_REFILL:
        case OBJECT_LASER_REFILL:
        case OBJECT_GRENADE_REFILL:
        // gemstones
        case OBJECT_GEMSTONE_MALACHITE:
        case OBJECT_GEMSTONE_RUBY:
        case OBJECT_GEMSTONE_TURQUOISE:
        case OBJECT_GEMSTONE_SILVER:
        case OBJECT_GEMSTONE_AMETHYST:
        case OBJECT_GEMSTONE_JADE:
        case OBJECT_GEMSTONE_ONYX:
            die_pickup_sprite(object);
            break;
/*
        // block drops
        case OBJECT_DIRT_BLOCK_DROP:
            die_dirt_block_drop(object);
            break;
        case OBJECT_STONE_BLOCK_DROP:
            die_stone_block_drop(object);
            break;
        case OBJECT_SOFT_ROCK_BLOCK_DROP:
            die_soft_rock_block_drop(object);
            break;
        case OBJECT_MEDIUM_ROCK_BLOCK_DROP:
            die_medium_rock_block_drop(object);
            break;
        case OBJECT_HARD_ROCK_BLOCK_DROP:
            die_hard_rock_block_drop(object);
            break;
        case OBJECT_INFECTED_ROCK_BLOCK_DROP:
            die_infected_rock_block_drop(object);
            break;
*/
        // fabs
        case OBJECT_AGENT_SPAWNER:
            die_agent_spawner(object);
            break;
        case OBJECT_TURRET:
            die_turret(object);
            break;

        // mobs
        case OBJECT_MONSTER_SPAWNER:
            die_mob_spawner(object);
            break;
        case OBJECT_MONSTER_BOX:
            die_mob_robot_box(object);
            break;
        case OBJECT_MONSTER_BOMB:
            die_mob_bomb(object);
            break;

        default:
            printf("WARNING: destroying unknown object type %d\n", type);
            break;
    }
    release_object_components(object);
    int id = object->id;
    object_list->destroy(type, id);
}

void destroy_switch(ObjectType type, int id)
{
    Object* object = get_object(type, id);
    if (object != NULL)
        destroy_switch(object);
}

Object* get_object(ObjectType type, int id)
{
    return object_list->get(type, id);
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
    assert(objects != NULL);
    char* used = object_list->get_used(type);
    assert(used != NULL);
    int max = object_list->max(type);
    assert(max > 0);
    
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
        assert(physics != NULL);
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

void damage_team_objects_within_sphere(const ObjectType* types, int n_types, Vec3 position, float radius, int damage, int inflictor_team, int inflictor_id)
{
    Object* object;

    using Components::HealthComponent;
    HealthComponent* health;

    using Components::TeamComponent;
    TeamComponent* team;
    int team_id;
    
    using Components::OwnerComponent;
    OwnerComponent* owner;    
    int owner_id;
    
    int count = filter->within_sphere(object_list, types, n_types, position, radius);
    for (int i=0; i<count; i++)
    {
        object = filter->objects[i];

        health = (HealthComponent*)object->get_component_interface(COMPONENT_INTERFACE_HEALTH);
        if (health == NULL) continue;

        team = (TeamComponent*)object->get_component_interface(COMPONENT_INTERFACE_TEAM);
        if (team != NULL) team_id = team->get_team();
        else team_id = NO_TEAM;

        owner = (OwnerComponent*)object->get_component_interface(COMPONENT_INTERFACE_OWNER);
        if (owner != NULL) owner_id = owner->get_owner();
        else owner_id = NO_AGENT;
        
        if ((team_id == inflictor_team && owner_id != NO_AGENT)
            && owner_id != inflictor_id)
            continue;

        health->take_damage(damage);
    }
}

void spawn_mobs()
{   // fill all monster spawner capacity
    const ObjectType type = OBJECT_MONSTER_SPAWNER;
    const ObjectType spawn_type = OBJECT_MONSTER_BOX;
    if (object_list->empty(type)) return;
    Object** objects = object_list->get_objects(type);
    assert(objects != NULL);
    char* used = object_list->get_used(type);;\
    assert(used != NULL);
    int max = object_list->max(type);
    assert(max > 0);
    Object* obj;
    Object* child;
    using Components::MonsterSpawnerComponent;
    MonsterSpawnerComponent* spawner;
    for (int i=0; i<max; i++)
    {
        if (!used[i]) continue;
        obj = objects[i];
        spawner = (MonsterSpawnerComponent*)obj->get_component(COMPONENT_MONSTER_SPAWNER);
        assert(spawner != NULL);
        child = spawner->spawn_child(spawn_type);
        if (child != NULL) Objects::ready(child);
    }
}

} // Objects
