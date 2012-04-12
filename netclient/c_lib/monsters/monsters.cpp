#include "monsters.hpp"

#include <c_lib/monsters/constants.hpp>
#include <c_lib/monsters/slime.cpp>
#include <c_lib/monsters/box.cpp>
#include <c_lib/monsters/monster_spawner.cpp>

namespace Monsters
{

void spawn_monsters(int max_monsters)
{
    // spawn monsters rnadomly distributed over monster spawners
    
    int current_monsters = STATE::object_list->get_object_count(OBJ_TYPE_MONSTER_BOX);
    int to_create = max_monsters - current_monsters;
    if (to_create <= 0) return;

    int n_spawners = STATE::object_list->get_object_count(OBJ_TYPE_MONSTER_SPAWNER);
    if (!n_spawners) return;

    int max_spawners = STATE::object_list->filter_active_objects(OBJ_TYPE_MONSTER_SPAWNER);
    if (!max_spawners) return;

    int monsters_per_spawner = (to_create/n_spawners) + 1;

    Box* box;
    MonsterSpawner* spawner;
    float spawn_point[3];
    for (int j=0; j<monsters_per_spawner; j++)
        for (int i=0; i<max_spawners; i++)
        {
            spawner = (MonsterSpawner*)STATE::object_list->filtered_objects[i];
            box = (Box*)STATE::object_list->create(spawner->spawn.type);
            if (box == NULL) return; // abort; out of room
            spawner->spawn.get_spawn_point(spawner->get_position(), BOX_HEIGHT, spawn_point);
            box->set_position(spawn_point[0], spawn_point[1], spawn_point[2]);
            box->born();
        }
}

void create_monsters_spawners(int max_spawners)
{
    int current_spawners = STATE::object_list->get_object_count(OBJ_TYPE_MONSTER_SPAWNER);
    int to_create = max_spawners - current_spawners;
    if (to_create <= 0) return;
    MonsterSpawner* obj;
    for (int i=0; i<to_create; i++)
    {
        obj = (MonsterSpawner*)STATE::object_list->create(OBJ_TYPE_MONSTER_SPAWNER);
        if (obj == NULL) break;
        float x = randrange(0,map_dim.x-1) + randf();
        float y = randrange(0,map_dim.y-1) + randf();
        float z = t_map::get_highest_open_block(x,y);
        obj->set_position(x,y,z);
        obj->born();
        obj->spawn.type = OBJ_TYPE_MONSTER_BOX;
    }
}

} // Monsters
