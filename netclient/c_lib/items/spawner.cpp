#include "spawner.hpp"

#include <math.h>
#include <c_lib/common/random.h>
#include <c_lib/chat/client.hpp>
#include <net_lib/net.hpp>
#include <c_lib/common/quicksort.hpp>
#include <c_lib/agent/net_agent.hpp>

/* Packets */
class Spawner_create_StoC: public FixedSizeReliableNetPacketToClient<Spawner_create_StoC>
{
    public:
        uint8_t id;
        uint8_t owner;
        uint8_t team;
        int8_t team_index;
        float x,y,z;
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&owner, buff, buff_n, pack);
            pack_u8(&team, buff, buff_n, pack);
            pack_8(&team_index, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
        }
        inline void handle();
};

class Spawner_destroy_StoC: public FixedSizeReliableNetPacketToClient<Spawner_destroy_StoC>
{
    public:
        uint8_t id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class spawner_state_StoC: public FixedSizeReliableNetPacketToClient<spawner_state_StoC>
{
    public:
        uint8_t id;
        float x,y,z;
        //float vx,vy,vz;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
        }
        inline void handle();
};


#ifdef DC_CLIENT
inline void spawner_state_StoC::handle()
{
    
    Spawner* s = ClientState::spawner_list->get(id);
    if (s==NULL) return;
    s->set_position(x,y,z);
}

inline void Spawner_create_StoC::handle()
{
    Spawner* s = ClientState::spawner_list->create(id, x,y,z);
    if (s==NULL)
    {
        printf("WARNING Spawner_create_StoC::handle() -- could not create spawner %d\n", id);
        return;
    }
    s->set_team(team);
    s->team_index = team_index; //overwrite with server authority
    s->set_owner(owner);
    s->init_vox();
    Sound::spawner_placed(x,y,z,0,0,0);
    // TODO -- use object_* after Spawner is ObjectPolicyInterface
    //system_message->spawner_created(s);
    //system_message->object_created(s);
}

inline void Spawner_destroy_StoC::handle()
{
    //Spawner* s = ClientState::spawner_list->get(id);
    //if (s != NULL)    // TODO -- use object_* after Spawner is ObjectPolicyInterface
        //system_message->spawner_destroyed(s);
        //system_message->object_destroyed(s);
    ClientState::spawner_list->destroy(id);
}
#endif

#ifdef DC_SERVER
inline void spawner_state_StoC::handle(){}
inline void Spawner_create_StoC::handle(){}
inline void Spawner_destroy_StoC::handle(){}
#endif

/* Spawners */

VoxDat spawner_vox_dat;

void Spawner::set_position(float x, float y, float z)
{
    if (this->x == x && this->y == y && this->z == z) return;

    this->x = x;
    this->y = y;
    this->z = z;

    this->vox->thaw();
    this->vox->update(this->x, this->y, this->z, this->theta, this->phi);
    this->vox->freeze();

    #ifdef DC_SERVER
    spawner_state_StoC msg;
    msg.x = this->x;
    msg.y = this->y;
    msg.z = this->z;
    msg.id = this->id;
    msg.broadcast();
    #endif
}

int Spawner::get_team()
{
    return this->team;
}

void Spawner::get_spawn_point(int agent_height, int* spawn)
{
    int x,y;
    x = (int)this->x;
    y = (int)this->y;

    int sx,sy;
    sx = randrange(x-spawn_radius, x+spawn_radius);
    sy = randrange(y-spawn_radius, y+spawn_radius);
    spawn[0] = (sx < map_dim.x) ? sx : map_dim.x;
    spawn[0] = (spawn[0] < 0) ? 0 : spawn[0];
    spawn[1] = (sy < map_dim.y) ? sy : map_dim.y;
    spawn[1] = (spawn[1] < 0) ? 0 : spawn[1];
    spawn[2] = _get_highest_open_block(spawn[0], spawn[1], agent_height);
}

int Spawner::get_owner()
{
    return this->owner;
}

