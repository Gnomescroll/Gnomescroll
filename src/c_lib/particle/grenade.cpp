#include "grenade.hpp"

#if DC_CLIENT
#include <animations/animations.hpp>
#include <sound/triggers.hpp>
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

#include <entity/constants.hpp>
#include <common/random.h>

#include <particle/_interface.hpp>

/* properties */


namespace Particle
{


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


class grenade_StoC: public FixedSizeReliableNetPacketToClient<grenade_StoC>
{
    public:

        float x,y,z;
        float mx,my,mz; // send initial impulse, not velocity
        uint8_t owner;
        uint16_t ttl_max;
        uint8_t type;

        inline void packet(char* buff, unsigned int* buff_n, bool pack) 
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
    Grenade* g = Particle::grenade_list->create();
    if (g == NULL) return;
    g->set_state(x, y, z, mx, my, mz);
    g->owner = owner;
    g->ttl_max = (int)ttl_max;
    g->type = (int)type;
    #endif
}

void Grenade::reset()
{
    this->ttl_max = GRENADE_TTL;
    this->type = GRENADE_TYPE;
    this->texture_index = GRENADE_TEXTURE_ID;
    this->scale = GRENADE_TEXTURE_SCALE;
    this->verlet.dampening = GRENADE_DAMP;
}

Grenade::Grenade(int id)
:
ParticleMotion(id, 0,0,0,0,0,0, GRENADE_MASS),
BillboardSprite(),
bounce_count(0),
owner(-1)
{
    this->reset();
}

Grenade::~Grenade()
{
    #if DC_CLIENT
    Vec3 position = this->get_position();
    Sound::plasma_grenade_explode(
        position.x, position.y, position.z,
        0,0,0
    );
    #endif
}

#if DC_SERVER
void Grenade::broadcast()
{
    grenade_StoC msg;
    Vec3 position = this->get_position();
    msg.x = position.x;
    msg.y = position.y;
    msg.z = position.z;
    Vec3 momentum = this->get_momentum();
    msg.mx = momentum.x;
    msg.my = momentum.y;
    msg.mz = momentum.z;
    msg.ttl_max = this->ttl_max;
    msg.owner = this->owner;
    msg.type = this->type;
    msg.broadcast();
}
#endif

void Grenade::tick()
{

    bool bounced = this->verlet.bounce();
    if (bounced)
    {
        this->bounce_count++;
        #if DC_CLIENT
        Vec3 position = this->get_position();
        Sound::grenade_bounce(
            position.x,
            position.y,
            position.z,
            0,0,0
        );
        #endif
    }
    if (this->bounce_count >= GRENADE_BOUNCE_EXPLODE_LIMIT)
        this->ttl = this->ttl_max;
    this->ttl++;
}

void Grenade::explode()
{
    this->explode(1);
}

void Grenade::explode(int multiplier)
{
    Vec3 position = this->get_position();
    #if DC_CLIENT
    if (multiplier > 3) multiplier = 3;
    for (int i=0; i<multiplier; i++)
        Animations::grenade_explode(position.x, position.y, position.z);
    #endif

    #if DC_SERVER
    // this has to be called before damage_blocks(), unless you want the blast to go through blocks AND hit players newly exposed
    // leave this for other objects, but agents are damaged by shrapnel now

    ServerState::damage_objects_within_sphere(
        position.x, position.y, position.z,
        GRENADE_DAMAGE_RADIUS,
        GRENADE_SPLASH_DAMAGE*multiplier, this->owner, OBJECT_GRENADE, this->id
    );

    // apply block damage
    damage_blocks(multiplier);
    #endif
}

inline int Grenade::block_damage(int dist)
{
    int max_dist = GRENADE_BLOCK_DESTROY_RADIUS*3;
    if (dist >= max_dist) return 0;
    float ratio = (float)(max_dist - dist) / (float)(max_dist);
    float dmg = ratio * (float)(GRENADE_BLOCK_DAMAGE);
    int idmg = (int)(dmg);
    return idmg;
}

void Grenade::damage_blocks()
{
    this->damage_blocks(1);
}

void Grenade::damage_blocks(int multiplier)
{
    #if DC_SERVER
    using t_map::apply_damage_broadcast;
    const t_map::TerrainModificationAction action = t_map::TMA_GRENADE;

    Vec3 position = this->get_position();
    int mx = (int)position.x;
    int my = (int)position.y;
    int mz = (int)position.z;
    
    int ir = GRENADE_BLOCK_DESTROY_RADIUS;
    int bx,by,bz;
    int dmg = 0;

    for (int i=0; i<ir; i++)
    for (int j=0; j<ir; j++)
    for (int k=0; k<ir; k++)
    {
        bx = mx + i;
        by = my + j;
        bz = mz + k;
        if (bz <= 0) continue;  // dont damage floor
        if (bz >= map_dim.z) continue;  // dont damage floor

        bx = translate_point(bx);
        by = translate_point(by);
        
        dmg = block_damage(i+j+k)*multiplier;
        if (dmg <= 0) continue;
        
        apply_damage_broadcast(bx,by,bz, dmg, action);
        bx = translate_point(mx - i);
        apply_damage_broadcast(bx,by,bz, dmg, action);
        by = translate_point(my - j);
        apply_damage_broadcast(bx,by,bz, dmg, action);
        by = translate_point(my + j);
        bz = mz - k;
        if (bz > 0 && bz < map_dim.z)
            apply_damage_broadcast(bx,by,bz, dmg, action);
        bx = translate_point(mx + i);
        by = translate_point(my - j);
        apply_damage_broadcast(bx,by,bz, dmg, action);
        bx = translate_point(mx - i);
        apply_damage_broadcast(bx,by,bz, dmg, action);
    }
    #endif
}

/* Grenade list */
void Grenade_list::tick()
{
    if (num <= 0) return;
    for(int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if(a[i]->ttl >= a[i]->ttl_max)
        {
            a[i]->explode();
            destroy(a[i]->id);
        }
    }
}

void Grenade_list::draw()
{
    #if DC_CLIENT
    if (num <= 0) return;
    for(int i=0; i<n_max; i++)
        if (a[i] != NULL)
            a[i]->draw(a[i]->get_position());
    #endif
}

}
