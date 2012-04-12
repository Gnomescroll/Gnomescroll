#include "spawner.hpp"

#include <math.h>
#include <c_lib/common/random.h>
#include <c_lib/chat/client.hpp>
#include <net_lib/net.hpp>
#include <c_lib/common/quicksort.hpp>
#include <c_lib/agent/net_agent.hpp>

/* Packets */

#ifdef DC_CLIENT

void spawner_create(object_create_owner_team_index_StoC_model* msg)
{
    Spawner* s = (Spawner*)ClientState::object_list->create((Object_types)msg->type, (int)msg->id);
    if (s == NULL)
    {
        printf("WARNING spawner_create() -- could not create spawner %d\n", msg->id);
        return;
    }
    s->set_position(msg->x, msg->y, msg->z);
    s->set_team(msg->team);
    s->set_team_index((unsigned int)msg->team_index); //overwrite with server authority
    s->set_owner(msg->owner);
    s->born();
    Sound::spawner_placed(msg->x, msg->y, msg->z, 0,0,0);
    system_message->object_created(s);
}
#endif

/* Spawners */

VoxDat spawner_vox_dat;

/* SpawnerComponent */

void SpawnerComponent::get_spawn_point(Vec3 position, int spawned_object_height, int* spawn_pt)
{
    int x,y;
    x = (int)position.x;
    y = (int)position.y;

    int sx,sy;
    sx = randrange(x - this->spawner_properties.radius, x + this->spawner_properties.radius);
    sy = randrange(y - this->spawner_properties.radius, y + this->spawner_properties.radius);
    spawn_pt[0] = (sx < map_dim.x) ? sx : map_dim.x;
    spawn_pt[0] = (spawn_pt[0] < 0) ? 0 : spawn_pt[0];
    spawn_pt[1] = (sy < map_dim.y) ? sy : map_dim.y;
    spawn_pt[1] = (spawn_pt[1] < 0) ? 0 : spawn_pt[1];
    spawn_pt[2] = _get_highest_open_block(spawn_pt[0], spawn_pt[1], spawned_object_height);
}


/* List Manager */

bool SpawnerList::team_spawner_available(int team)
{
    ObjectPolicyInterface *s;
    int n = 0;
    for (int i=0; i<this->max; i++)
    {
        if (this->objects[i] == NULL) continue;
        s = this->objects[i]->obj;
        if (s == NULL) continue;
        if (s->get_team() == team) n++;
    }
    return (n < this->max_per_team);
}

int SpawnerList::get_random_spawner(int team)
{
    ObjectPolicyInterface *s;
    int objects[this->max_per_team+1];
    int j=0;
    for (int i=0; i<this->max; i++)
    {   // filter down to team's objects
        if (this->objects[i] == NULL) continue;
        s = this->objects[i]->obj;
        if (s == NULL) continue;
        if (s->get_team() == team)
            objects[j++] = s->get_team_index();
    }
    objects[j++] = BASE_SPAWN_ID;
    return objects[randrange(0,j-1)];
}

// when objects player says "spawner 8" he may be on the other team
// we need to find the 8th spawner for his team
int SpawnerList::get_numbered_team_spawner(int team, int id)
{
    ObjectPolicyInterface *s;
    for (int i=0; i<this->max; i++)
    {
        if (this->objects[i] == NULL) continue;
        s = this->objects[i]->obj;
        if (s == NULL) continue;
        if (s->get_team() != team) continue;
        if ((int)s->get_team_index() == id)
            return s->state()->id;
    }
    return BASE_SPAWN_ID;
}

ObjectPolicyInterface* SpawnerList::get_by_team_index(int team, int team_index)
{
    ObjectPolicyInterface *s;
    for (int i=0; i<this->max; i++)
    {
        if (this->objects[i] == NULL) continue;
        s = this->objects[i]->obj;
        if (s == NULL) continue;
        if (s->get_team() != team) continue;
        if ((int)s->get_team_index() == team_index)
            return s;
    }
    return NULL;
}

bool SpawnerList::spawner_exists(int team, int team_index)
{
    if (this->get_by_team_index(team, team_index) != NULL)
        return true;
    return false;
}

void SpawnerList::assign_team_index(ObjectPolicyInterface* spawner)
{   // pick an index for the spawner that is available, these are separate from
    // id because each team's set of objects has its own indexing
    // and objects may be destroyed; we dont want to renumber every time
    // get smallest available team index
    if (!this->max) return;
    int* taken = (int*)calloc(this->max, sizeof(int));
    int team_index = TEAM_INDEX_NONE;
    ObjectPolicyInterface* s;
    for (int i=0; i<this->max; i++)
    {
        if (this->objects[i] == NULL) continue;
        s = this->objects[i]->obj;
        if (s == NULL) continue;
        if (s->get_team() != spawner->get_team()) continue;
        if (spawner->get_team_index() != TEAM_INDEX_NONE
          && spawner->get_team_index() != 0)  // should never be 0, team_indexing starts at 1
            taken[spawner->get_team_index() - 1] = 1;
    }
    for (int i=0; i<this->max; i++)
        if (!taken[i])
        {
            team_index = i+1;
            break;
        }
    spawner->set_team_index(team_index);
    free(taken);
}