void Spawner::set_owner(int owner)
{
    switch_agent_ownership(this->type, this->owner, owner);
    this->owner = owner;
}

void Spawner::set_team(int team)
{
    this->team = team;
    STATE::spawner_list->assign_team_index(this);
    if (this->vox != NULL) this->vox->update_team_color(this->team);
}

void Spawner::init_vox()
{
    if (this->team == 0) printf("WARNING Spawner::init_vox() -- team not set\n");
    this->vox = new Voxel_model(&spawner_vox_dat, this->id, this->type, this->team);
    this->vox->set_hitscan(true);
    this->vox->register_hitscan();
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
    this->vox->thaw();
    this->vox->update(this->x, this->y, this->z, this->theta, this->phi);
    this->vox->freeze();
}

void Spawner::update()
{
    if (this->vox == NULL) return;
    #if DC_CLIENT
    this->vox->was_updated = false;
    if (current_camera == NULL || !current_camera->in_view(x,y,z))
    {
        this->vox->set_draw(false);
        this->vox->set_hitscan(false);
    }
    else
    {
        this->vox->set_draw(true);
        this->vox->set_hitscan(true);
    }
    if (input_state.skeleton_editor)
    {
        this->vox->thaw();
        this->vox->update(this->x, this->y, this->z, this->theta, this->phi);
        this->vox->freeze();
    }
    #endif

    #if DC_SERVER
    this->vox->was_updated = false;
    this->vox->set_hitscan(true);
    #endif
}

Spawner::Spawner(int id)
:
team(0),
owner(0),
id(id),
team_index(-1),
type(OBJ_TYPE_SPAWNER),
x(0), y(0), z(0),
theta(0), phi(0),
spawn_radius(SPAWNER_RADIUS),
vox(NULL)
{
}
Spawner::Spawner(int id, float x, float y, float z)
:
team(0),
owner(0),
id(id),
team_index(-1),
health(SPAWNER_HEALTH),
type(OBJ_TYPE_SPAWNER),
x(x), y(y), z(z),
theta(0), phi(0),
spawn_radius(SPAWNER_RADIUS),
vox(NULL)
{
}

#ifdef DC_SERVER
void Spawner::create_message(Spawner_create_StoC* msg)
{
    msg->id = this->id;
    msg->team = this->team;
    msg->owner = this->owner;
    msg->team_index = this->team_index;
    msg->x = this->x;
    msg->y = this->y;
    msg->z = this->z;
}
#endif

int Spawner::get_coins_for_kill(int owner, int team)
{
    if ((this->team != team && this->owner != NO_AGENT) || owner == this->owner) // enemy team, or owner, can destroy/reclaim spawner
        return get_object_cost(this->type);
    return 0;
}

int Spawner::take_damage(int dmg)
{
    this->health -= dmg;
    this->health = (this->health < 0) ? 0 : this->health;
    return this->health;
}

void Spawner::tick()
{
    if (this->health <= 0)
    {
        STATE::spawner_list->destroy(this->id);
        return;
    }
    
#ifdef DC_SERVER
    int x,y,z;
    x = (int)this->x;
    y = (int)this->y;
    z = (int)this->z;
    if (isSolid(_get(x,y,z)))
    {
        // move up
        while (isSolid(_get(x,y,++z)))
        {
            if (z >= map_dim.z)
            {
                z = map_dim.z;
                break;
            }
        }
    }
    else
    {
        // fall down
        while (!isSolid(_get(x,y,--z)))
        {
            if (z<=0)
            {
                z = 0;
                break;
            }
        }
        z++;
    }
    this->set_position(this->x, this->y, (float)z);
#endif
}


Spawner::~Spawner()
{
    #ifdef DC_SERVER
    Spawner_destroy_StoC msg;
    msg.id = this->id;
    msg.broadcast();
    #endif

    #if DC_CLIENT
    Animations::team_item_explode(this->vox->get_part(0)->get_center(), this->team);
    #endif

    if (this->owner != NO_AGENT)
    {
        Agent_state* a = STATE::agent_list->get(owner);
        if (a != NULL)
            a->status.lose_item(this->type);
    }
    if (this->vox != NULL) delete this->vox;

}

