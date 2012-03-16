#include "grenade.hpp"

#if DC_CLIENT
#include <c_lib/animations/animations.hpp>
#include <sound/triggers.hpp>
#endif
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>
#include <c_lib/state/client_state.hpp>
#include <c_lib/state/server_state.hpp>
#include <common/enum_types.hpp>
#include <c_lib/common/random.h>

/* properties */

const int GRENADE_TTL = 100;
const float GRENADE_DAMP = 0.5f;
const int GRENADE_TYPE = 1;
const int GRENADE_TEXTURE_ID = 5;
const float GRENADE_TEXTURE_SCALE = 1.0f;
const int GRENADE_BOUNCE_EXPLODE_LIMIT = 2;
const float GRENADE_MASS = 0.5f;


/*
 *  Networking; spawn packet from server to client
 */

#include <net_lib/net.hpp>

class grenade_StoC: public FixedSizeNetPacketToClient<grenade_StoC>
{
    public:

        float x,y,z;
        float mx,my,mz; // send initial impulse, not velocity
        uint8_t owner;
        uint16_t ttl_max;
        uint8_t type;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);

            pack_float(&mx, buff, buff_n, pack);
            pack_float(&my, buff, buff_n, pack);
            pack_float(&mz, buff, buff_n, pack);

            pack_u8(&owner, buff, buff_n, pack);
            pack_u16(&ttl_max, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
        }
        inline void handle();
};

inline void grenade_StoC::handle()
{
    #if DC_CLIENT
    if (owner == ClientState::playerAgent_state.agent_id) return;
    Grenade* g = ClientState::grenade_list->create(x, y, z, mx, my, mz);
    g->owner = owner;
    g->ttl_max = (int)ttl_max;
    g->type = (int)type;
    #endif
}

void Grenade::init()
{
    this->ttl_max = GRENADE_TTL;
    this->type = GRENADE_TYPE;
    this->texture_index = GRENADE_TEXTURE_ID;
    this->scale = GRENADE_TEXTURE_SCALE;    
}

Grenade::Grenade(int id)
:
Particle(id, 0,0,0,0,0,0, GRENADE_MASS),
BillboardSprite(),
bounce_count(0),
owner(-1)
{
    this->init();
}

Grenade::Grenade(int id, float x, float y, float z, float mx, float my, float mz)
:
Particle(id, x,y,z,mx,my,mz, GRENADE_MASS),
BillboardSprite(),
bounce_count(0),
owner(-1)
{
    this->init();
}

Grenade::~Grenade()
{
    #if DC_CLIENT
    Sound::grenade_explode(
        this->vp->p.x, this->vp->p.y, this->vp->p.z,
        0,0,0
    );
    #endif
}

#if DC_SERVER
void Grenade::broadcast()
{
    grenade_StoC msg;
    msg.x = this->vp->p.x;
    msg.y = this->vp->p.y;
    msg.z = this->vp->p.z;
    Vec3 mom = this->vp->get_momentum();
    msg.mx = mom.x;
    msg.my = mom.y;
    msg.mz = mom.z;
    msg.ttl_max = this->ttl_max;
    msg.owner = this->owner;
    msg.type = this->type;
    msg.broadcast();
}
#endif

void Grenade::tick()
{

    bool bounced = Verlet::bounce(this->vp, GRENADE_DAMP);
    if (bounced)
    {
        bounce_count++;
        #if DC_CLIENT
        Sound::grenade_bounce(
            this->vp->p.x,
            this->vp->p.y,
            this->vp->p.z,
            0,0,0
        );
        #endif
    }
    if (bounce_count==GRENADE_BOUNCE_EXPLODE_LIMIT)
        this->ttl = this->ttl_max;
    this->ttl++;
}

