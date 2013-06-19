#include "objects.hpp"

#include <entity/object/main.hpp>

namespace Entities
{

Entity* get(EntityType type, EntityID id)
{
    return get_object(type, id);
}

void ready(Entity* object)
{
    ready_switch(object);
}

void destroy(Entity* object)
{
    destroy_switch(object);
}

void destroy(EntityType type, EntityID id)
{
    destroy_switch(type, id);
}

void stress_test()
{
    EntityType type = ENTITY_MONSTER_BOMB;
    const int iters = 1000;
    printf("Begin object stress test, maxing out %d %d times\n", type, iters);
    int ttl = 0;
    for (int i=0; i<iters; i++)
    {
        //printf("%d\n", i);
        Entity* obj = NULL;
        int n = 0;
        while ((obj = create(type)) != NULL)
        {
            n++;
            ready(obj);
        }
        //printf("Maxed out %d to %d\n", type, n);
        //printf("Destroy %d of %d\n", n, type);
        for (int j=0; j<n; j++)
            destroy(type, EntityID(j));
        //printf("Destroyed %d\n", n);
        ttl += n;
    }
    printf("Stress test done. Created and destroyed %d objects of type %d\n", iters*ttl, type);
}

static Vec3 get_default_spawn_position(Entity* object)
{
    int h = 1;
    auto dims = GET_COMPONENT_INTERFACE(Dimension, object);
    if (dims != NULL) h = dims->get_integer_height();
    Vec3i position = vec3i_init(0);
    int tries = 0;
    const int MAX_TRIES = 10;
    do
    {
        position.x = randrange(0, map_dim.x - 1);
        position.y = randrange(0, map_dim.y - 1);
        int z = randrange(1, map_dim.z-1);
        position.z = t_map::get_nearest_surface_block(position.x, position.y, z, h);
    } while (position.z <= 0 && tries < MAX_TRIES);
    if (tries == MAX_TRIES)
        position.z = t_map::get_highest_open_block(0, 0);
    return vec3_add(vec3_init(position), vec3_init(0.5f, 0.5f, 0.0f));
}

static Vec3 get_slime_spawn_position(Entity* slime)
{   // spawn the slime underground
    Vec3 position = get_default_spawn_position(slime);
    int z = position.z - 1;
    while (z >= 0 && !t_map::isSolid(position.x, position.y, z))
        z--;
    if (z < 0)
        return vec3_init(0);
    position.z = z;
    return position;
}

static void spawn_monsters(EntityType type, int n, Vec3 (*get_spawn_position)(Entity* object))
{
    int count = Entities::count(type);
    for (int i=0; i<n-count; i++)
    {
        Entities::Entity* obj = Entities::create(type);
        if (obj == NULL) break;
            auto physics = GET_COMPONENT_INTERFACE(Physics, obj);
        IF_ASSERT(physics == NULL)
        {
            Entities::destroy(obj);
            break;
        }
        Vec3 position = get_spawn_position(obj);
        physics->set_position(position);
        Entities::ready(obj);
    }
}

void spawn_monsters(EntityType type, int n)
{
    spawn_monsters(type, n, &get_default_spawn_position);
}

void spawn_slimes(int n)
{
    spawn_monsters(ENTITY_MONSTER_SLIME, n, &get_slime_spawn_position);
}

} // Entities
