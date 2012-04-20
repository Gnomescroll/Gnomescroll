#include "main.hpp"

#include <c_lib/entity/components.hpp>
#include <c_lib/entity/object/helpers.hpp>

namespace Objects
{

using Components::Component;

ObjectList* object_list = NULL;

void init()
{   // must specify maximum values for objects here

    object_list = new ObjectList;
    object_list->init();
    
    // refills
    object_list->set_object_max(OBJECT_HEALTH_REFILL, 256);
    object_list->set_object_max(OBJECT_LASER_REFILL, 256);
    object_list->set_object_max(OBJECT_GRENADE_REFILL, 256);

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
    
    // fabs
    object_list->set_object_max(OBJECT_AGENT_SPAWNER, 256);
    object_list->set_object_max(OBJECT_TURRET, 512);

    // mobs
    object_list->set_object_max(OBJECT_MONSTER_SPAWNER, 64);
    object_list->set_object_max(OBJECT_MONSTER_BOX, 1024);
    object_list->set_object_max(OBJECT_MONSTER_BOMB, 512);
}

void teardown()
{
    if (object_list != NULL) delete object_list;
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

Object* create_switch(ObjectType type)
{
    switch (type)
    {
        // refills
        case OBJECT_HEALTH_REFILL:
            return create_health_refill();
        case OBJECT_LASER_REFILL:
            return create_laser_refill();
        case OBJECT_GRENADE_REFILL:
            return create_grenade_refill();

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

        // gemstones
        case OBJECT_GEMSTONE_MALACHITE:
            return create_malachite_gemstone();
        case OBJECT_GEMSTONE_RUBY:
            return create_ruby_gemstone();
        case OBJECT_GEMSTONE_TURQUOISE:
            return create_turquoise_gemstone();
        case OBJECT_GEMSTONE_SILVER:
            return create_silver_gemstone();
        case OBJECT_GEMSTONE_AMETHYST:
            return create_amethyst_gemstone();
        case OBJECT_GEMSTONE_JADE:
            return create_jade_gemstone();
        case OBJECT_GEMSTONE_ONYX:
            return create_onyx_gemstone();

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

Object* create_switch(ObjectType type, int id)
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
            ready_health_refill(object);
            break;
        case OBJECT_LASER_REFILL:
            ready_laser_refill(object);
            break;
        case OBJECT_GRENADE_REFILL:
            ready_grenade_refill(object);
            break;

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

        // gemstones
        case OBJECT_GEMSTONE_MALACHITE:
            ready_malachite_gemstone(object);
            break;
        case OBJECT_GEMSTONE_RUBY:
            ready_ruby_gemstone(object);
            break;
        case OBJECT_GEMSTONE_TURQUOISE:
            ready_turquoise_gemstone(object);
            break;
        case OBJECT_GEMSTONE_SILVER:
            ready_silver_gemstone(object);
            break;
        case OBJECT_GEMSTONE_AMETHYST:
            ready_amethyst_gemstone(object);
            break;
        case OBJECT_GEMSTONE_JADE:
            ready_jade_gemstone(object);
            break;
        case OBJECT_GEMSTONE_ONYX:
            ready_onyx_gemstone(object);
            break;

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
            die_health_refill(object);
            break;
        case OBJECT_LASER_REFILL:
            die_laser_refill(object);
            break;
        case OBJECT_GRENADE_REFILL:
            die_grenade_refill(object);
            break;

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

        // gemstones
        case OBJECT_GEMSTONE_MALACHITE:
            die_malachite_gemstone(object);
            break;
        case OBJECT_GEMSTONE_RUBY:
            die_ruby_gemstone(object);
            break;
        case OBJECT_GEMSTONE_TURQUOISE:
            die_turquoise_gemstone(object);
            break;
        case OBJECT_GEMSTONE_SILVER:
            die_silver_gemstone(object);
            break;
        case OBJECT_GEMSTONE_AMETHYST:
            die_amethyst_gemstone(object);
            break;
        case OBJECT_GEMSTONE_JADE:
            die_jade_gemstone(object);
            break;
        case OBJECT_GEMSTONE_ONYX:
            die_onyx_gemstone(object);
            break;

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
    for (int i=0; i<object->n_components; i++)
        remove_component_from_object(object, object->components[i]);
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
        obj = objects[i];
        if (obj == NULL) continue;
        
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

} // Objects