void Grenade::explode()
{
#if DC_CLIENT
    Animations::grenade_explode(this->vp->p.x, this->vp->p.y, this->vp->p.z);
#endif

#if DC_SERVER
    // this has to be called before damage_blocks(), unless you want the blast to go through blocks AND hit players newly exposed

    // leave this for other objects, but agents are damaged by shrapnel now
    ServerState::damage_objects_within_sphere(
        this->vp->p.x, this->vp->p.y, this->vp->p.z,
        GRENADE_DAMAGE_RADIUS,
        GRENADE_SPLASH_DAMAGE, this->owner, OBJ_TYPE_GRENADE
    );

    // create a bunch of grenade shrapnel particles
    // copied/modified from Animations::grenade_explode

    const int n = 16;
    const float vel = 30.0f;
    //const Vec3 v = vec3_init(vel,vel,vel);
    //const Vec3 cv = vec3_init(vel,vel,vel);

    Grenade_shrapnel* g;
    Vec3 cv;
    for (int i=0; i<n; i++)
    {
        cv = vec3_init(randf()-0.5, randf()-0.5, randf()-0.5);
        normalize_vector(&cv);
        cv = vec3_scalar_mult(cv, vel);
        g = ServerState::grenade_shrapnel_list->create(
            this->vp->p.x, this->vp->p.y, this->vp->p.z,
            cv.x, cv.y, cv.z
        );
        if (g == NULL) break;
        g->owner = this->owner;
    }
    
    // apply block damage
    damage_blocks();
#endif
}

int Grenade::block_damage(int dist)
{
    int max_dist = GRENADE_BLOCK_DESTROY_RADIUS*3;
    float ratio = (float)(max_dist - dist) / (float)(max_dist);
    float dmg = ratio * (float)(GRENADE_BLOCK_DAMAGE);
    int idmg = (int)(dmg);
    return idmg;
}

void Grenade::damage_blocks()
{
#if DC_SERVER
    const t_map::TerrainModificationAction action = t_map::TMA_GRENADE;
    using t_map::apply_damage_broadcast;
    
    float x = this->vp->p.x;
    float y = this->vp->p.y;
    float z = this->vp->p.z;

    int ir = GRENADE_BLOCK_DESTROY_RADIUS;

    int mx = (int)x;
    int my = (int)y;
    int mz = (int)z;
    
    int bx,by,bz;
    int dmg=0;
    for (int i=0; i<ir; i++)
        for (int j=0; j<ir; j++)
            for (int k=0; k<ir; k++)
            {
                dmg = block_damage(i+j+k);

                bx = mx + i;
                by = my + j;
                bz = mz + k;
                apply_damage_broadcast(bx,by,bz, dmg, action);
                bx = mx - i;
                apply_damage_broadcast(bx,by,bz, dmg, action);
                by = my - j;
                apply_damage_broadcast(bx,by,bz, dmg, action);
                by = my + j;
                bz = mz - k;
                apply_damage_broadcast(bx,by,bz, dmg, action);
                bx = mx + i;
                by = my - j;
                apply_damage_broadcast(bx,by,bz, dmg, action);
                bx = mx - i;
                apply_damage_broadcast(bx,by,bz, dmg, action);
            }
#endif
}

/* Grenade list */
void Grenade_list::tick()
{
    for(int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if(a[i]->ttl >= a[i]->ttl_max)
        {
            a[i]->explode();
            destroy(a[i]->id);
            num--;
        }
    }
}

void Grenade_list::draw()
{
#if DC_CLIENT
    if(num == 0) return;
    for(int i=0; i<n_max; i++)
        if (a[i] != NULL)
            a[i]->draw(a[i]->vp->p);
#endif
}

inline void print_grenade(Grenade *g)
{
    printf("Print Grenade -\n");
    printf("ID: %d\n", g->id);
    printf("Pos: %0.2f %0.2f %0.2f\n", g->vp->p.x, g->vp->p.y, g->vp->p.z);
    printf("Vel: %0.2f %0.2f %0.2f\n", g->vp->v.x, g->vp->v.y, g->vp->v.z);
    printf("TTL max: %d\n", g->ttl_max);
    printf("Type: %d\n", g->type);
}
