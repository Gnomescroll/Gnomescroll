#include "spawner.hpp"

#include <c_lib/common/random.h>
#include <math.h>

#include <c_lib/agent/net_agent.hpp>

VoxDat spawner_vox_dat;

#include <net_lib/net.hpp>

class spawner_state_StoC: public FixedSizeReliableNetPacketToClient<spawner_state_StoC>
{
    public:
        int id;
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
    
    Spawner* s = ClientState::spawner_list.get(id);
    if (s==NULL) return;
    s->set_position(x,y,z);
}
#endif

#ifdef DC_SERVER
inline void spawner_state_StoC::handle(){}
#endif


void Spawner::set_position(float x, float y, float z)
{
    if (this->x == x && this->y == y && this->z == z) return;

    this->x = x;
    this->y = y;
    this->z = z;
    this->vox->update(&spawner_vox_dat, this->x, this->y, this->z, this->theta, this->phi);

    #ifdef DC_SERVER
    spawner_state_StoC msg;
    msg.x = this->x;
    msg.y = this->y;
    msg.z = this->z;
    msg.id = this->id;
    msg.broadcast();
    #endif
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

void Spawner::set_owner(int owner)
{
    this->owner = owner;
}

void Spawner::set_team(int team)
{
    this->team = team;
    if (this->vox != NULL) this->vox->update_team_color(&spawner_vox_dat, this->team);
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
    this->vox->update(&spawner_vox_dat, this->x, this->y, this->z, this->theta, this->phi);
}

Spawner::Spawner(int id)
:
id(id),
team(0),
owner(0),
type(OBJ_TYPE_SPAWNER),
x(0), y(0), z(0),
theta(0), phi(0),
spawn_radius(SPAWNER_RADIUS),
vox(NULL)
{}
Spawner::Spawner(int id, float x, float y, float z)
:
id(id),
team(0),
owner(0),
health(SPAWNER_HEALTH),
type(OBJ_TYPE_SPAWNER),
x(x), y(y), z(z),
theta(0), phi(0),
spawn_radius(SPAWNER_RADIUS),
vox(NULL)
{}

void Spawner::create_message(Spawner_create_StoC* msg)
{
    #ifdef DC_SERVER
    msg->id = this->id;
    msg->team = this->team;
    msg->owner = this->owner;
    msg->x = this->x;
    msg->y = this->y;
    msg->z = this->z;
    #endif
}

int Spawner::get_coins_for_kill(int owner, int team)
{
    if (this->team != team || owner == this->owner) // enemy team, or owner, can destroy/reclaim spawner
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
        STATE::spawner_list.destroy(this->id);
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
    if (this->vox != NULL) delete this->vox;
}

/* Spawner list */

bool Spawner_list::team_spawner_available(int team)
{
    int ct = 0;
    for (int i=0; i<n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->team == team) ct++;
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
        if (s->team == team) spawners[j++] = i;
    }
    spawners[j++] = BASE_SPAWN_ID;
    return spawners[randrange(0,j-1)];
}

void Spawner_list::send_to_client(int client_id)
{
    #ifdef DC_SERVER
    for (int i=0; i<n_max; i++)
    {
        Spawner *s = this->a[i];
        if (s == NULL) continue;

        Spawner_create_StoC msg;
        s->create_message(&msg);
        msg.sendToClient(client_id);
    }
    #endif
}

void Spawner_list::tick()
{
    for (int i=0; i<n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        this->a[i]->tick();
    }
}