/* Spawner list */

bool Spawner_list::team_spawner_available(int team)
{
    int ct = 0;
    for (int i=0; i<n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->get_team() == team) ct++;
    }
    return (ct < SPAWNERS_PER_TEAM);
}

bool Spawner_list::point_occupied(int x, int y, int z)
{
    for (int i=0; i<n_max; i++)
    {
        Spawner *s = this->a[i];
        if (s == NULL) continue;
        if ((int)s->x == x && (int)s->y == y)
        {
            // spawner is 2 blocks tall
            for (int j=0; j<(int)ceil(SPAWNER_HEIGHT); j++)
            {
                if ((int)s->z+j == z) return true;
            }
        }
    }
    return false;
}

int Spawner_list::get_random_spawner(int team)
{
    int spawners[SPAWNERS_PER_TEAM+1];
    int j=0;
    for (int i=0; i<n_max; i++)
    {   // filter down to team's spawners
        Spawner *s = this->a[i];
        if (s == NULL) continue;
        if (s->get_team() == team) spawners[j++] = s->team_index;
    }
    spawners[j++] = BASE_SPAWN_ID;
    return spawners[randrange(0,j-1)];
}

// when a player says "spawner 8" he may be on the other team
// we need to find the 8th spawner for his team
int Spawner_list::get_numbered_team_spawner(int team, int id)
{
    for (int i=0; i<this->n_max; i++)
    {
        Spawner *s = this->a[i];
        if (s == NULL) continue;
        if (s->get_team() != team) continue;
        if (s->team_index == id)
        {
            return s->id;
        }
    }
    return BASE_SPAWN_ID;
}

#ifdef DC_SERVER
void Spawner_list::send_to_client(int client_id)
{
    for (int i=0; i<this->n_max; i++)
    {
        Spawner *s = this->a[i];
        if (s == NULL) continue;

        Spawner_create_StoC msg;
        s->create_message(&msg);
        msg.sendToClient(client_id);
    }
}
#endif

Spawner* Spawner_list::get_by_team_index(int team, int team_index)
{
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->get_team() != team) continue;
        if (this->a[i]->team_index == team_index)
            return this->a[i];
    }
    return NULL;
}

bool Spawner_list::spawner_exists(int team, int team_index)
{
    if (this->get_by_team_index(team, team_index) != NULL)
        return true;
    return false;
}

void Spawner_list::assign_team_index(Spawner* spawner)
{   // pick an index for the spawner that is available, these are separate from
    // id because each team's set of spawners has its own indexing
    // and spawners may be destroyed; we dont want to renumber every time

    // get smallest available team index
    int taken[MAX_SPAWNERS] = {0};
    for (int i=0; i<this->n_max; i++)
    {
        Spawner* s = this->a[i];
        if (s == NULL) continue;
        if (s->get_team() != spawner->get_team()) continue;
        if (s->team_index != -1 && s->team_index != 0)  // should never be 0
            taken[s->team_index - 1] = 1;
    }
    for (int i=0; i<MAX_SPAWNERS; i++)
        if (!taken[i])
        {
            spawner->team_index = i+1;
            return;
        }
    printf("failed to get team index\n");
    spawner->team_index = -1;
}

void Spawner_list::tick()
{
    for (int i=0; i<n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        this->a[i]->tick();
    }
}

void Spawner_list::update()
{
    for (int i=0; i<n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        this->a[i]->update();
    }
}

void Spawner_list::alter_owner(int owner, int new_owner)
{
    #if DC_SERVER
    if (owner == new_owner)
    {
        printf("WARNING -- Spawner_list::alter_owner -- owner == new_owner %d\n", owner);
        return;
    }
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->get_owner() != owner) continue;
        this->a[i]->set_owner(new_owner);
        alter_item_ownership_StoC msg;
        msg.owner = new_owner;
        msg.id = this->a[i]->id;
        msg.type = this->a[i]->type;
        msg.broadcast();
    }
    #endif
}
