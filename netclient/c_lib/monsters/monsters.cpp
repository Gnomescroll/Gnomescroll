#include "monsters.hpp"

#include <c_lib/monsters/constants.hpp>
#include <c_lib/monsters/slime.cpp>
#include <c_lib/monsters/box.cpp>
#include <c_lib/monsters/monster_spawner.cpp>


#include <math.h>
#include <c_lib/t_map/t_map.hpp>
namespace Monsters
{

#if DC_SERVER

// should just periodically fill each spawner to their max

void spawn_monsters(int max_monsters)
{
    // spawn monsters rnadomly distributed over monster spawners
    
    int current_monsters = STATE::object_list->get_object_count(OBJECT_MONSTER_BOX);
    int to_create = max_monsters - current_monsters;
    if (to_create <= 0) return;

    int n_spawners = STATE::object_list->get_object_count(OBJECT_MONSTER_SPAWNER);
    if (!n_spawners) return;

    int max_spawners = STATE::object_list->filter_active_objects(OBJECT_MONSTER_SPAWNER);
    if (!max_spawners) return;

    int ct = 0;
    Box* box;
    MonsterSpawner* spawner;
    float spawn_point[3];

    while (ct < to_create)
    {
        int added = 0;
        for (int i=0; i<max_spawners; i++)
        {
            if (ct+added >= to_create) break;
            spawner = (MonsterSpawner*)STATE::object_list->filtered_objects[i];
            if (spawner->spawn.full()) continue;
            box = (Box*)STATE::object_list->create(spawner->spawn.type);
            if (box == NULL) return; // abort; out of room
            spawner->spawn.get_spawn_point(spawner->get_position(), BOX_HEIGHT, spawn_point);
            spawner->spawn.gain_child(spawner->spawn.type, box->state()->id);
            box->set_position(spawn_point[0], spawn_point[1], spawn_point[2]);
            box->spawner = spawner->state()->id;
            box->born();// TODO
            added++;
        }
        if (added == 0) break; // all spawners full
        ct += added;
    }
    //printf("Created %d\n", ct);
}

void create_monsters_spawners(int max_spawners)
{
    int current_spawners = STATE::object_list->get_object_count(OBJECT_MONSTER_SPAWNER);
    int to_create = max_spawners - current_spawners;
    if (to_create <= 0) return;
    MonsterSpawner* obj;
    for (int i=0; i<to_create; i++)
    {
        obj = (MonsterSpawner*)STATE::object_list->create(OBJECT_MONSTER_SPAWNER);
        if (obj == NULL) break;
        int x = randrange(0,map_dim.x-1);
        int y = randrange(0,map_dim.y-1);
        int z = t_map::get_highest_open_block(x,y);
        obj->spawn.type = OBJECT_MONSTER_BOX;
        obj->set_position(x+0.5f,y+0.5f,z);
        obj->born();   // TODO
    }
}
#endif

} // Monsters
