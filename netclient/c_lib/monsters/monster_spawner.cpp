#include "monster_spawner.hpp"

#include <math.h>
#include <c_lib/common/random.h>
#include <c_lib/chat/client.hpp>
#include <net_lib/net.hpp>
#include <c_lib/common/quicksort.hpp>

namespace Monsters
{

/* Packets */

#ifdef DC_CLIENT
void monster_spawner_state(object_state_StoC_model* msg)
{
    MonsterSpawner* s = (MonsterSpawner*)ClientState::object_list->get((Object_types)msg->type, msg->id);
    if (s == NULL) return;
    s->set_position(msg->x, msg->y, msg->z);
}

void monster_spawner_create(object_create_owner_team_index_StoC_model* msg)
{
    MonsterSpawner* s = (MonsterSpawner*)ClientState::object_list->create((Object_types)msg->type, (int)msg->id);
    if (s == NULL)
    {
        printf("WARNING monster_spawner_create() -- could not create monster_spawner %d\n", msg->id);
        return;
    }
    s->set_position(msg->x, msg->y, msg->z);
    s->set_team(msg->team);
    s->set_team_index((unsigned int)msg->team_index); //overwrite with server authority
    s->set_owner(msg->owner);
    s->born();
    //Sound::monster_spawner_placed(msg->x, msg->y, msg->z, 0,0,0);
    system_message->object_created(s);
}
#endif

/* MonsterSpawners */

VoxDat monster_spawner_vox_dat;

/* MonsterSpawnerComponent */

void MonsterSpawnerComponent::get_spawn_point(Vec3 position, float spawned_object_height, float* spawn_pt)
{
    float sx,sy;
    sx = (randf() * this->radius * 2) - this->radius;
    sy = (randf() * this->radius * 2) - this->radius;
    spawn_pt[0] = (sx < map_dim.x - 1) ? sx : map_dim.x - 1;
    spawn_pt[0] = (spawn_pt[0] < 0) ? 0 : spawn_pt[0];
    spawn_pt[1] = (sy < map_dim.y - 1) ? sy : map_dim.y - 1;
    spawn_pt[1] = (spawn_pt[1] < 0) ? 0 : spawn_pt[1];
    int h = (int)ceil(spawned_object_height);
    spawn_pt[2] = _get_highest_open_block((int)spawn_pt[0], (int)spawn_pt[1], h);
}

} // Monsters
