#include "spawner.hpp"

#include <math.h>
#include <c_lib/common/random.h>
#include <c_lib/chat/client.hpp>
#include <net_lib/net.hpp>
#include <c_lib/common/quicksort.hpp>
#include <c_lib/agent/net_agent.hpp>

/* Packets */

#ifdef DC_CLIENT
void spawner_state(object_state_StoC_model* msg)
{
    
    Spawner* s = (Spawner*)ClientState::object_list->get(msg->id);
    if (s == NULL) return;
    s->state()->set_position(msg->x, msg->y, msg->z);
}

void spawner_create(object_create_owner_team_index_StoC_model* msg)
{
    Spawner* s = (Spawner*)ClientState::object_list->create(
        (int)msg->id,
        msg->x, msg->y, msg->z,
        0,0,0,
        (Object_types)msg->type
    );
    if (s == NULL)
    {
        printf("WARNING spawner_create() -- could not create spawner %d\n", msg->id);
        return;
    }
    s->state()->set_team(msg->team);
    s->state()->team_index = msg->team_index; //overwrite with server authority
    s->state()->set_owner(msg->owner);
    s->born();
    Sound::spawner_placed(msg->x, msg->y, msg->z, 0,0,0);
    system_message->object_created(s);
}

void spawner_destroy(int id)
{
    Spawner* s = (Spawner*)ClientState::object_list->get(id);
    if (s != NULL)
        system_message->object_destroyed(s);
    ClientState::object_list->destroy(id);
}
#endif

/* Spawners */

VoxDat spawner_vox_dat;

/* SpawnerComponent */

void SpawnerComponent::get_spawn_point(ObjectState* state, int spawned_object_height, int* spawn_pt)
{
    int x,y;
    Vec3 p = state->get_position();
    x = (int)p.x;
    y = (int)p.y;

    int sx,sy;
    sx = randrange(x-state->spawn_radius, x+state->spawn_radius);
    sy = randrange(y-state->spawn_radius, y+state->spawn_radius);
    spawn_pt[0] = (sx < map_dim.x) ? sx : map_dim.x;
    spawn_pt[0] = (spawn_pt[0] < 0) ? 0 : spawn_pt[0];
    spawn_pt[1] = (sy < map_dim.y) ? sy : map_dim.y;
    spawn_pt[1] = (spawn_pt[1] < 0) ? 0 : spawn_pt[1];
    spawn_pt[2] = _get_highest_open_block(spawn_pt[0], spawn_pt[1], spawned_object_height);
}
