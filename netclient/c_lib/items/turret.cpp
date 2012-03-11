#include "turret.hpp"

#include <c_lib/common/random.h>
#include <math.h>

#include <c_lib/agent/net_agent.hpp>
#include <net_lib/net.hpp>

/* Packets */
class turret_create_StoC: public FixedSizeReliableNetPacketToClient<turret_create_StoC>
{
    public:
        uint8_t id;
        uint8_t owner;
        uint8_t team;
        float x,y,z;
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&owner, buff, buff_n, pack);
            pack_u8(&team, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
        }
        inline void handle();
};

class turret_destroy_StoC: public FixedSizeReliableNetPacketToClient<turret_destroy_StoC>
{
    public:
        uint8_t id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class turret_state_StoC: public FixedSizeReliableNetPacketToClient<turret_state_StoC>
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
inline void turret_state_StoC::handle()
{
    
    Turret* t = ClientState::turret_list->get(id);
    if (t==NULL) return;
    t->set_position(x,y,z);
}

inline void turret_create_StoC::handle()
{
    Turret* t = ClientState::turret_list->create(id, x,y,z);
    if (t==NULL)
    {
        printf("WARNING turret_create_StoC::handle() -- could not create turret %d\n", id);
        return;
    }
    t->set_team(team);
    t->set_owner(owner);
    t->init_vox();
    //Sound::turret_placed(x,y,z,0,0,0);
    //system_message->turret_created(t);
}

inline void turret_destroy_StoC::handle()
{
    //turret* t = ClientState::turret_list->get(id);
    //system_message->turret_destroyed(t);
    ClientState::turret_list->destroy(id);
}
#endif

#ifdef DC_SERVER
inline void turret_state_StoC::handle(){}
inline void turret_create_StoC::handle(){}
inline void turret_destroy_StoC::handle(){}
#endif


/* Turrets */

VoxDat turret_vox_dat;

void Turret::set_position(float x, float y, float z)
{
    if (this->x == x && this->y == y && this->z == z) return;

    this->x = x;
    this->y = y;
    this->z = z;
    this->vox->update(&turret_vox_dat, this->x, this->y, this->z, this->theta, this->phi);

    #ifdef DC_SERVER
    turret_state_StoC msg;
    msg.x = this->x;
    msg.y = this->y;
    msg.z = this->z;
    msg.id = this->id;
    msg.broadcast();
    #endif
}

int Turret::get_team()
{
    return this->team;
}

void Turret::set_owner(int owner)
{
    this->owner = owner;
}

void Turret::set_team(int team)
{
    this->team = team;
    if (this->vox != NULL) this->vox->update_team_color(&turret_vox_dat, this->team);
}

void Turret::init_vox()
{
    if (this->team == 0) printf("WARNING Turret::init_vox() -- team not set\n");
    this->vox = new Voxel_model(&turret_vox_dat, this->id, this->type, this->team);
    this->vox->set_hitscan(true);
    this->vox->register_hitscan();
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
    this->vox->update(&turret_vox_dat, this->x, this->y, this->z, this->theta, this->phi);
}

void Turret::update()
{
    #ifdef DC_CLIENT
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
        this->vox->update(&turret_vox_dat, this->x, this->y, this->z, this->theta, this->phi);
    #endif
}

Turret::Turret(int id)
:
team(0),
id(id),
owner(0),
type(OBJ_TYPE_TURRET),
x(0), y(0), z(0),
theta(0), phi(0),
vox(NULL)
{
}
Turret::Turret(int id, float x, float y, float z)
:
team(0),
id(id),
owner(0),
health(TURRET_HEALTH),
type(OBJ_TYPE_TURRET),
x(x), y(y), z(z),
theta(0), phi(0),
vox(NULL)
{
}

#ifdef DC_SERVER
void Turret::create_message(turret_create_StoC* msg)
{
    msg->id = this->id;
    msg->team = this->team;
    msg->owner = this->owner;
    msg->x = this->x;
    msg->y = this->y;
    msg->z = this->z;
}
#endif

int Turret::get_coins_for_kill(int owner, int team)
{
    if (this->team != team || owner == this->owner) // enemy team, or owner, can destroy/reclaim turret
        return get_object_cost(this->type);
    return 0;
}

int Turret::take_damage(int dmg)
{
    this->health -= dmg;
    this->health = (this->health < 0) ? 0 : this->health;
    return this->health;
}

void Turret::tick()
{
    if (this->health <= 0)
    {
        STATE::turret_list->destroy(this->id);
        return;
    }
    
#ifdef DC_SERVER
    int x,y,z;
    x = (int)this->x;
    y = (int)this->y;
    z = (int)this->z;
    if (isSolid(_get(x,y,z)))
    {   // move up
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
    {   // fall down
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


Turret::~Turret()
{
    #ifdef DC_SERVER
    turret_destroy_StoC msg;
    msg.id = this->id;
    msg.broadcast();
    #endif
    if (this->vox != NULL) delete this->vox;
}

/* Turret list */

bool Turret_list::point_occupied(int x, int y, int z)
{
    for (int i=0; i<n_max; i++)
    {
        Turret *s = this->a[i];
        if (s == NULL) continue;
        if ((int)s->x == x && (int)s->y == y)
        {
            // turret is 2 blocks tall
            for (int j=0; j<(int)ceil(TURRET_HEIGHT); j++)
            {
                if ((int)s->z+j == z) return true;
            }
        }
    }
    return false;
}

#ifdef DC_SERVER
void Turret_list::send_to_client(int client_id)
{
    for (int i=0; i<this->n_max; i++)
    {
        Turret *s = this->a[i];
        if (s == NULL) continue;

        turret_create_StoC msg;
        s->create_message(&msg);
        msg.sendToClient(client_id);
    }
}
#endif

void Turret_list::tick()
{
    for (int i=0; i<n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        this->a[i]->tick();
    }
}

void Turret_list::update()
{
    for (int i=0; i<n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        this->a[i]->update();
    }
}


